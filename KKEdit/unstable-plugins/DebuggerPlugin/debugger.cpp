/*
  debugger.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <libgen.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <spawn.h>
#include <poll.h>
#include <signal.h>

#include <gdk/gdkkeysyms.h>

#include <libintl.h>
#include <locale.h>

#include "../config.h"
#include "../common.h"
#include "../kkedit-plugins.h"

#define PLUGVERSION VERSION
#ifdef _USEGTK3_
#define TEXTDOMAIN "Debugger-3"
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "Debugger" 
#define ABOUTICON "KKEditPlug"
#endif

#define MAXBUFSIZE 8192

struct varData
{
	bool	isPtr=false;
	bool	isStr=false;
	bool	isStruct=false;
};

char		buffer[MAXBUFSIZE]={0,};
FILE		*to,*from;
/* Pipes connected to gdb. */
int			to_gdb[2];
int			from_gdb[2];
pid_t		xtermpid;
GtkWidget	*command;
char		*instance;
char		*sinkReturnStr;
unsigned	lastline=0;
char		*currentfile=strdup("");
int			childPid=-1;
const char	*keywords[]={"fullname=","line=","value=",NULL};
char		*fullPath=NULL;
char		*fileName=NULL;
char		*lineNumber=NULL;
char		*value=NULL;
bool		running=false;

char		*prefsPath;
GtkWidget	*menuPlug;
char		*currentdomain=NULL;
int			sinkInt;
plugData	*thisPlugData;
char		*dataPath;

//menus
GtkWidget	*hideMenu=NULL;
bool		showing=false;

//debugger controls
enum {RUNNAPP=1,NEXTLINE,STEPINTO,STEPOUTOF,EXITLOOP,INTAPP,CONTINUE,SHOWBACK,GETBP,SETBP,CLEARBP,GETVAR,UPDATEVAR,RUNGDBCOM,OPENPATH,EXITAPP};

GtkWidget	*toolBarBox;
GtkWidget	*debuggerVBox;
GtkToolbar	*debuggerToolbar=NULL;
GtkToolItem	*runAppButton=NULL;
GtkToolItem	*quitAppButton=NULL;
GtkToolItem	*nextAppButton=NULL;
GtkToolItem	*stepIntoAppButton=NULL;
GtkToolItem	*stepOutAppButton=NULL;
GtkToolItem	*endLoopAppButton=NULL;
GtkToolItem	*intAppButton=NULL;
GtkToolItem	*contAppButton=NULL;
GtkToolItem	*backtraceAppButton=NULL;

//boxes
//breakpoints
GtkToolItem	*getBreakPointButton=NULL;
GtkWidget	*getBreakPointEntry=NULL;
GtkToolItem	*getBreakPointEntryTI=NULL;
GtkToolItem	*setBreakPointButton=NULL;
GtkToolItem	*unsetBreakPointButton=NULL;
//vars
GtkToolItem	*getVarButton=NULL;
GtkWidget	*getVarEntry=NULL;
GtkToolItem	*getVarEntryTI=NULL;
GtkToolItem	*updateVarButton=NULL;
//gdb command
GtkToolItem	*gdbCommandButton=NULL;
GtkWidget	*gdbCommandEntry=NULL;
GtkToolItem	*gdbCommandEntryTI=NULL;
//app to debug
GtkToolItem	*debugApp=NULL;
GtkWidget	*debugAppEntry=NULL;
GtkToolItem	*debugAppEntryTI=NULL;

int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	return(NULL);
}

void setTextDomain(bool plugdomain,plugData* pdata)
{
	if(plugdomain==true)
		{
			//set domain to plug
			bindtextdomain(TEXTDOMAIN,LOCALEDIR);
			textdomain(TEXTDOMAIN);
			bind_textdomain_codeset(TEXTDOMAIN,"UTF-8");
		}
	else
		{
			//resetdomain
			bindtextdomain(currentdomain,pdata->locale);
			textdomain(currentdomain);
			bind_textdomain_codeset(currentdomain,"UTF-8");	
		}
}

void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault at address %p\n", si->si_addr);
    exit(0);
}

void showHideDebugger(plugData* plugdata,bool startup)
{
	setTextDomain(true,plugdata);
	if(showing==true)
		{
			if(plugdata->bottomShow==0 && startup==false)
				showTop(false);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Hide Debugger"));
			showToolOutput(true);
		}
	else
		{
			if(plugdata->bottomShow==1 && startup==false)
				hideTop(false);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Show Debugger"));
			hideToolOutput(true);
		}
	setTextDomain(false,plugdata);
}

void toggleDebugger(GtkWidget* widget,gpointer data)
{
	showing=!showing;
	showHideDebugger((plugData*)data,false);
}

int stopChild(void)
{
	fputs("info proc\n",to);
	fflush(to);
	buffer[0]=0;
	usleep(10000);

	while(fgets((char*)&buffer,MAXBUFSIZE-1,from))
		{
			if(strstr(buffer,"process")!=NULL)
				return(atoi(&buffer[10]));
			buffer[0]=0;
		}
	return(-1);
}

void selectTabByPath(const char *path)
{
	pageStruct	*page;

	for(int j=-0;j<gtk_notebook_get_n_pages(thisPlugData->notebook);j++)
		{
			page=getPageStructByIDFromPage(j);
			if(page!=NULL)
				{
					if(strcmp(page->filePath,path)==0)
						{
							gtk_notebook_set_current_page(thisPlugData->notebook,j);
							break;
						}
				}
		}
}

void setLineMarker(const char *file,unsigned long nl)
{
	if(strcmp(currentfile,file)!=0)
		{
			free(currentfile);
			currentfile=strdup(file);
			openFile(file,nl,false);
			selectTabByPath(file);
		}
	else
		{
			removeUserMark("user2",nl-1);
			setUserMark("user2",nl-1);
			gotoLine(NULL,(gpointer)nl);
		}
	lastline=nl;
}

void removeLineMark(void)
{
	removeUserMark("user2",lastline-1);
}

void setLineMark(const char *file,unsigned long line,const char *markname,bool set)
{

	selectTabByPath(file);
	gotoLine(NULL,(gpointer)line);
	if(set==true)
		setUserMark(markname,line-1);
	else
		removeUserMark(markname,line-1);
}

bool parseOPString(void)
{
	char		*holdstr;
	unsigned	cnt=0;
	char		*endptr=NULL;
	char		*startptr=NULL;
	bool		found=false;
	holdstr=strdup(buffer);

	while(keywords[cnt]!=NULL)
		{
			if(strstr(buffer,keywords[cnt])!=NULL)
				{
					startptr=strstr(buffer,keywords[cnt]);
					endptr=strstr(startptr,"\",");
					if(endptr!=NULL)
						*endptr=0;
					found=true;
					switch(cnt)
						{
							case 0:
								if(fullPath!=NULL)
									free(fullPath);
								fullPath=strdup(&startptr[strlen(keywords[cnt])+1]);
								break;
							case 1:
								if(lineNumber!=NULL)
									free(lineNumber);
								lineNumber=strdup(&startptr[strlen(keywords[cnt])+1]);
								break;
							case 2:
								endptr=strstr(startptr,"\"\n");
								if(endptr!=NULL)
									*endptr=0;
								if(value!=NULL)
									free(value);
								value=strdup(&startptr[strlen(keywords[cnt])+1]);
								break;
						}
					sprintf(buffer,"%s",holdstr);
				}
			cnt++;
		}
	return(found);
}

void waitForPrompt(void)
{
	fflush(to);
	buffer[0]=0;
	usleep(150000);
	while(fgets((char*)&buffer,MAXBUFSIZE-1,from))
		{
			if(parseOPString()==true)
				setLineMarker(fullPath,atoi(lineNumber));
			buffer[0]=0;
		}
}

int fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

void setUpGDB(const char*program)
{
	/* Streams for the pipes. */
	/* PID of child gdb. */

	if(running==true)
		return;
	pid_t		pid;
	extern char	**environ;
	const char	*commands[]={"-break-insert main\n","-exec-run\n","\n",NULL};
	int			cnt=0;
	char		*ttynum;
	FILE		*fp=NULL;
	const char	*argvxt[]={"xterm","-geom","100x20+100+100","-e","/tmp/xtermscript",NULL};
	const char	*argvv[6];

	system("rm /tmp/xtermscript /tmp/ptynum &>/dev/null");
	system("echo -e \"#!/bin/bash\ntty >/tmp/ptynum\ntail -f $@ &>/dev/null\n\" > /tmp/xtermscript");
	system ("chmod +x /tmp/xtermscript");
	posix_spawnp(&xtermpid,argvxt[0],NULL,NULL,(char**)argvxt,environ);

	while(fileExists("/tmp/ptynum")!=0)
		usleep(10000);

	fp=popen("cat /tmp/ptynum","r");
	if(fp!=NULL)
		{
			while(fgets(buffer,MAXBUFSIZE-1,fp))
				{
					buffer[strlen(buffer)-1]=0;
					asprintf(&ttynum,"--tty=%s",buffer);
				}
			fclose(fp);
		}

	argvv[0]="/usr/bin/gdb";
	argvv[1]="--interpreter=mi2";
	argvv[2]="-q";
	argvv[3]=ttynum;
	argvv[4]=program;
	argvv[5]=NULL;

	pipe2(to_gdb,O_NONBLOCK);
	pipe2(from_gdb,O_NONBLOCK);
	to=fdopen(to_gdb[1],"w");
	from=fdopen(from_gdb[0],"r");

	posix_spawn_file_actions_t action;
	posix_spawn_file_actions_init(&action);
	posix_spawn_file_actions_addclose(&action,to_gdb[1]);
	posix_spawn_file_actions_addclose(&action,from_gdb[0]);

	posix_spawn_file_actions_adddup2(&action,to_gdb[0],STDIN_FILENO);
	posix_spawn_file_actions_adddup2(&action,from_gdb[1],STDOUT_FILENO);

	posix_spawn_file_actions_addclose(&action,to_gdb[0]);
	posix_spawn_file_actions_addclose(&action,from_gdb[1]);
	posix_spawnp(&pid,argvv[0],  &action, NULL, (char**)argvv, environ);

	usleep(500000);
	while(commands[cnt]!=NULL)
		{
			fputs(commands[cnt],to);
			fflush(to);
			waitForPrompt();
			cnt++;
		}
	fputs("set print pretty on\n",to);
	childPid=stopChild();
	running=true;
}

void waitForResponse(void)
{
	bool	isdone=false;

	buffer[0]=0;

	do
		{
			fflush(NULL);
			buffer[0]=0;
			fgets((char*)&buffer,MAXBUFSIZE-1,from);
			if(buffer[0]!=0)
				isdone=true;
		}
	while(isdone==false);
}

void printToToolOP(const char *txt)
{
	GtkTextIter	enditer;
	gtk_text_buffer_get_end_iter(thisPlugData->toolOutBuffer,&enditer);
	gtk_text_buffer_insert(thisPlugData->toolOutBuffer,&enditer,txt,-1);

	gtk_text_buffer_get_end_iter(thisPlugData->toolOutBuffer,&enditer);

	gtk_text_view_scroll_to_iter((GtkTextView*)thisPlugData->toolOutWindow,&enditer,0,true,0,0.5);
	while(gtk_events_pending())
		gtk_main_iteration();
}

void discardUntilGDB(void)
{
	bool	isdone=false;

	fflush(to);
	buffer[0]=0;

	do
		{
			isdone=false;
			while(fgets((char*)&buffer,MAXBUFSIZE-1,from))
				{
					if((strncmp(buffer,"(gdb)",5)==0) || (strncmp(buffer,"^done",5)==0) || (strncmp(buffer,"^error",6)==0))
						{
							isdone=true;
							break;
						}
				}
		}
	while(isdone==false);
	printToToolOP("\n");
}

void printOut(void)
{
	char	*prettystr;
	fflush(to);
	buffer[0]=0;
	usleep(10000);
	while(fgets((char*)&buffer,MAXBUFSIZE-1,from))
		{
			buffer[strlen(buffer)-2]=0;
			if(buffer[1]=='"')
				prettystr=g_strcompress(&buffer[2]);
			else
				prettystr=g_strcompress(buffer);
			//printf("%s",prettystr);
			printToToolOP(prettystr);
			g_free(prettystr);
			buffer[0]=0;
		}
	printToToolOP("\n");
}

bool getIsPtr(const char *var)
{
	char	gdbcommand[256];
	bool	retval=false;

	fflush(to);
	buffer[0]=0;
	gdbcommand[0]=0;
	sprintf(gdbcommand,"whatis %s\n",var);

	fputs(gdbcommand,to);
	waitForResponse();
	waitForResponse();	
	if(strcmp(&buffer[strlen(buffer)-5],"*\\n\"\n")==0)
		retval=true;

	discardUntilGDB();
	return(retval);
}

bool getIsStruct(const char *var)
{
	char	gdbcommand[256];
	bool	retval=false;

	fflush(to);
	buffer[0]=0;
	gdbcommand[0]=0;
	sprintf(gdbcommand,"ptype %s\n",var);
	fputs(gdbcommand,to);
	waitForResponse();
	waitForResponse();	
	if((strstr(buffer,"type = struct")!=NULL) || (strstr(buffer,"type = class")!=NULL))
		retval=true;

	discardUntilGDB();
	return(retval);
}

bool getIsString(const char *var)
{
	char	gdbcommand[256];
	bool	retval=false;

	fflush(to);
	buffer[0]=0;
	gdbcommand[0]=0;
	sprintf(gdbcommand,"ptype %s\n",var);
	fputs(gdbcommand,to);
	waitForResponse();
	waitForResponse();	
	if(strstr(buffer,"type = char *")!=NULL)
		retval=true;

	discardUntilGDB();
	return(retval);
}

void printGDBCommandToToolOP(const char *command)
{
	char	gdbcommand[256];
	char	*prettystr;
	bool	isdone;

	if(gdbcommand!=NULL)
		{
			sprintf(gdbcommand,"%s",command);
			fputs(gdbcommand,to);
		}

	fflush(to);
	buffer[0]=0;

	do
		{
			isdone=false;
			while(fgets((char*)&buffer,MAXBUFSIZE-1,from))
				{
					if((strncmp(buffer,"(gdb)",5)==0) || (strncmp(buffer,"^done",5)==0) || (strncmp(buffer,"^error",6)==0))
						{
							isdone=true;
							break;
						}
					buffer[strlen(buffer)-2]=0;
					if(buffer[1]=='"')
						prettystr=g_strcompress(&buffer[2]);
					else
						prettystr=g_strcompress(buffer);
					printToToolOP(prettystr);
					g_free(prettystr);
				}
		}
	while(isdone==false);
	printToToolOP("\n");

}

void printUntilGDB(void)
{
	char	*prettystr;
	bool	isdone;

	fflush(to);
	buffer[0]=0;

	do
		{
			isdone=false;
			while(fgets((char*)&buffer,MAXBUFSIZE-1,from))
				{
					if((strncmp(buffer,"(gdb)",5)==0) || (strncmp(buffer,"^done",5)==0) || (strncmp(buffer,"^error",6)==0))
						{
							isdone=true;
							break;
						}
					buffer[strlen(buffer)-2]=0;
					if(buffer[1]=='"')
						prettystr=g_strcompress(&buffer[2]);
					else
						prettystr=g_strcompress(buffer);
					printToToolOP(prettystr);
					g_free(prettystr);
				}
		}
	while(isdone==false);
	printToToolOP("\n");
}

void printVariable(const char *var)
{
	char	gdbcommand[256];
	varData	theVaris={false,false,false};

	fflush(to);
	buffer[0]=0;

	gdbcommand[0]=0;

	theVaris.isPtr=getIsPtr(var);
	discardUntilGDB();

	theVaris.isStruct=getIsStruct(var);
	discardUntilGDB();

	theVaris.isStr=getIsString(var);
	discardUntilGDB();


	if(theVaris.isStr==true)
		{
			sprintf(gdbcommand,"print %s\n",var);
			printGDBCommandToToolOP(gdbcommand);
			discardUntilGDB();
			return;
		}

	if((theVaris.isPtr==true) && (theVaris.isStruct==true))
		{
			sprintf(gdbcommand,"print *%s\n",var);
			printGDBCommandToToolOP(gdbcommand);
			discardUntilGDB();
			return;
		}

	if(theVaris.isStruct==true)
		{
			sprintf(gdbcommand,"ptype %s\n",var);
			printGDBCommandToToolOP(gdbcommand);
			discardUntilGDB();
			return;
		}

	sprintf(gdbcommand,"print %s\n",var);
	printGDBCommandToToolOP(gdbcommand);
	discardUntilGDB();
}

void makeBreakPoint(bool set)
{
	if(set==true)
		sprintf(buffer,"-break-insert %s",gtk_entry_get_text((GtkEntry*)getBreakPointEntry));
	else
		sprintf(buffer,"clear %s",gtk_entry_get_text((GtkEntry*)getBreakPointEntry));
}

void buttonCB(GtkWidget* widget,gpointer data)
{
	//printf("select=%lu\n",(unsigned long)data);
	switch((unsigned long)data)
		{
			case RUNNAPP:
				if(gtk_entry_get_text_length((GtkEntry*)debugAppEntry)>0)
					{
						setUpGDB(gtk_entry_get_text((GtkEntry*)debugAppEntry));
						fputs("-exec-run\n",to);
						usleep(10000);
						waitForPrompt();
						childPid=stopChild();
						return;
					}
				break;
			case EXITAPP:
				if(running==false)
					return;
				removeLineMark();
				printGDBCommandToToolOP("kill");
				kill(xtermpid,9);

				close(to_gdb[0]);
				close(to_gdb[1]);
				close(from_gdb[0]);
				close(from_gdb[1]);
				system("rm /tmp/xtermscript /tmp/ptynum");
				running=false;
				return;
				break;
			case NEXTLINE:
				removeLineMark();
				fputs("-exec-next\n",to);
				break;
			case STEPINTO:
				removeLineMark();
				fputs("-exec-step\n",to);
				break;
			case STEPOUTOF:
				removeLineMark();
				fputs("-exec-finish\n",to);
				break;
			case EXITLOOP:
				removeLineMark();
				fputs("-exec-until\n",to);
				break;
			case INTAPP:
				if(childPid!=-1)
					{
						sprintf(buffer,"kill -2 %i",childPid);
						system(buffer);
					}
				printOut();
				printGDBCommandToToolOP("backtrace\n");
				break;
			case CONTINUE:
				removeLineMark();
				fputs("-exec-continue\n",to);
				break;
			case SHOWBACK:
				printGDBCommandToToolOP("backtrace\n");
				break;
			case GETBP:
				{
					char		linenumasstr[16];
					int			linenum;
					GtkTextIter	startiter;
					pageStruct	*page=getPageStructByIDFromPage(-1);

//TODO//
					if(page!=NULL)
						{
							gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&startiter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
							linenum=gtk_text_iter_get_line(&startiter);
							sprintf(linenumasstr,"%i",linenum+1);
							gtk_entry_set_text((GtkEntry*)getBreakPointEntry,linenumasstr);
							makeBreakPoint(true);
							fputs(buffer,to);
							setUserMark("user1",atoi(gtk_entry_get_text((GtkEntry*)getBreakPointEntry))-1);
							discardUntilGDB();
							return;
						}
				}
				break;
			case SETBP:
				{
					pageStruct	*page=getPageStructByIDFromPage(-1);
					if(page!=NULL)
						{
							makeBreakPoint(true);
							fputs(buffer,to);							
							setUserMark("user1",atoi(gtk_entry_get_text((GtkEntry*)getBreakPointEntry))-1);
							discardUntilGDB();
							return;
						}
				}
				break;
			case CLEARBP:
				{
					pageStruct	*page=getPageStructByIDFromPage(-1);
					if(page!=NULL)
						{
							makeBreakPoint(false);
							fputs(buffer,to);							
							removeUserMark("user1",atoi(gtk_entry_get_text((GtkEntry*)getBreakPointEntry))-1);
							discardUntilGDB();
							return;
						}
				}
				break;
			case GETVAR:
					{
						char		*clipdata=NULL;
						GtkTextIter	startiter;
						GtkTextIter	enditer;

						pageStruct	*page=getPageStructByIDFromPage(-1);

						if(page!=NULL)
							{
								if(gtk_text_buffer_get_has_selection((GtkTextBuffer*)page->buffer)==true)
									{
										gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&startiter,&enditer);
										sinkInt=asprintf(&clipdata,"%s",gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false));

										if(clipdata!=NULL)
											{
												printVariable(clipdata);
												gtk_entry_set_text((GtkEntry*)getVarEntry,clipdata);
												return;
												break;
											}
									}
							}
					}
				break;
			case UPDATEVAR:
				{
					const char *clipdata=NULL;
					clipdata=gtk_entry_get_text((GtkEntry*)getVarEntry);
					if(clipdata!=NULL)
						{
							printVariable(clipdata);
							gtk_entry_set_text((GtkEntry*)getVarEntry,clipdata);
							return;
							break;
						}
				}
				break;
			case RUNGDBCOM:
				if(strlen(gtk_entry_get_text((GtkEntry*)gdbCommandEntry))>0)
					printGDBCommandToToolOP(gtk_entry_get_text((GtkEntry*)gdbCommandEntry));
				break;
			case OPENPATH:
				{
					GtkWidget	*dialog;
					char		*filename;

#ifdef _USEGTK3_
					dialog=gtk_file_chooser_dialog_new(gettext("Select Application To Debug"),NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN_LABEL, GTK_RESPONSE_ACCEPT,NULL);
#else
					dialog=gtk_file_chooser_dialog_new(gettext("Select Application To Debug"),NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
#endif
					gtk_window_set_transient_for((GtkWindow*)dialog,(GtkWindow*)thisPlugData->mainWindow);
					if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
						{
							filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
							if(filename!=NULL)
								{
									struct stat st;
									stat(filename,&st);
									
									if((st.st_mode & S_IXUSR))
										{
											gtk_entry_set_text((GtkEntry*)debugAppEntry,filename);
											g_free(filename);
										}
								}
							gtk_widget_destroy(dialog);
						}
					else
						gtk_widget_destroy(dialog);
				}
				return;
				break;
		}
	waitForPrompt();
}

GtkToolItem* buildToolItem(const char *filepath,const char *label)
{
	GtkWidget	*image;
	GtkToolItem	*button;

	image=gtk_image_new_from_file(filepath);
	gtk_image_set_pixel_size ((GtkImage*)image,16);
	button=gtk_tool_button_new(image,label);

	return(button);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget	*menu;
	plugData	*plugdata=(plugData*)data;

	thisPlugData=(plugData*)data;

    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_sigaction;
    sa.sa_flags   = SA_SIGINFO;

    sigaction(SIGSEGV, &sa, NULL);


	setTextDomain(true,plugdata);
	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuView);
	hideMenu=gtk_menu_item_new_with_label(gettext("Hide Debuuger"));
	g_signal_connect(G_OBJECT(hideMenu),"activate",G_CALLBACK(toggleDebugger),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),hideMenu);
	gtk_widget_show_all(plugdata->mlist.menuView);

//toolbar
	debuggerVBox=createNewBox(NEWVBOX,false,0);
	debuggerToolbar=(GtkToolbar*)gtk_toolbar_new();
	gtk_toolbar_set_icon_size(debuggerToolbar,GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_toolbar_set_style (debuggerToolbar,GTK_TOOLBAR_ICONS);

//run
	runAppButton=buildToolItem(_RESOURCES_ "debug.png","Run");
	gtk_toolbar_insert(debuggerToolbar,runAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)runAppButton,"Run application");
	g_signal_connect_after(G_OBJECT(runAppButton),"clicked",G_CALLBACK(buttonCB),(void*)RUNNAPP);

//quit
	quitAppButton=buildToolItem(_RESOURCES_ "exit.png","Exit");
	gtk_toolbar_insert(debuggerToolbar,quitAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)quitAppButton,"Stop debugging application");
	g_signal_connect_after(G_OBJECT(quitAppButton),"clicked",G_CALLBACK(buttonCB),(void*)EXITAPP);

//next
//	nextAppButton=createNewToolItem(GTK_STOCK_MEDIA_NEXT,"Next");
	nextAppButton=buildToolItem(_RESOURCES_ "next.png","Next");
	gtk_toolbar_insert(debuggerToolbar,nextAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)nextAppButton,"Next comannd");
	g_signal_connect_after(G_OBJECT(nextAppButton),"clicked",G_CALLBACK(buttonCB),(void*)NEXTLINE);

//into
//	stepIntoAppButton=createNewToolItem(GTK_STOCK_GO_DOWN,"Step In");
	stepIntoAppButton=buildToolItem(_RESOURCES_ "into.png","Into");
	gtk_toolbar_insert(debuggerToolbar,stepIntoAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)stepIntoAppButton,"Step into func");
	g_signal_connect_after(G_OBJECT(stepIntoAppButton),"clicked",G_CALLBACK(buttonCB),(void*)STEPINTO);

//out
	stepOutAppButton=buildToolItem(_RESOURCES_ "outof.png","Out of");
	gtk_toolbar_insert(debuggerToolbar,stepOutAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)stepOutAppButton,"Step out of func");
	g_signal_connect_after(G_OBJECT(stepOutAppButton),"clicked",G_CALLBACK(buttonCB),(void*)STEPOUTOF);

//end loop
	endLoopAppButton=buildToolItem(_RESOURCES_ "endloop.png","End Loop");
	gtk_toolbar_insert(debuggerToolbar,endLoopAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)endLoopAppButton,"Run to end of loop");
	g_signal_connect_after(G_OBJECT(endLoopAppButton),"clicked",G_CALLBACK(buttonCB),(void*)EXITLOOP);

//interupt
	intAppButton=buildToolItem(_RESOURCES_ "interupt.png","Int");
	gtk_toolbar_insert(debuggerToolbar,intAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)intAppButton,"Interupt app");
	g_signal_connect_after(G_OBJECT(intAppButton),"clicked",G_CALLBACK(buttonCB),(void*)INTAPP);

//continue
	contAppButton=buildToolItem(_RESOURCES_ "continue.png","Cont");
	gtk_toolbar_insert(debuggerToolbar,contAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)contAppButton,"Continue app");
	g_signal_connect_after(G_OBJECT(contAppButton),"clicked",G_CALLBACK(buttonCB),(void*)CONTINUE);

//backtrace
	backtraceAppButton=buildToolItem(_RESOURCES_ "backtrace.png","Trace");
	gtk_toolbar_insert(debuggerToolbar,backtraceAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)backtraceAppButton,"Show backtrace");
	g_signal_connect_after(G_OBJECT(backtraceAppButton),"clicked",G_CALLBACK(buttonCB),(void*)SHOWBACK);

//get breakpoint
	getBreakPointButton=buildToolItem(_RESOURCES_ "breakpoint.png","Get BP");
	gtk_toolbar_insert(debuggerToolbar,getBreakPointButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)getBreakPointButton,"Get break point at current line");
	g_signal_connect_after(G_OBJECT(getBreakPointButton),"clicked",G_CALLBACK(buttonCB),(void*)GETBP);

//set/unset
	setBreakPointButton=buildToolItem(_RESOURCES_ "set.png","Set BP");
	gtk_toolbar_insert(debuggerToolbar,setBreakPointButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)setBreakPointButton,"Set break point at current line");
	g_signal_connect_after(G_OBJECT(setBreakPointButton),"clicked",G_CALLBACK(buttonCB),(void*)SETBP);

	unsetBreakPointButton=buildToolItem(_RESOURCES_ "unset.png","Unset BP");
	gtk_toolbar_insert(debuggerToolbar,unsetBreakPointButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)unsetBreakPointButton,"Unset break point at current line");
	g_signal_connect_after(G_OBJECT(unsetBreakPointButton),"clicked",G_CALLBACK(buttonCB),(void*)CLEARBP);

//breakpoint data
	getBreakPointEntry=gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *)getBreakPointEntry,6);
	getBreakPointEntryTI=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)getBreakPointEntryTI,getBreakPointEntry);
	gtk_toolbar_insert(debuggerToolbar,getBreakPointEntryTI,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)getBreakPointEntry,"Get break point at current line");

//variables
	getVarButton=buildToolItem(_RESOURCES_ "query.png","Get Var");
	gtk_toolbar_insert(debuggerToolbar,getVarButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)getVarButton,"Get selected variable");
	g_signal_connect_after(G_OBJECT(getVarButton),"clicked",G_CALLBACK(buttonCB),(void*)GETVAR);

//update
	updateVarButton=buildToolItem(_RESOURCES_ "set.png","Update");
	gtk_toolbar_insert(debuggerToolbar,updateVarButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)updateVarButton,"Update variable data");
	g_signal_connect_after(G_OBJECT(updateVarButton),"clicked",G_CALLBACK(buttonCB),(void*)UPDATEVAR);

//variable data
	getVarEntry=gtk_entry_new();
	getVarEntryTI=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)getVarEntryTI,getVarEntry);
	gtk_toolbar_insert(debuggerToolbar,getVarEntryTI,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)getVarEntry,"Variable data");
	g_signal_connect_after(G_OBJECT(getVarEntry),"activate",G_CALLBACK(buttonCB),(void*)UPDATEVAR);

//gdb command
	gdbCommandButton=buildToolItem(_RESOURCES_ "set.png","GDB Com");
	gtk_toolbar_insert(debuggerToolbar,gdbCommandButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)gdbCommandButton,"Run GDB command");
	g_signal_connect_after(G_OBJECT(gdbCommandButton),"clicked",G_CALLBACK(buttonCB),(void*)RUNGDBCOM);

//gdb command data
	gdbCommandEntry=gtk_entry_new();
	gdbCommandEntryTI=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)gdbCommandEntryTI,gdbCommandEntry);
	gtk_toolbar_insert(debuggerToolbar,gdbCommandEntryTI,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)gdbCommandEntry,"GDB command to run");
	g_signal_connect_after(G_OBJECT(gdbCommandEntry),"activate",G_CALLBACK(buttonCB),(void*)RUNGDBCOM);
	gtk_tool_item_set_expand(gdbCommandEntryTI,true);

//debug app path
	debugApp=buildToolItem(_RESOURCES_ "open.png","Open");
	gtk_toolbar_insert(debuggerToolbar,debugApp,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)debugApp,"Path to app for debugging");
	g_signal_connect_after(G_OBJECT(debugApp),"clicked",G_CALLBACK(buttonCB),(void*)OPENPATH);

//debug app data
	debugAppEntry=gtk_entry_new();
	debugAppEntryTI=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)debugAppEntryTI,debugAppEntry);
	gtk_toolbar_insert(debuggerToolbar,debugAppEntryTI,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)debugAppEntry,"Path to app for debugging");
	gtk_tool_item_set_expand(debugAppEntryTI,true);


//vbox->debuggerToolbar->swindow->terminal
	GtkWidget *vbox=createNewBox(NEWVBOX,false,0);

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)debuggerToolbar,false,false,0);
	gtk_widget_show_all(vbox);

	gtk_box_pack_start((GtkBox*)plugdata->bottomUserBox,vbox,false,false,0);
	showHideDebugger(plugdata,false);

	GtkTextIter	endopiter;
	gtk_text_buffer_get_end_iter(plugdata->toolOutBuffer,&endopiter);
	gtk_text_buffer_insert(plugdata->toolOutBuffer,&endopiter,"Welcome to the KKEdit Debuuger Plugin\n",-1);
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	prefs;
	GtkWidget*	vbox;
	int			response;
	char*		command;
	plugData*	plugdata=(plugData*)data;

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("Debugger",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	prefs=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)prefs,prefsPath);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Prefs Path"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,prefs,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		{
			asprintf(&command,"echo %s>%s/debugger.rc",gtk_entry_get_text((GtkEntry*)prefs),plugdata->lPlugFolder);
			system(command);
			free(command);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData		*plugdata=(plugData*)data;
	char			*licencepath;
	char			*licence;
	GtkAboutDialog	*about;
	char			*translators;

	setTextDomain(true,plugdata);

	const char		*aboutboxstring=gettext("Debugger Plugin - Add's debugger pane");
	sinkInt=asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));
	sinkInt=asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Debugger Plugin"));
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,COPYRITE);
	gtk_about_dialog_set_version(about,PLUGVERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
	gtk_about_dialog_set_logo_icon_name(about,ABOUTICON);
	gtk_about_dialog_set_license(about,licence);
	gtk_about_dialog_set_translator_credits(about,(const gchar*)translators);
	gtk_window_set_transient_for((GtkWindow*)about,(GtkWindow*)plugdata->prefsWindow);

	gtk_dialog_run(GTK_DIALOG(about));
	gtk_widget_destroy((GtkWidget*)about);
	free(licence);
	free(licencepath);
	free(translators);
	setTextDomain(false,plugdata);
	return(0);
}


extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(hideMenu);
			gtk_widget_show_all(plugdata->mlist.menuBar);	
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	return(0);
}
