/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:49:21 GMT 2015 kdhedger68713@gmail.com

 * This file(main.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <X11/Xatom.h>
#include <gdk/gdkx.h>

#include "kkedit-includes.h"

bool	singleOverRide=false;
bool	loadPluginsFlag=true;

char* oneLiner(const char *command)
{
	FILE	*fp=NULL;
	char	*retstr=(char*)calloc(1,256);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			sinkReturnStr=fgets(retstr,256,fp);
			pclose(fp);
		}
	if(sinkReturnStr==NULL)
		debugFree(&retstr);
	else
		{
			if(retstr[strlen(retstr)-1]=='\n')
				retstr[strlen(retstr)-1]=0;
		}
	return(retstr);
}

void readConfig(void)
{
	ERRDATA
	getOldConfigs("kkedit.rc",kkedit_rc);

	getOldConfigs("kkedit.window.rc",kkedit_window_rc);
	if(windowAllocData!=NULL)
		sscanf(windowAllocData,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);

	if(docWindowAllocData!=NULL)
		sscanf(docWindowAllocData,"%i %i %i %i",(int*)&docWindowWidth,(int*)&docWindowHeight,(int*)&docWindowX,(int*)&docWindowY);
	ERRDATA
}

void init(void)
{
	ERRDATA
	char		*filename=NULL;
	int			exitstatus;
	char		tmpfoldertemplate[]="/tmp/KKEdit-XXXXXX";

	filename=mkdtemp(tmpfoldertemplate);
	if(filename!=NULL)
		tmpFolderName=strdup(filename);
	else
		{
			infoMessage("Can't create tempory files!\nClear some space on /tmp.\nQuiting ...");
			exit(0);
		}
	filename=NULL;
	sinkReturn=asprintf(&logFile,"%s.log",tmpFolderName);

//nag times
	lastNagTime=-1;
	nagTime=(long)time(NULL);

#ifdef _ASPELL_
	AspellCanHaveError	*possible_err;
#endif
	globalSlice=new StringSlice;

	indent=true;
	lineNumbers=true;
	lineWrap=true;
	highLight=true;
	noSyntax=false;
	showWhiteSpace=false;
	tabWidth=4;
#ifdef _USEGTK3_
	tabsSize=2;
#else
	tabsSize=0;
#endif
	depth=1;
	singleUse=true;
	fontAndSize=strdup("mono 10");
	terminalCommand=strdup("xterm -e");
	rootCommand=strdup(GTKSUPATH);
	autoShowMinChars=4;
	autoShowComps=false;
	autoCheck=true;
	useGlobalPlugMenu=true;
	maxTabChars=20;
	maxJumpHistory=10;

	windowWidth=800;
	windowHeight=600;
	windowX=-1;
	windowY=-1;
	docWindowWidth=800;
	docWindowHeight=600;
	docWindowX=-1;
	docWindowY=-1;

	wrapSearch=true;
	insensitiveSearch=true;
	useRegex=false;
	replaceAll=false;
	onExitSaveSession=false;
	onExitSaveSession=false;
	restoreBookmarks=false;
	noDuplicates=false;
	noWarnings=false;
	readLinkFirst=false;
	hightlightAll=true;
	maxFuncDefs=64;
	maxBMChars=64;

	openInThisTab=-1;
	loadingSession=false;
	doGoogleSearch=true;

	recentMan=gtk_recent_manager_get_default();
//runtime deps
	
	exitstatus=system("which manpageeditor 2>&1 >/dev/null");
	gotManEditor=(bool)exitstatus;
	exitstatus=system("which doxygen 2>&1 >/dev/null");
	gotDoxygen=(bool)exitstatus;
#ifdef _INBSD_
	ctagsPath=oneLiner("which exctags");
#else
	ctagsPath=oneLiner("which ctags");
#endif

	if(getuid()!=0)
		styleName=strdup("classic");
	else
		styleName=strdup("Root Source");
	nagScreen=false;
	highlightColour=strdup("#A3ACFF");
	showBMBar=false;
	listFunction=4;
	showStatus=true;
 
	sinkReturn=asprintf(&filename,"%s/%s/tools",getenv("HOME"),APPFOLDENAME);
	g_mkdir_with_parents(filename,493);
	ERRDATA debugFree(&filename);
	schemeManager=gtk_source_style_scheme_manager_get_default();
	sinkReturn=asprintf(&filename,"%s/.gnome2/gedit/styles",getenv("HOME"));
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	ERRDATA debugFree(&filename);
	sinkReturn=asprintf(&filename,"%s",GSTYLESFOLDER);
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	ERRDATA debugFree(&filename);

//toolbar layout
	toolBarLayout=strdup("NOSsXCPsURBWsFGsE9AQDL");

	readConfig();
	loadKeybindings();

	styleScheme=gtk_source_style_scheme_manager_get_scheme(schemeManager,styleName);

	tmpStyleName=strdup(styleName);
	tmpHighlightColour=highlightColour;

	sinkReturn=asprintf(&htmlFile,"%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));
	sinkReturn=asprintf(&htmlURI,"file://%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));

#ifdef _ASPELL_
	spellChecker=NULL;
	aspellConfig=NULL;
	aspellConfig=new_aspell_config();
	possible_err=new_aspell_speller(aspellConfig);

	if(aspell_error_number(possible_err)!= 0)
		puts(aspell_error_message(possible_err));
	else
		spellChecker=to_aspell_speller(possible_err);
#endif

//do plugins
	globalPlugins=new PluginClass(loadPluginsFlag);
//set up plugin data
	globalPlugins->globalPlugData=(plugData*)malloc(sizeof(plugData));
	globalPlugins->globalPlugData->dataDir=GAPPFOLDER;
	globalPlugins->globalPlugData->gPlugFolder=globalPlugins->plugFolderPaths[GLOBALPLUGS];
	globalPlugins->globalPlugData->lPlugFolder=globalPlugins->plugFolderPaths[LOCALPLUGS];
	globalPlugins->globalPlugData->htmlFile=htmlFile;
	globalPlugins->globalPlugData->thePage=&thePage;
	globalPlugins->globalPlugData->currentTab=-1;
	globalPlugins->globalPlugData->tmpFolder=tmpFolderName;
	globalPlugins->globalPlugData->kkeditVersion=VERSION;

/*	
		for(int j=0;j<globalPlugins->plugCount;j++)
			{
				struct pluginData
	{
		char		*name;
		bool		enabled;
		GModule	*module;
		bool		loaded;
		char       *path;
	};
				pluginData *pd=(pluginData*)g_list_nth_data(globalPlugins->plugins,j);
				printf("num %i name=%s\n",j,pd->name);
				printf("num %i enabled=%i\n",j,(int)pd->enabled);
				printf("num %i path=%s\n",j,pd->path);
			}
*/

//time to nag?

	timeToNag=false;
	if((lastNagTime==0) ||(nagTime-lastNagTime>updateWait))
		{
			timeToNag=true;
			lastNagTime=nagTime;
		}

	localeLang=getenv("LANG");

	globalSlice->setReturnDupString(true);

	funcProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	funcProv->priority=1;
	funcProv->name=COMPS_FUNCS_LABEL;
	funcProv->proposals=NULL;

	varsProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	varsProv->priority=2;
	varsProv->name=COMPS_VARS_LABEL;
	varsProv->proposals=NULL;

	customProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	customProv->priority=11;
	customProv->name=COMPS_WORDS_LABEL;
	customProv->proposals=NULL;

	docWordsProv=gtk_source_completion_words_new(NULL,NULL);
	g_object_set(docWordsProv,"priority",10,NULL);
	g_object_set(docWordsProv,"minimum-word-size",autoShowMinChars,NULL);
	g_object_set(docWordsProv,"interactive-delay",50,NULL);
	ERRDATA
}

void doNagScreen(void)
{
	ERRDATA
	GtkWidget *dialog;

	dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",DIALOG_PLEASE_DONATE_LABEL);
	gtk_message_dialog_format_secondary_markup((GtkMessageDialog*)dialog,"%s\n<b>%s</b>\n%s\n\n%s",DIALOG_PAYPAL_LABEL,MYEMAIL,DIALOG_SEND_IT_LABEL,DIALOG_THANKS_LABEL);

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	ERRDATA
}

void doNagStuff(void)
{
	ERRDATA
	char			*command=NULL;
	char			*control=NULL;
	int				gotcurl=-1;
	int				gotwget=-1;
	int				exitstatus;
	FILE			*fp;
	char			t1[1024];
	char			vers[1024];
	char			plugt[1024];
	unsigned int	thisupdate=0;
	char			*kkeditupdatemessage=strdup("");
	char			*plugupdatemessage=strdup("");

	sinkReturn=asprintf(&command,"%s \"%s\" %s/updatecontrol &",POLEPATH,CHECKING_LABEL,tmpFolderName);
	sinkReturn=system(command);
	while(gtk_events_pending())
		gtk_main_iteration();

	ERRDATA debugFree(&command);

	sinkReturn=asprintf(&control,"echo \"pulse\" > \"%s/updatecontrol\"",tmpFolderName);
	sinkReturn=system(control);
	ERRDATA debugFree(&control);

	exitstatus=system("which curl 2>&1 >/dev/null");
	gotcurl=(bool)exitstatus;

	if(gotcurl==0)
		sinkReturn=asprintf(&command,"curl %s -s -o -",NAGTIMELINK);
	else
		{
			exitstatus=system("which wget 2>&1 >/dev/null");
			gotwget=(bool)exitstatus;
			if(gotwget==0)
				sinkReturn=asprintf(&command,"wget %s -O - -q",NAGTIMELINK);
		}

	if(command!=NULL)
		{
			t1[0]=0;
			vers[0]=0;
			plugt[0]=0;
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					sinkReturnStr=fgets(t1,1024,fp);
					sinkReturnStr=fgets(vers,1024,fp);
					sinkReturnStr=fgets(plugt,1024,fp);
					pclose(fp);
				}
			t1[strlen(t1)-1]=0;
			vers[strlen(vers)-1]=0;
			plugt[strlen(plugt)-1]=0;

			sinkReturn=asprintf(&control,"echo \"quit\" > %s/updatecontrol",tmpFolderName);
			sinkReturn=system(control);
			ERRDATA debugFree(&control);
			if(strlen(t1)>1)
				{
					thisupdate=atol(t1);
					if((thisupdate>lastUpdate) ||(strcmp(VERSION,vers)!=0))
						{
							GtkWidget *dialog;

							if(strcmp(VERSION,vers)!=0)
								{
									ERRDATA debugFree(&kkeditupdatemessage);
									sinkReturn=asprintf(&kkeditupdatemessage,"%s <b>%s</b> %s <b>%s</b>\n%s:\n<b>%s</b>\n",KKEDIT_UPDATE_AVAILABLE_LABEL,vers,UPDATE_FROM_VERS_LABEL,VERSION,FROM_HERE_LABEL,MYWEBSITE);
								}

							if(lastPlugUpdate<atol(plugt))
								{
									ERRDATA debugFree(&plugupdatemessage);
									sinkReturn=asprintf(&plugupdatemessage,"\n%s:\n<b>https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins</b>",PLUGIN_UPDATES_LABEL);
									lastPlugUpdate=thisupdate;
								}

							dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",UPDATES_AVAILABLE_LABEL);
							gtk_message_dialog_format_secondary_markup((GtkMessageDialog*)dialog,"%s%s",kkeditupdatemessage,plugupdatemessage);

							gtk_dialog_run(GTK_DIALOG(dialog));
							gtk_widget_destroy(dialog);
							ERRDATA debugFree(&kkeditupdatemessage);
							ERRDATA debugFree(&plugupdatemessage);
							lastUpdate=thisupdate;
						}
				}
			ERRDATA debugFree(&command);
		}

	if((nagScreen==false))
		doNagScreen();

	sinkReturn=asprintf(&control,"echo \"quit\" > %s/updatecontrol",tmpFolderName);
	sinkReturn=system(control);
	ERRDATA debugFree(&control);
}

void activate(GApplication *application)
{
	ERRDATA
	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);
}

void doKKCommand(const char *command)
{
	char		*commanddata;
//	char		*folder;
	char		commandname;
	long		line;
	pageStruct	*page=getPageStructByIDFromPage(-1);

	commanddata=basename((char*)command);
	commandname=commanddata[2];
	commanddata+=2;

	commanddata++;

//	printf(">>command = %c<<\n",commandname);
//	printf(">>com data= %s<<\n",commanddata);
//	printf(">>folder = %s<<\n",folder);
	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);

	switch(commandname)
		{
//goto line on current page
			case 'G':
				line=atoi(commanddata);
				if(fromGOpen==true)
					{
						gtk_widget_show_all((GtkWidget*)(GtkTextView*)page->view);
//						if(line>0)
//						while(gtk_events_pending())
//							gtk_main_iteration();
					}
				gotoLine(NULL,(void*)line);
				break;
//search for define
			case 'S':
				defSearchFromBar((GtkWidget*)commanddata,NULL);
				break;
//switch to tab by name
			case 'N':
				for(int j=-0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
					{
						page=getPageStructByIDFromPage(j);
						if(strcmp(page->fileName,commanddata)==0)
							{
								gtk_notebook_set_current_page(mainNotebook,j);
								break;
							}
					}
				break;
//toggle bookmark
			case 'B':
				toggleBookmark(NULL,NULL);
				break;
//close current tab
			case 'C':
				closeTab(NULL,(void*)-1);
				break;
		}
	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);
}

void open(GApplication *application,GFile** files,gint n_files,const gchar *hint)
{
	ERRDATA
	char	*filepath=NULL;
	char	*linenum=NULL;
	int		line=0;
	char	*basepart=NULL;

	g_application_hold(application);
	fromGOpen=true;

	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);

	for(int i=0; i<n_files; i++)
		{
			filepath=g_file_get_path(files[i]);
			basepart=basename(filepath);
			if((basepart[0]=='@') &&(basepart[1]=='@'))
				{
					doKKCommand(filepath);
					continue;
				}
			linenum=strrchr(filepath,'@');
			if(linenum!=NULL)
				{
					*linenum=0;
					linenum++;
					line=atoi(linenum);
				}
			
			if(filepath!=NULL)
				{
					openFile(filepath,line,true);
					ERRDATA g_free(filepath);
				}
		}

	fromGOpen=false;
	g_application_release(application);
	ERRDATA
}

void appStart(GApplication  *application,gpointer data)
{
	ERRDATA
	int	w,h;

	g_application_hold(application);

	setlocale(LC_ALL,"");
	bindtextdomain(APPEXECNAME,LOCALEDIR);
	textdomain(APPEXECNAME);
	bind_textdomain_codeset(APPEXECNAME,"UTF-8");

	init();
	buildMainGui();

	if(onExitSaveSession==true)
		restoreSession(NULL,(void*)restoreBookmarks);

	refreshMainWindow();

	buildFindReplace();
	for(unsigned int j=0; j<g_slist_length(findList); j++)
		gtk_combo_box_text_append_text((GtkComboBoxText*)findDropBox,(const char*)g_slist_nth_data(findList,j));

	for(unsigned int j=0; j<g_slist_length(replaceList); j++)
		gtk_combo_box_text_append_text((GtkComboBoxText*)replaceDropBox,(const char*)g_slist_nth_data(replaceList,j));

#ifdef _BUILDDOCVIEWER_
	buildGtkDocViewer();
#endif

	gtk_window_get_size((GtkWindow*)mainWindow,&w,&h);
	gtk_paned_set_position((GtkPaned*)mainVPane,toolOutHeight);

	gtk_paned_set_position((GtkPaned*)secondWindowVPane,topVPaneHite);
	gtk_paned_set_position((GtkPaned*)mainWindowVPane,bottomVPaneHite);

	gtk_widget_hide(toolOutVBox);
	if(getuid()!=0)
		{
			gtk_window_set_default_icon_name(PACKAGE);
			gtk_window_set_icon_name((GtkWindow*)mainWindow,PACKAGE);
		}
	else
		{
			gtk_window_set_default_icon_name(PACKAGE "Root");
			gtk_window_set_icon_name((GtkWindow*)mainWindow,PACKAGE "Root");
		}

	if((timeToNag==true) &&(autoCheck==true))
		doNagStuff();

/*
	char	*tabcss=NULL;

	provider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	sinkReturn=asprintf(&tabcss,"* {\n \
  font-family: mono;\n \
  font-size: 8px;\n \
  border: 0px solid;\n \
  padding: 4px; \n \
}\n");

*/

#ifdef _USEGTK3_
	char	*tabcss=NULL;

	provider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	sinkReturn=asprintf(&tabcss,"* {\n \
  padding: %ipx; \n \
}\n",tabsSize);

	tabBoxProvider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	gtk_css_provider_load_from_data((GtkCssProvider*)tabBoxProvider,tabcss,-1,NULL);
	debugFree(&tabcss);
#endif

	setPageSensitive();
	gtk_widget_set_size_request(mainWindow,800,400);
	ERRDATA
}

int getWorkspaceNumber(void)
{
	ERRDATA
	GdkDisplay		*display;
	GdkWindow		*root_win;
	Atom			_net_current_desktop,type;
	int				format;
	unsigned long	n_items, bytes_after;
	unsigned char	*data_return=0;
	int				retnum=0;

	display=gdk_screen_get_display(gdk_screen_get_default());
	root_win=gdk_screen_get_root_window(gdk_screen_get_default());

	_net_current_desktop=gdk_x11_get_xatom_by_name_for_display(display,"_NET_CURRENT_DESKTOP");

	XGetWindowProperty(GDK_DISPLAY_XDISPLAY(display),GDK_WINDOW_XID(root_win),_net_current_desktop,0,G_MAXLONG,False,XA_CARDINAL,&type,&format,&n_items,&bytes_after,&data_return);

	if(type==XA_CARDINAL && format==32 && n_items>0)
		{
			retnum=(int)data_return[0];
			XFree(data_return);
		}
	ERRDATA return retnum;
}

int main(int argc, char **argv)
{
	ERRDATA
	int				status;
	char			*dbusname;
	bool			safeflag=false;
	GOptionContext	*context;

#if _DEBUGLEVEL_ > DBG0
	int				data[]={SIGSEGV,SIGFPE,SIGILL,SIGBUS,SIGINT,SIGABRT};
	int				num=6;

	for(int j=0;j<num;j++)
		{
			if(signal(data[j],catchSignal)==SIG_ERR)
				{
        			fprintf(stderr,"An error occurred while setting a signal handler.\n");
					ERRDATA return(EXIT_FAILURE);
				}
		}
#endif

	singleOverRide=false;
	loadPluginsFlag=true;
	loadingSession=false;

	GOptionEntry	entries[]=
{
    {"multiple",'m',0,G_OPTION_ARG_NONE,&singleOverRide,"Multiple instance mode",NULL},
    { "safe",'s',0,G_OPTION_ARG_NONE,&safeflag,"Safe mode(disable all plugins and use new instance )",NULL},
    { NULL }
};

	context=g_option_context_new(NULL);
	g_option_context_add_main_entries(context,entries,NULL);
	g_option_context_set_help_enabled(context,true); 
	g_option_context_parse(context,&argc,&argv,NULL);

	if(safeflag==true)
		{
			singleOverRide=true;
			loadPluginsFlag=false; 
		}

	getOldConfigs("kkedit.rc",kkedit_startup_vars);

	gtk_init(&argc,&argv);

	sinkReturn=asprintf(&dbusname,"org.keithhedger%i." APPEXECNAME,getWorkspaceNumber());
	if((singleOverRide==true) ||(singleUse==false))
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_OPEN));
	else
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_HANDLES_OPEN));

	g_signal_connect(mainApp,"activate",G_CALLBACK(activate),NULL);
	g_signal_connect(mainApp,"startup",G_CALLBACK(appStart),NULL);
	g_signal_connect(mainApp,"open",G_CALLBACK(open),NULL);

	status=g_application_run(mainApp,argc,argv);

	g_object_unref(mainApp);
	ERRDATA delete globalHistory;
	ERRDATA delete globalSlice;

	ERRDATA return(status);
}

