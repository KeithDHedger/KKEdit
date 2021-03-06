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
#include <sys/msg.h>

#include "kkedit-includes.h"

bool	singleOverRide=false;
bool	loadPluginsFlag=true;
char	*sessionName=NULL;

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
	char	*scrwid;
	char	*scrhite;

	getOldConfigs("kkedit.rc",kkedit_rc);

	getOldConfigs("kkedit.window.rc",kkedit_window_rc);
	if(windowAllocData!=NULL)
		sscanf(windowAllocData,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);

	if(docWindowAllocData!=NULL)
		sscanf(docWindowAllocData,"%i %i %i %i",(int*)&docWindowWidth,(int*)&docWindowHeight,(int*)&docWindowX,(int*)&docWindowY);


	scrwid=oneLiner("xdpyinfo | awk -F'[ x]+' '/dimensions:/{print $3}'");
	scrhite=oneLiner("xdpyinfo | awk -F'[ x]+' '/dimensions:/{print $4}'");

	if(atoi(scrwid)<windowX)
		windowX=10;
	if(atoi(scrhite)<windowY)
		windowY=10;
	if(atoi(scrwid)<docWindowX)
		docWindowX=10;
	if(atoi(scrhite)<docWindowY)
		docWindowY=10;

	free(scrwid);
	free(scrhite);
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
	inWindow=false;

	wrapSearch=true;
	insensitiveSearch=true;
	useRegex=false;
	replaceAll=false;
	onExitSaveSession=false;
	noDuplicates=false;
	noWarnings=false;
	readLinkFirst=false;
	hightlightAll=true;
	maxFuncDefs=64;
	maxBMChars=64;
	showMenuIcons=false;

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

	localeLang=getenv("LANG");
	if(localeLang==NULL)
		localeLang="en_GB";
		
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

void activate(GApplication *application,gpointer data)
{
	ERRDATA
	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);
	if(sessionName!=NULL)
		{
			restoreSession(NULL,(gpointer)sessionName);
			debugFree(&sessionName);
		}

	getMsg();
}

void open(GApplication *application,GFile** files,gint n_files,const gchar *hint)
{
	ERRDATA
	char	*filepath=NULL;
	char	*linenum=NULL;
	int		line=0;

	g_application_hold(application);
	fromGOpen=true;

	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);

	for(int i=0; i<n_files; i++)
		{
			filepath=g_file_get_path(files[i]);
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
	if(waitForFinish==true)
		{
			waitForFinish=false;
			sendOK();
		}
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
	if((onExitSaveSession==true) && (safeMode==false) && (overRideLoadSession==false))
		restoreSession(NULL,NULL);

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

#ifdef _USEGTK3_
	tabBoxProvider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	provider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
#endif

	setPageSensitive();
	gtk_widget_set_size_request(mainWindow,800,400);

//autosave timer
	if(autoSavePeriod>0)
		autoSaveSource=g_timeout_add_seconds(autoSavePeriod*60,autoSaveCallBack,NULL);

	ERRDATA
}

gint commandLine(GApplication *application,GApplicationCommandLine *command_line,gpointer user_data)
{
	int		argc=0;
	char	*linenum=NULL;
	int		line=0;
	gchar **args=g_application_command_line_get_arguments(command_line,&argc);

	for(int j=1;j<argc;j++)
		{
			linenum=strrchr(args[j],'@');
			if(linenum!=NULL)
				{
					*linenum=0;
					linenum++;
					line=atoi(linenum);
				}
			
			if(args[j]!=NULL)
				{
					openFile(args[j],line,true);
				}
		}
	g_strfreev(args);
	return(0);
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
	ERRDATA return retnum+100;
}

int main(int argc, char **argv)
{
	ERRDATA

	int				status;
	char			*dbusname;
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
	sessionID=-1;

	GOptionEntry	entries[]=
{
    {"multiple",'m',0,G_OPTION_ARG_NONE,&singleOverRide,"Multiple instance mode",NULL},
    {"no-autosession",'a',0,G_OPTION_ARG_NONE,&overRideLoadSession,"Don't auto load last session",NULL},
    {"sessionid",'i',0,G_OPTION_ARG_INT,&sessionID,"Set an ID to be used for (new) instance",NULL},
    {"session",'f',0,G_OPTION_ARG_STRING,&sessionName,"Load a session for a new instance",NULL},
    { "safe",'s',0,G_OPTION_ARG_NONE,&safeMode,"Safe mode(disable all plugins and use new instance )",NULL},
    { NULL }
};
	context=g_option_context_new(NULL);
	g_option_context_add_main_entries(context,entries,NULL);

	g_option_context_set_help_enabled(context,true); 
	g_option_context_parse(context,&argc,&argv,NULL);

	if(safeMode==true)
		{
			singleOverRide=true;
			loadPluginsFlag=false; 
		}

	getOldConfigs("kkedit.rc",kkedit_startup_vars);

	gtk_init(&argc,&argv);

	if(sessionID==-1)
		sessionID=getWorkspaceNumber();

	sinkReturn=asprintf(&dbusname,"org.keithhedger%i." APPEXECNAME,sessionID);

	if((singleOverRide==true) ||(singleUse==false))
		{
			mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_OPEN|G_APPLICATION_HANDLES_COMMAND_LINE));
			g_signal_connect(mainApp,"command-line",G_CALLBACK(commandLine),NULL);
		}
	else
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_HANDLES_OPEN));

	createQueue();

	g_signal_connect(mainApp,"activate",G_CALLBACK(activate),NULL);
	g_signal_connect(mainApp,"startup",G_CALLBACK(appStart),NULL);
	g_signal_connect(mainApp,"open",G_CALLBACK(open),NULL);

	status=g_application_run(mainApp,argc,argv);
	g_object_unref(mainApp);

	ERRDATA delete globalHistory;
	ERRDATA delete globalSlice;

	ERRDATA return(status);
}

