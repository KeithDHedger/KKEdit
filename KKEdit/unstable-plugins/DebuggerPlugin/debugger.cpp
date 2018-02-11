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

char		*prefsPath;
GtkWidget	*menuPlug;
char		*currentdomain=NULL;
int			sinkInt;

//menus
GtkWidget	*hideMenu=NULL;
bool		showing=false;

//debugger controls
GtkWidget	*toolBarBox;
GtkWidget	*debuggerVBox;
GtkToolbar	*debuggerToolbar=NULL;
GtkToolItem	*runAppButton=NULL;
GtkToolItem	*nextAppButton=NULL;
GtkToolItem	*stepIntoAppButton=NULL;
GtkToolItem	*stepOutAppButton=NULL;
GtkToolItem	*endLoopAppButton=NULL;
GtkToolItem	*finishAppButton=NULL;
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

void runCommandAndOut(char* command,plugData* plugdata)
{
	FILE*		fp=NULL;
	char		line[1024];
	GtkTextIter	iter;

	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					gtk_text_buffer_insert_at_cursor(plugdata->toolOutBuffer,line,strlen(line));
					while(gtk_events_pending())
						gtk_main_iteration();
					gtk_text_buffer_get_end_iter(plugdata->toolOutBuffer,&iter);
					gtk_text_view_scroll_to_iter((GtkTextView*)plugdata->toolOutWindow,&iter,0,true,0,0);
				}
			pclose(fp);
		}
}

void theCallBack(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;

	showToolOutput(true);
	runCommandAndOut((char*)"ls /",plugdata);
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

extern "C" int addToGui(gpointer data)
{
	GtkWidget	*menu;
	plugData	*plugdata=(plugData*)data;

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
	runAppButton=createNewToolItem(GTK_STOCK_MEDIA_PLAY,"Run");
	gtk_toolbar_insert(debuggerToolbar,runAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)runAppButton,"Run application");
//next
	nextAppButton=createNewToolItem(GTK_STOCK_MEDIA_NEXT,"Next");
	gtk_toolbar_insert(debuggerToolbar,nextAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)nextAppButton,"Next comannd");
//into
	stepIntoAppButton=createNewToolItem(GTK_STOCK_GO_DOWN,"Step In");
	gtk_toolbar_insert(debuggerToolbar,stepIntoAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)stepIntoAppButton,"Step into next func");
//out
	stepOutAppButton=createNewToolItem(GTK_STOCK_GO_UP,"Step Out");
	gtk_toolbar_insert(debuggerToolbar,stepOutAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)stepOutAppButton,"Step out of next func");
//end loop
	endLoopAppButton=createNewToolItem(GTK_STOCK_REDO,"Exit Loop");
	gtk_toolbar_insert(debuggerToolbar,endLoopAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)endLoopAppButton,"Run to end of loop");
//finish
	finishAppButton=createNewToolItem(GTK_STOCK_GOTO_LAST,"Finish");
	gtk_toolbar_insert(debuggerToolbar,finishAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)finishAppButton,"Finish app");
//interupt
	intAppButton=createNewToolItem(GTK_STOCK_DIALOG_WARNING,"Interupt");
	gtk_toolbar_insert(debuggerToolbar,intAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)intAppButton,"Interupt app");
//continue
	contAppButton=createNewToolItem(GTK_STOCK_EXECUTE,"Continue");
	gtk_toolbar_insert(debuggerToolbar,contAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)contAppButton,"Continue app");
//backtrace
	backtraceAppButton=createNewToolItem(GTK_STOCK_COPY,"Backtrace");
	gtk_toolbar_insert(debuggerToolbar,backtraceAppButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)backtraceAppButton,"Show backtrace");

//get breakpoint
	getBreakPointButton=createNewToolItem(GTK_STOCK_MEDIA_RECORD,"BP");
	gtk_toolbar_insert(debuggerToolbar,getBreakPointButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)getBreakPointButton,"Get break point at current line");
//set/unset
	setBreakPointButton=createNewToolItem(GTK_STOCK_APPLY,"Set");
	gtk_toolbar_insert(debuggerToolbar,setBreakPointButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)setBreakPointButton,"Set break point at current line");

	unsetBreakPointButton=createNewToolItem(GTK_STOCK_CANCEL,"Unset");
	gtk_toolbar_insert(debuggerToolbar,unsetBreakPointButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)unsetBreakPointButton,"Unset break point at current line");
//breakpoint data
	getBreakPointEntry=gtk_entry_new();
	getBreakPointEntryTI=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)getBreakPointEntryTI,getBreakPointEntry);
	gtk_toolbar_insert(debuggerToolbar,getBreakPointEntryTI,-1);
						//g_signal_connect_after(G_OBJECT(findApiWidget),"activate",G_CALLBACK(docSearchFromBar),(void*)findApiWidget);
	gtk_widget_set_tooltip_text((GtkWidget*)getBreakPointEntry,"Get break point at current line");

//variables
	getVarButton=createNewToolItem(GTK_STOCK_DIALOG_QUESTION,"Var");
	gtk_toolbar_insert(debuggerToolbar,getVarButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)getVarButton,"Get selected variable");
//update
	updateVarButton=createNewToolItem(GTK_STOCK_APPLY,"Update");
	gtk_toolbar_insert(debuggerToolbar,updateVarButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)updateVarButton,"Update variable data");

//variable data
	getVarEntry=gtk_entry_new();
	getVarEntryTI=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)getVarEntryTI,getVarEntry);
	gtk_toolbar_insert(debuggerToolbar,getVarEntryTI,-1);
						//g_signal_connect_after(G_OBJECT(findApiWidget),"activate",G_CALLBACK(docSearchFromBar),(void*)findApiWidget);
	gtk_widget_set_tooltip_text((GtkWidget*)getVarEntry,"Variable data");

//gdb command
	gdbCommandButton=createNewToolItem(GTK_STOCK_APPLY,"GDB");
	gtk_toolbar_insert(debuggerToolbar,gdbCommandButton,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)gdbCommandButton,"Run GDB command");
//gdb command data
	gdbCommandEntry=gtk_entry_new();
	gdbCommandEntryTI=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)gdbCommandEntryTI,gdbCommandEntry);
	gtk_toolbar_insert(debuggerToolbar,gdbCommandEntryTI,-1);
						//g_signal_connect_after(G_OBJECT(findApiWidget),"activate",G_CALLBACK(docSearchFromBar),(void*)findApiWidget);
	gtk_widget_set_tooltip_text((GtkWidget*)gdbCommandEntry,"GDB command to run");


//vbox->debuggerToolbar->swindow->terminal
	GtkWidget *vbox=createNewBox(NEWVBOX,false,0);

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)debuggerToolbar,false,false,0);
	gtk_widget_show_all(vbox);

	gtk_box_pack_start((GtkBox*)plugdata->bottomUserBox,vbox,true,true,0);
	showHideDebugger(plugdata,false);

	GtkTextIter	endopiter;
	gtk_text_buffer_get_end_iter(plugdata->toolOutBuffer,&endopiter);
	gtk_text_buffer_insert(plugdata->toolOutBuffer,&endopiter,"Welcome to the KKEdit Debuuger Plugin",-1);
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
			//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),hideMenu);

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
