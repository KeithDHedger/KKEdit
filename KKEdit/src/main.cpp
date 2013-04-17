/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "globals.h"
#include "files.h"
#include "callbacks.h"

GtkWidget*	prefswin;
GtkWidget*	fontBox;
GtkWidget*	terminalBox;
bool		tmpIndent;
bool		tmpLineNumbers;
bool		tmpLineWrap;
bool		tmpHighLight;
int			tmpTabWidth;

void buildTools(void);

void doAbout(GtkWidget* widget,gpointer data)
{
	const char*	authors[]={"K.D.Hedger <"MYEMAIL">",NULL};
	const char	copyright[] ="Copyright \xc2\xa9 2013 K.D.Hedger";
	const char*	aboutboxstring="KKEdit Code Text Editor";

	gtk_show_about_dialog(NULL,"authors",authors,"comments",aboutboxstring,"copyright",copyright,"version",VERSION,"website",MYWEBSITE,"program-name","KKEdit","logo-icon-name","KKEdit",NULL); 
}

void writeConfig(void)
{
	GtkAllocation	alloc;
	FILE*			fd=NULL;
	char*			filename;
	int				winx;
	int				winy;

	gtk_widget_get_allocation(window,&alloc);
	gtk_window_get_position((GtkWindow*)window,&winx,&winy);

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);
	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	fd=fopen(filename,"w");
	if (fd!=NULL)
		{
			fprintf(fd,"indentcode	%i\n",(int)indent);
			fprintf(fd,"showlinenumbers	%i\n",(int)lineNumbers);
			fprintf(fd,"wrapline	%i\n",(int)lineWrap);
			fprintf(fd,"highlightcurrentline	%i\n",(int)highLight);
			fprintf(fd,"insenssearch	%i\n",(int)insensitiveSearch);
			fprintf(fd,"wrapsearch	%i\n",(int)wrapSearch);

			fprintf(fd,"tabwidth	%i\n",tabWidth);
			fprintf(fd,"font	%s\n",fontAndSize);
			fprintf(fd,"terminalcommand	%s\n",terminalCommand);
			fprintf(fd,"windowsize	%i %i %i %i\n",alloc.width,alloc.height,winx,winy);
			fclose(fd);
		}
	g_free(filename);
}

void shutdown(GtkWidget* widget,gpointer data)
{
	int			numpages=gtk_notebook_get_n_pages(notebook);
	int			result;
	pageStruct*	page;

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)))
				{
					result=show_question(g_path_get_basename(page->filePath));
					switch(result)
						{
							case GTK_RESPONSE_YES:
								gtk_notebook_set_current_page(notebook,loop);
								saveFile(NULL,NULL);
								break;
							case GTK_RESPONSE_NO:
								break;
							default:
								return;
								break;
						}
				}
		}
	writeConfig();
	gtk_main_quit();
}

void readConfig(void)
{
	FILE*	fd=NULL;
	char*	filename;
	char	buffer[1024];
	char	name[256];
	int		intarg;
	char	strarg[256];

	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	fd=fopen(filename,"r");
	if (fd!=NULL)
		{
			while(feof(fd)==0)
				{
					fgets(buffer,1024,fd);
					sscanf(buffer,"%s %s",(char*)&name,(char*)&strarg);

					if(strcasecmp(name,"indentcode")==0)
							indent=(bool)atoi(strarg);
					if(strcasecmp(name,"showlinenumbers")==0)
							lineNumbers=(bool)atoi(strarg);
					if(strcasecmp(name,"wrapline")==0)
							lineWrap=(bool)atoi(strarg);
					if(strcasecmp(name,"highlightcurrentline")==0)
							highLight=(bool)atoi(strarg);

					if(strcasecmp(name,"insenssearch")==0)
							insensitiveSearch=(bool)atoi(strarg);
					if(strcasecmp(name,"wrapsearch")==0)
							wrapSearch=(bool)atoi(strarg);

					if(strcasecmp(name,"tabwidth")==0)
							tabWidth=atoi(strarg);
					if(strcasecmp(name,"font")==0)
						{
							sscanf(buffer,"%*s %s %i",(char*)&strarg,(int*)&intarg);
							asprintf(&fontAndSize,"%s %i",strarg,intarg);
						}
					if(strcasecmp(name,"terminalcommand")==0)
						{
							g_free(terminalCommand);
							sscanf(buffer,"%*s %[]a-zA-Z0-9 ()_-,.*#;[\"]s",(char*)&strarg);
							asprintf(&terminalCommand,"%s",strarg);
						}

					if(strcasecmp(name,"windowsize")==0)
						sscanf(buffer,"%*s %i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);
				}
			fclose(fd);
		}
}

void init(void)
{
	char*	filename;

	indent=true;
	lineNumbers=true;
	lineWrap=true;
	highLight=true;
	tabWidth=4;
	asprintf(&fontAndSize,"%s","mono 10");
	asprintf(&terminalCommand,"%s","xterm");
	windowWidth=800;
	windowHeight=400;
	windowX=-1;
	windowY=-1;
	wrapSearch=true;
	insensitiveSearch=true;
	replaceAll=false;

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);

	readConfig();

	tmpIndent=indent;
	tmpLineNumbers=lineNumbers;
	tmpLineWrap=lineWrap;
	tmpHighLight=highLight;
	tmpTabWidth=tabWidth;
}

void buildFindReplace(void)
{
	GtkWidget*	content_area;
	GtkWidget*	replace;
	GtkWidget*	image;
	GtkWidget*	label;
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	item;

	findReplaceDialog=gtk_dialog_new_with_buttons("Find/Replace",(GtkWindow*)window, GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_GO_FORWARD,FINDNEXT,GTK_STOCK_GO_BACK,FINDPREV,"Replace",REPLACE,NULL);
	gtk_dialog_set_default_response((GtkDialog*)findReplaceDialog,GTK_RESPONSE_OK);
	g_signal_connect(G_OBJECT(findReplaceDialog),"response",G_CALLBACK(doFindReplace),NULL);
	content_area=gtk_dialog_get_content_area(GTK_DIALOG(findReplaceDialog));

	vbox=gtk_vbox_new(true,0);
	hbox=gtk_hbox_new(false,0);


	label=gtk_label_new("Find");
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	findBox=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)findBox,"");
	gtk_entry_set_activates_default((GtkEntry*)findBox,true);
	gtk_container_add(GTK_CONTAINER(content_area),findBox);

	label=gtk_label_new("Replace With");
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	replaceBox=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)replaceBox,"");
	gtk_entry_set_activates_default((GtkEntry*)replaceBox,true);
	gtk_container_add(GTK_CONTAINER(content_area),replaceBox);

	item=gtk_check_button_new_with_label("Case insensitive");
	gtk_toggle_button_set_active((GtkToggleButton*)item,insensitiveSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)1);

	item=gtk_check_button_new_with_label("Wrap");
	gtk_toggle_button_set_active((GtkToggleButton*)item,wrapSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)2);

	item=gtk_check_button_new_with_label("Replace All");
	gtk_toggle_button_set_active((GtkToggleButton*)item,replaceAll);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
//	gtk_widget_show(item);
	gtk_widget_hide(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)3);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(content_area),vbox,true,true,0);

	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,100);
	image=gtk_image_new_from_stock(GTK_STOCK_FIND_AND_REPLACE,GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image((GtkButton*)replace,image);

	gtk_widget_show(findBox);
	gtk_widget_show(replaceBox);
	gtk_widget_show(vbox);
	gtk_widget_show(hbox);

	gtk_signal_connect_object(GTK_OBJECT(findReplaceDialog),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(findReplaceDialog));
	gtk_signal_connect (GTK_OBJECT(findReplaceDialog),"delete_event",GTK_SIGNAL_FUNC(gtk_true),NULL);
}

GtkWidget*	toolNameWidget;;
GtkWidget*	commandLineWidget;;
GtkWidget*	inTermWidget;
GtkWidget*	syncWidget;
GtkWidget*	ignoreWidget;
GtkWidget*	pasteWidget;
GtkWidget*	replaceWidget;

bool		inTerm=false;
bool		runSync=true;
bool		ignoreOut=true;
bool		pasteOut=false;
bool		replaceOut=false;

void setToolOptions(GtkWidget* widget,gpointer data)
{
	int		flags;
	char*	dirname;
	FILE*	fd=NULL;
	char	toolpath[2048];
	int		toolnum=1;

	if(strcmp(gtk_widget_get_name(widget),"interm")==0)
		inTerm=gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget);
	if(strcmp(gtk_widget_get_name(widget),"sync")==0)
		runSync=gtk_toggle_button_get_active((GtkToggleButton*)syncWidget);
	if(strcmp(gtk_widget_get_name(widget),"ignore")==0)
		ignoreOut=gtk_toggle_button_get_active((GtkToggleButton*)ignoreWidget);
	if(strcmp(gtk_widget_get_name(widget),"paste")==0)
		pasteOut=gtk_toggle_button_get_active((GtkToggleButton*)pasteWidget);
	if(strcmp(gtk_widget_get_name(widget),"replace")==0)
		replaceOut=gtk_toggle_button_get_active((GtkToggleButton*)replaceWidget);

	if(runSync==false)
		{
			flags=TOOL_ASYNC;
			gtk_widget_set_sensitive(ignoreWidget,false);
			gtk_widget_set_sensitive(pasteWidget,false);
			gtk_widget_set_sensitive(replaceWidget,false);
		}
	else
		{
			gtk_widget_set_sensitive(ignoreWidget,true);
			gtk_widget_set_sensitive(pasteWidget,true);
			gtk_widget_set_sensitive(replaceWidget,true);

			if(ignoreOut==true)
				flags=TOOL_IGNORE_OP;
			if(pasteOut==true)
				flags=TOOL_PASTE_OP;
			if(replaceOut==true)
				flags=TOOL_REPLACE_OP;				
		}

	if(inTerm==true)
		{
			gtk_widget_set_sensitive(ignoreWidget,false);
			gtk_widget_set_sensitive(pasteWidget,false);
			gtk_widget_set_sensitive(replaceWidget,false);
		}

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			asprintf(&dirname,"%s/.KKEdit/tools",getenv("HOME"));

			while(true)
				{
					sprintf((char*)&toolpath,"%s/.KKEdit/tools/tool-%s-%i",getenv("HOME"),gtk_entry_get_text((GtkEntry*)toolNameWidget),toolnum);
					if(!g_file_test(toolpath,G_FILE_TEST_EXISTS))
						break;
					toolnum++;
				}

			fd=fopen(toolpath,"w");
			if (fd!=NULL)
				{
					fprintf(fd,"name\t%s\n",gtk_entry_get_text((GtkEntry*)toolNameWidget));
					fprintf(fd,"command\t%s\n",gtk_entry_get_text((GtkEntry*)commandLineWidget));
					fprintf(fd,"flags\t%i\n",flags);
					fprintf(fd,"interm\t%i\n",(int)inTerm);
					fclose(fd);
				}
			g_free(dirname);
			gtk_widget_destroy((GtkWidget*)data);

			buildTools();
			gtk_widget_show_all(menutools);
		}

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		gtk_widget_destroy((GtkWidget*)data);
}

void doMakeTool(void)
{
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;
	GtkWidget*	toolwin;

	toolwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)toolwin,"Create New Tool");
	vbox=gtk_vbox_new(true,8);

//name
	toolNameWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Tool Name: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),toolNameWidget);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(toolNameWidget);
	gtk_entry_set_text((GtkEntry*)toolNameWidget,"New Tool");

//command
	commandLineWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Command: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),commandLineWidget);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(commandLineWidget);
	gtk_entry_set_text((GtkEntry*)commandLineWidget,"");

//in terminal
	inTermWidget=gtk_check_button_new_with_label("Run Tool In Terminal");
	gtk_widget_set_name(inTermWidget,"interm");
	gtk_toggle_button_set_active((GtkToggleButton*)inTermWidget,inTerm);
	gtk_box_pack_start(GTK_BOX(vbox),inTermWidget,false,true,0);
	g_signal_connect(G_OBJECT(inTermWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//flags
//snch/async
	syncWidget=gtk_check_button_new_with_label("Run Tool Synchronously");
	gtk_widget_set_name(syncWidget,"sync");
	gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,runSync);
	gtk_box_pack_start(GTK_BOX(vbox),syncWidget,false,true,0);
	g_signal_connect(G_OBJECT(syncWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - ignore
	ignoreWidget=gtk_radio_button_new_with_label(NULL,"Ignore Output");
	gtk_widget_set_name(ignoreWidget,"ignore");
	gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,ignoreOut);
	gtk_box_pack_start(GTK_BOX(vbox),ignoreWidget,false,true,0);
	g_signal_connect(G_OBJECT(ignoreWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - paste
	pasteWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Paste Output");
    gtk_widget_set_name(pasteWidget,"paste");
	gtk_toggle_button_set_active((GtkToggleButton*)pasteWidget,pasteOut);
	gtk_box_pack_start(GTK_BOX(vbox),pasteWidget,false,true,0);
	g_signal_connect(G_OBJECT(pasteWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - replace all
	replaceWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Replace All Contents");
	gtk_widget_set_name(replaceWidget,"replace");
	gtk_toggle_button_set_active((GtkToggleButton*)replaceWidget,replaceOut);
	gtk_box_pack_start(GTK_BOX(vbox),replaceWidget,false,true,0);
	g_signal_connect(G_OBJECT(replaceWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//buttons
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);

	hbox=gtk_hbox_new(false,0);
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"apply");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"cancel");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,false,2);

//show it
	gtk_container_add(GTK_CONTAINER(toolwin),(GtkWidget*)vbox);
	gtk_widget_show_all(toolwin);
}

void buildTools(void)
{
	GDir*			folder;
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	const gchar*	entry=NULL;
	FILE*			fd=NULL;
	char*			filepath;
	char			buffer[4096];
	toolStruct*		tool;
	char*			datafolder[2];
	char			strarg[1024];
	
	int				intermarg=0;
	int				flagsarg=0;
	char*			commandarg=NULL;
	char*			menuname=NULL;

	asprintf(&datafolder[0],"%s/tools/",DATADIR);
	asprintf(&datafolder[1],"%s/.KKEdit/tools/",getenv("HOME"));

	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)menutools);
	if (submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)menutools,NULL);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menutools),menu);

//addtool
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doMakeTool),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	for(int loop=0;loop<2;loop++)
		{
			folder=g_dir_open(datafolder[loop],0,NULL);
			if(folder!=NULL)
				{
					entry=g_dir_read_name(folder);
					while(entry!=NULL)
						{
							asprintf(&filepath,"%s%s",datafolder[loop],entry);
							fd=fopen(filepath,"r");
							if (fd!=NULL)
								{
									intermarg=0;
									flagsarg=0;

									while(fgets(buffer,4096,fd))
										{
											buffer[strlen(buffer)-1]=0;
											sscanf((char*)&buffer,"%s",(char*)&strarg);
											if(strcmp(strarg,"name")==0)
												asprintf(&menuname,"%.*s",(int)strlen(buffer)-5,(char*)&buffer[5]);
											if(strcmp(strarg,"command")==0)
												asprintf(&commandarg,"%.*s",(int)strlen(buffer)-8,(char*)&buffer[8]);
											if(strcmp(strarg,"interm")==0)
												sscanf((char*)&buffer,"%*s %i",&intermarg);
											if(strcmp(strarg,"flags")==0)
												sscanf((char*)&buffer,"%*s %i",&flagsarg);
										}

									if((menuname!=NULL) && (strlen(menuname)>0))
										{
											tool=(toolStruct*)malloc(sizeof(toolStruct));
											asprintf(&tool->menuName,"%s",menuname);
											asprintf(&tool->command,"%s",commandarg);
											tool->flags=flagsarg;
											tool->inTerminal=(bool)intermarg;
											menuitem=gtk_image_menu_item_new_with_label(tool->menuName);
											gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
											gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)tool);
											g_free(menuname);
											g_free(commandarg);
											menuname=NULL;
										}
									fclose(fd);
								}
							entry=g_dir_read_name(folder);
						}
				}
		}
}

void setPrefs(GtkWidget* widget,gpointer data)
{

	if(strcmp(gtk_widget_get_name(widget),"indent")==0)
		tmpIndent=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"show")==0)
		tmpLineNumbers=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"wrap")==0)
		tmpLineWrap=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"high")==0)
		tmpHighLight=gtk_toggle_button_get_active((GtkToggleButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"tabs")==0)
		tmpTabWidth=(int)gtk_spin_button_get_value((GtkSpinButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		gtk_widget_destroy(prefswin);

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			indent=tmpIndent;
			lineNumbers=tmpLineNumbers;
			lineWrap=tmpLineWrap;
			highLight=tmpHighLight;
			if(terminalCommand!=NULL)
				{
					g_free(terminalCommand);
					asprintf(&terminalCommand,"%s",gtk_entry_get_text((GtkEntry*)terminalBox));
				}

			if(fontAndSize!=NULL)
				{
					g_free(fontAndSize);
					asprintf(&fontAndSize,"%s",gtk_entry_get_text((GtkEntry*)fontBox));
				}

			tabWidth=tmpTabWidth;
			gtk_widget_destroy(prefswin);
			resetAllFilePrefs();
		}
}

void doPrefs(void)
{
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	item;

	prefswin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)prefswin,"Preferences");
	vbox=gtk_vbox_new(true,8);
//indent
	item=gtk_check_button_new_with_label("Auto Indent Lines");
	gtk_widget_set_name(item,"indent");
	gtk_toggle_button_set_active((GtkToggleButton*)item,indent);
	gtk_box_pack_start(GTK_BOX(vbox),item,false,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);
//linenumbers
	item=gtk_check_button_new_with_label("Show Line Numbers");
	gtk_widget_set_name(item,"show");
	gtk_toggle_button_set_active((GtkToggleButton*)item,lineNumbers);
	gtk_box_pack_start(GTK_BOX(vbox),item,false,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);
//wraplines
	item=gtk_check_button_new_with_label("Wrap Lines");
	gtk_widget_set_name(item,"wrap");
	gtk_toggle_button_set_active((GtkToggleButton*)item,lineWrap);
	gtk_box_pack_start(GTK_BOX(vbox),item,false,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//highlite
	item=gtk_check_button_new_with_label("Highlight Current Line");
	gtk_widget_set_name(item,"high");
	gtk_toggle_button_set_active((GtkToggleButton*)item,highLight);
	gtk_box_pack_start(GTK_BOX(vbox),item,false,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//tabwidth  -- CLEAN
	GtkObject*	adj=gtk_adjustment_new(tmpTabWidth,1,64,1,1,0);
	hbox=gtk_hbox_new(true,0);
	item=gtk_spin_button_new((GtkAdjustment*)adj,1,0);
	gtk_widget_set_name(item,"tabs");
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Tab width: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),item);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	g_signal_connect(G_OBJECT(item),"value-changed",G_CALLBACK(setPrefs),(void*)item);
//font
	fontBox=gtk_entry_new();
	hbox=gtk_hbox_new(true,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Font And Size: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),fontBox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)fontBox,fontAndSize);

//terminalcommand
	terminalBox=gtk_entry_new();
	hbox=gtk_hbox_new(true,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Terminal Command: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),terminalBox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)terminalBox,terminalCommand);

//buttons
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);

	hbox=gtk_hbox_new(false,0);
	item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"apply");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);	

	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"cancel");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,false,2);

//show it
	gtk_container_add(GTK_CONTAINER(prefswin),(GtkWidget*)vbox);
	gtk_widget_show_all(prefswin);
}

int main(int argc,char **argv)
{
	GtkWidget*		vbox;
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GtkWidget*		toolbar;
	GtkToolItem*	toolbutton;
	GtkAccelGroup*	accgroup;
	GtkWidget*		image;
	GtkWidget*		entrybox;

	gtk_init(&argc,&argv);
	init();

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)window,windowWidth,windowHeight);
	if(windowX!=-1 && windowY!=-1)
		gtk_window_move((GtkWindow *)window,windowX,windowY);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);
	accgroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)window,accgroup);

	notebook=(GtkNotebook*)gtk_notebook_new();
	gtk_notebook_set_scrollable(notebook,true);
	gtk_signal_connect(GTK_OBJECT(notebook),"switch-page",G_CALLBACK(switchPage),NULL);

	vbox=gtk_vbox_new(FALSE,0);
	menubar=gtk_menu_bar_new();
	toolbar=gtk_toolbar_new();

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)menubar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)toolbar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)notebook,true,true,0);

//dnd
	gtk_drag_dest_set(vbox,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(vbox);
	g_signal_connect (G_OBJECT(vbox),"drag_data_received",G_CALLBACK(dropUri),NULL);

//toolbar
//new
	newButton=gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	gtk_toolbar_insert((GtkToolbar*)toolbar,newButton,-1);
	gtk_signal_connect(GTK_OBJECT(newButton),"clicked",G_CALLBACK(newFile),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)newButton,"New File");
//open
	openButton=gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert((GtkToolbar*)toolbar,openButton,-1);
	gtk_signal_connect(GTK_OBJECT(openButton),"clicked",G_CALLBACK(doOpenFile),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)openButton,"Open File");
//save
	saveButton=gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_toolbar_insert((GtkToolbar*)toolbar,saveButton,-1);
	gtk_signal_connect(GTK_OBJECT(saveButton),"clicked",G_CALLBACK(saveFile),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)saveButton,"Save File");
//close
	closeButton=gtk_tool_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_toolbar_insert((GtkToolbar*)toolbar,closeButton,-1);
	gtk_signal_connect(GTK_OBJECT(closeButton),"clicked",G_CALLBACK(closeTab),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)closeButton,"Close Tab");

	gtk_toolbar_insert((GtkToolbar*)toolbar,gtk_separator_tool_item_new(),-1);

//edit buttons
//cut
	toolbutton=gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
	gtk_toolbar_insert((GtkToolbar*)toolbar,toolbutton,-1);
	gtk_signal_connect(GTK_OBJECT(toolbutton),"clicked",G_CALLBACK(cutToClip),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)toolbutton,"Cut");
//copy
	toolbutton=gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
	gtk_toolbar_insert((GtkToolbar*)toolbar,toolbutton,-1);
	gtk_signal_connect(GTK_OBJECT(toolbutton),"clicked",G_CALLBACK(copyToClip),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)toolbutton,"Copy");
//paste
	toolbutton=gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);
	gtk_toolbar_insert((GtkToolbar*)toolbar,toolbutton,-1);
	gtk_signal_connect(GTK_OBJECT(toolbutton),"clicked",G_CALLBACK(pasteFromClip),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)toolbutton,"Paste");

	gtk_toolbar_insert((GtkToolbar*)toolbar,gtk_separator_tool_item_new(),-1);

//undo
	undoButton=gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
	gtk_toolbar_insert((GtkToolbar*)toolbar,undoButton,-1);
	gtk_signal_connect(GTK_OBJECT(undoButton),"clicked",G_CALLBACK(undo),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)undoButton,"Undo");
//redo
	redoButton=gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
	gtk_toolbar_insert((GtkToolbar*)toolbar,redoButton,-1);
	gtk_signal_connect(GTK_OBJECT(redoButton),"clicked",G_CALLBACK(redo),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)redoButton,"Redo");

	gtk_toolbar_insert((GtkToolbar*)toolbar,gtk_separator_tool_item_new(),-1);

//find
	toolbutton=gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
	gtk_toolbar_insert((GtkToolbar*)toolbar,toolbutton,-1);
	gtk_signal_connect(GTK_OBJECT(toolbutton),"clicked",G_CALLBACK(find),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)toolbutton,"Find/Replace");

//navigation
	toolbutton=gtk_tool_button_new_from_stock(GTK_STOCK_DIALOG_QUESTION);
	gtk_toolbar_insert((GtkToolbar*)toolbar,toolbutton,-1);
	gtk_signal_connect(GTK_OBJECT(toolbutton),"clicked",G_CALLBACK(goToDefinition),NULL);
	gtk_widget_set_tooltip_text((GtkWidget*)toolbutton,"Go To Definition");

	gtk_toolbar_insert((GtkToolbar*)toolbar,gtk_separator_tool_item_new(),-1);

//goto line
	entrybox=gtk_entry_new();
	toolbutton=gtk_tool_item_new();
	gtk_container_add((GtkContainer *)toolbutton,entrybox);
	gtk_toolbar_insert((GtkToolbar*)toolbar,toolbutton,-1);
	g_signal_connect_after(G_OBJECT(entrybox),"key-release-event",G_CALLBACK(jumpToLineFromBar),NULL);
	gtk_widget_set_size_request((GtkWidget*)toolbutton,48,-1);
	gtk_widget_set_tooltip_text((GtkWidget*)toolbutton,"Go To Line");

//menus
//file menu
	menufile=gtk_menu_item_new_with_label("File");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufile),menu);
//new
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'N',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//open
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doOpenFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'O',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//save
	saveMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),saveMenu);
	gtk_signal_connect(GTK_OBJECT(saveMenu),"activate",G_CALLBACK(saveFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)saveMenu,"activate",accgroup,'S',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//savas
	saveAsMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),saveAsMenu);
	gtk_signal_connect(GTK_OBJECT(saveAsMenu),"activate",G_CALLBACK(saveFile),(void*)1);
	gtk_widget_add_accelerator((GtkWidget *)saveAsMenu,"activate",accgroup,'S',(GdkModifierType)(GDK_SHIFT_MASK|GDK_CONTROL_MASK),GTK_ACCEL_VISIBLE);
	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save session
	image=gtk_image_new_from_stock(GTK_STOCK_SAVE,GTK_ICON_SIZE_MENU);
	menuitem=gtk_image_menu_item_new_with_label("Save Session");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(saveSession),NULL);
//	gtk_widget_add_accelerator((GtkWidget *)saveMenu,"activate",accgroup,'S',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//restore session
	image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	menuitem=gtk_image_menu_item_new_with_label("Restore Session");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(restoreSession),NULL);
//	gtk_widget_add_accelerator((GtkWidget *)saveMenu,"activate",accgroup,'S',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//	menuitem=gtk_separator_menu_item_new();
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//printfile
//	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PRINT,NULL);
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(printFile),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//close
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(closeTab),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'W',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//reload file
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_REVERT_TO_SAVED,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(reloadFile),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(shutdown),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'Q',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//edit menu
	menuedit=gtk_menu_item_new_with_label("Edit");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuedit),menu);
//undo
	undoMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_UNDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),undoMenu);
	gtk_signal_connect(GTK_OBJECT(undoMenu),"activate",G_CALLBACK(undo),NULL);
	gtk_widget_add_accelerator((GtkWidget *)undoMenu,"activate",accgroup,'Z',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//redo
	redoMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_REDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),redoMenu);
	gtk_signal_connect(GTK_OBJECT(redoMenu),"activate",G_CALLBACK(redo),NULL);
	gtk_widget_add_accelerator((GtkWidget *)redoMenu,"activate",accgroup,'Z',(GdkModifierType)(GDK_SHIFT_MASK|GDK_CONTROL_MASK),GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//cut
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(cutToClip),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'X',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//copy
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(copyToClip),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'C',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//paste
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(pasteFromClip),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'V',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//find
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(find),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'F',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//prefs
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doPrefs),NULL);


//navigaion menu
	menunav=gtk_menu_item_new_with_label("Navigation");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menunav),menu);
//goto define
	image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_MENU);
	menuitem=gtk_image_menu_item_new_with_label("Go To Definition");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(goToDefinition),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'D',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//open include
	menuitem=gtk_image_menu_item_new_with_label("Open Include File");
	image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(findFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'I',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//goto line number
	menuitem=gtk_image_menu_item_new_with_label("Go To Line");
	image=gtk_image_new_from_stock(GTK_STOCK_GO_DOWN,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(jumpToLine),NULL);

//function menu
	menufunc=gtk_menu_item_new_with_label("Functions");

//bookmarks
	menubookmark=gtk_menu_item_new_with_label("Bookmarks");
	menubookmarksub=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubookmark),menubookmarksub);
	menuitem=gtk_menu_item_new_with_label("Add Bookmark");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubookmarksub),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(addBookmark),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menubookmarksub),menuitem);

//external tools
	menutools=gtk_menu_item_new_with_label("Tools");
	buildTools();

//help
	menuhelp=gtk_menu_item_new_with_label("Help");
	menu=gtk_menu_new();
//about
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuhelp),menu);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doAbout),NULL);
//help
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,NULL);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(openHelp),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menufile);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menuedit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menunav);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menufunc);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menubookmark);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menutools);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menuhelp);

	gtk_container_add(GTK_CONTAINER(window),(GtkWidget*)vbox);
	gtk_widget_set_sensitive((GtkWidget*)saveButton,false);
	gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);

	for (int j=1;j<argc;j++)
		openFile(argv[j],0);

	gtk_widget_show_all(window);
	buildFindReplace();
	gtk_main();
}
