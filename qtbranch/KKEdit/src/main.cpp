/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/


#include "kkedit-includes.h"

#ifndef _USEQT5_
#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#endif


bool	singleOverRide=false;
bool	loadPluginsFlag=true;

void readConfig(void)
{
	char*	filename;

	asprintf(&filename,"%s/" KKEDITFOLDER "/kkedit.rc",getenv("HOME"));
	loadVarsFromFile(filename,kkedit_rc);
	debugFree(&filename,"readConfig filename");

	asprintf(&filename,"%s/" KKEDITFOLDER "/kkedit.window.rc",getenv("HOME"));
	loadVarsFromFile(filename,kkedit_window_rc);
	if(windowAllocData!=NULL)
		sscanf(windowAllocData,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);

	if(docWindowAllocData!=NULL)
		sscanf(docWindowAllocData,"%i %i %i %i",(int*)&docWindowWidth,(int*)&docWindowHeight,(int*)&docWindowX,(int*)&docWindowY);

	debugFree(&filename,"readConfig filename");
}

void init(void)
{
	char*		filename;
	int			exitstatus;
	char		tmpfoldertemplate[]="/tmp/KKEdit-XXXXXX";

//nag times
	lastNagTime=-1;
	nagTime=(long)time(NULL);

#ifdef _ASPELL_
	AspellCanHaveError*	possible_err;
#endif
	globalSlice=new StringSlice;

	tmpFolderName=strdup(mkdtemp(tmpfoldertemplate));

	indent=true;
	lineNumbers=true;
	lineWrap=true;
	highLight=true;
	noSyntax=false;
	tabWidth=4;
	depth=1;
	singleUse=true;
	fontAndSize=strdup("Monospace 10");
	terminalCommand=strdup("xterm -e");
	rootCommand=strdup(GTKSUPATH);
	autoShowMinChars=4;
	autoShowComps=false;
	autoCheck=true;
	useGlobalPlugMenu=true;
	maxTabChars=20;

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
	maxFuncDefs=MAXTEXTWIDTH;
	maxBMChars=MAXTEXTWIDTH;

//runtime deps
	exitstatus=system("which manpageeditor 2>&1 >/dev/null");
	gotManEditor=WEXITSTATUS(exitstatus);
	exitstatus=system("which doxygen 2>&1 >/dev/null");
	gotDoxygen=WEXITSTATUS(exitstatus);

	if(getuid()!=0)
		styleName=strdup("classic");
	else
		styleName=strdup("Root Source");
	nagScreen=false;
	highlightColour=strdup("#A3ACFF");
	showBMBar=false;
	listFunction=0;
	showStatus=true;

	asprintf(&filename,"%s/" KKEDITFOLDER "/tools",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	debugFree(&filename,"init filename");

//TODO//
#ifndef _USEQT5_
	schemeManager=gtk_source_style_scheme_manager_get_default();
	asprintf(&filename,"%s/.gnome2/gedit/styles",getenv("HOME"));
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	debugFree(&filename,"init filename");
	asprintf(&filename,"%s/styles",DATADIR);
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	debugFree(&filename,"init filename");
#endif

//toolbar layout
	toolBarLayout=strdup("NOSsXCPsURsFGsE9ADL");

	readConfig();
	loadKeybindings();

#ifndef _USEQT5_
	styleScheme=gtk_source_style_scheme_manager_get_scheme(schemeManager,styleName);
#endif

	tmpStyleName=strdup(styleName);
	tmpHighlightColour=highlightColour;

	asprintf(&htmlFile,"%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));
	asprintf(&htmlURI,"file://%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));

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
	globalPlugins->globalPlugData->dataDir=DATADIR;
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
		char*		name;
		bool		enabled;
		GModule*	module;
		bool		loaded;
		char*       path;
	};
				pluginData* pd=(pluginData*)g_list_nth_data(globalPlugins->plugins,j);
				printf("num %i name=%s\n",j,pd->name);
				printf("num %i enabled=%i\n",j,(int)pd->enabled);
				printf("num %i path=%s\n",j,pd->path);
			}
	*/
//time to nag?

	timeToNag=false;
	if((lastNagTime==0) || (nagTime-lastNagTime>updateWait))
		{
			timeToNag=true;
			lastNagTime=nagTime;
		}

	localeLang=getenv("LANG");

	history=new HistoryClass;
	globalSlice->setReturnDupString(true);

//TODO//
#ifndef _USEQT5_

	funcProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	funcProv->priority=1;
	funcProv->name=gettext("Functions");
	funcProv->proposals=NULL;

	varsProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	varsProv->priority=2;
	varsProv->name=gettext("Variables");
	varsProv->proposals=NULL;

	customProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	customProv->priority=11;
	customProv->name=gettext("Custom Words");
	customProv->proposals=NULL;

	docWordsProv=gtk_source_completion_words_new(NULL,NULL);
	g_object_set(docWordsProv,"priority",10,NULL);
	g_object_set(docWordsProv,"minimum-word-size",autoShowMinChars,NULL);
	g_object_set(docWordsProv,"interactive-delay",50,NULL);
#endif
}

#if 0
void initX(void)
{
	char*		filename;
	int			exitstatus;
	char		tmpfoldertemplate[]="/tmp/KKEdit-XXXXXX";

//nag times
	lastNagTime=-1;
	nagTime=(long)time(NULL);

#ifdef _ASPELL_
	AspellCanHaveError*	possible_err;
#endif
	globalSlice=new StringSlice;

	tmpFolderName=strdup(mkdtemp(tmpfoldertemplate));

	indent=true;
	lineNumbers=true;
	lineWrap=true;
	highLight=true;
	noSyntax=false;
	tabWidth=4;
	depth=1;
	singleUse=true;
	fontAndSize=strdup("mono 10");
	terminalCommand=strdup("xterm -e");
	rootCommand=strdup(GTKSUPATH);
	autoShowMinChars=4;
	autoShowComps=false;
	autoCheck=true;

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

//runtime deps
	exitstatus=system("which manpageeditor 2>&1 >/dev/null");
	gotManEditor=WEXITSTATUS(exitstatus);
	exitstatus=system("which doxygen 2>&1 >/dev/null");
	gotDoxygen=WEXITSTATUS(exitstatus);

	if(getuid()!=0)
		styleName=strdup("classic");
	else
		styleName=strdup("Root Source");
	nagScreen=false;
	highlightColour=strdup("#A3ACFF");
	showBMBar=false;
	listFunction=0;
	showStatus=true;

	asprintf(&filename,"%s/" KKEDITFOLDER "/tools",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	debugFree(&filename,"init filename");

#ifndef _USEQT5_
	schemeManager=gtk_source_style_scheme_manager_get_default();
	asprintf(&filename,"%s/.gnome2/gedit/styles",getenv("HOME"));
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	debugFree(&filename,"init filename");
	asprintf(&filename,"%s/styles",DATADIR);
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	debugFree(&filename,"init filename");
#else
//TODO//
#endif

//toolbar layout
	toolBarLayout=strdup("NOSsXCPsURsFGsE9ADL");

	readConfig();
	loadKeybindings();

#ifndef _USEQT5_
	styleScheme=gtk_source_style_scheme_manager_get_scheme(schemeManager,styleName);
#else
//TODO//
#endif

	tmpIndent=indent;
	tmpLineNumbers=lineNumbers;
	tmpLineWrap=lineWrap;
	tmpNoSyntax=noSyntax;
	tmpHighLight=highLight;
	tmpSingleUse=singleUse;
	tmpTabWidth=tabWidth;
	tmpDepth=depth;
	tmpSaveSessionOnExit=onExitSaveSession;
	tmpRestoreBookmarks=restoreBookmarks;
	tmpStyleName=strdup(styleName);
	tmpNoDuplicates=noDuplicates;
	tmpNoWarnings=noWarnings;
	tmpReadLinkFirst=readLinkFirst;
	tmpAutoShowComps=autoShowComps;
	tmpAutoShowMinChars=autoShowMinChars;
	tmpAutoCheck=autoCheck;
	tmpNagScreen=nagScreen;
	tmpHighlightColour=highlightColour;

	asprintf(&htmlFile,"%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));
	asprintf(&htmlURI,"file://%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));

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
#ifndef _USEQT5_
	globalPlugins=new PluginClass(loadPluginsFlag);
//set up plugin data
	globalPlugins->globalPlugData=(plugData*)malloc(sizeof(plugData));
	globalPlugins->globalPlugData->dataDir=DATADIR;
	globalPlugins->globalPlugData->gPlugFolder=globalPlugins->plugFolderPaths[GLOBALPLUGS];
	globalPlugins->globalPlugData->lPlugFolder=globalPlugins->plugFolderPaths[LOCALPLUGS];
	globalPlugins->globalPlugData->htmlFile=htmlFile;
	globalPlugins->globalPlugData->thePage=&thePage;
	globalPlugins->globalPlugData->currentTab=-1;
	globalPlugins->globalPlugData->tmpFolder=tmpFolderName;
	globalPlugins->globalPlugData->kkeditVersion=VERSION;
#else
//TODO//
#endif
	/*
		for(int j=0;j<globalPlugins->plugCount;j++)
			{
				struct pluginData
	{
		char*		name;
		bool		enabled;
		GModule*	module;
		bool		loaded;
		char*       path;
	};
				pluginData* pd=(pluginData*)g_list_nth_data(globalPlugins->plugins,j);
				printf("num %i name=%s\n",j,pd->name);
				printf("num %i enabled=%i\n",j,(int)pd->enabled);
				printf("num %i path=%s\n",j,pd->path);
			}
	*/
//time to nag?

	timeToNag=false;
	if((lastNagTime==0) || (nagTime-lastNagTime>updateWait))
		{
			timeToNag=true;
			lastNagTime=nagTime;
		}

	localeLang=getenv("LANG");

	globalSlice->setReturnDupString(true);

#ifndef _USEQT5_
	history=new HistoryClass;


	funcProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	funcProv->priority=1;
	funcProv->name=gettext("Functions");
	funcProv->proposals=NULL;

	varsProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	varsProv->priority=2;
	varsProv->name=gettext("Variables");
	varsProv->proposals=NULL;

	customProv=(FunctionProvider*)g_object_new(function_provider_get_type(),NULL);
	customProv->priority=11;
	customProv->name=gettext("Custom Words");
	customProv->proposals=NULL;

	docWordsProv=gtk_source_completion_words_new(NULL,NULL);
	g_object_set(docWordsProv,"priority",10,NULL);
	g_object_set(docWordsProv,"minimum-word-size",autoShowMinChars,NULL);
	g_object_set(docWordsProv,"interactive-delay",50,NULL);
#else
//TODO//
#endif
}
#endif
void doNagScreen(void)
{
#ifndef _USEQT5_
	GtkWidget* dialog;

	dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",gettext("Please donate"));
	gtk_message_dialog_format_secondary_markup((GtkMessageDialog*)dialog,"%s\n<b>%s</b>\n%s\n\n%s",gettext("If you have a PayPal account you can donate any amount you like by logging into yor account and click the 'Send Money' tab, enter my email address"),MYEMAIL,gettext("and then send it."),gettext("Thank you for helping to support Free software."));

	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
#endif
}

void doNagStuff(void)
{
#ifndef _USEQT5_
	char*			command=NULL;
	char*			control=NULL;
	int				gotcurl=-1;
	int				gotwget=-1;
	int				exitstatus;
	FILE*			fp;
	char			t1[1024];
	char			vers[1024];
	char			plugt[1024];
	unsigned int	thisupdate=0;
	char*			kkeditupdatemessage=strdup("");
	char*			plugupdatemessage=strdup("");

	asprintf(&command,"KKEditProgressBar \"%s\" %s/updatecontrol &",gettext("Checking for updates ..."),tmpFolderName);
	system(command);
	while(gtk_events_pending())
		gtk_main_iteration();

	debugFree(&command,"command pulse from donagstuff");

	asprintf(&control,"echo \"pulse\" > \"%s/updatecontrol\"",tmpFolderName);
	system(control);
	debugFree(&control,"control from donagstuff");

	exitstatus=system("which curl 2>&1 >/dev/null");
	gotcurl=WEXITSTATUS(exitstatus);

	if(gotcurl==0)
		asprintf(&command,"curl %s -s -o -",NAGTIMELINK);
	else
		{
			exitstatus=system("which wget 2>&1 >/dev/null");
			gotwget=WEXITSTATUS(exitstatus);
			if(gotwget==0)
				asprintf(&command,"wget %s -O - -q",NAGTIMELINK);
		}

	if(command!=NULL)
		{
			t1[0]=0;
			vers[0]=0;
			plugt[0]=0;
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					fgets(t1,1024,fp);
					fgets(vers,1024,fp);
					fgets(plugt,1024,fp);
					pclose(fp);
				}
			t1[strlen(t1)-1]=0;
			vers[strlen(vers)-1]=0;
			plugt[strlen(plugt)-1]=0;

			asprintf(&control,"echo \"quit\" > %s/updatecontrol",tmpFolderName);
			system(control);
			debugFree(&control,"control from do nag stuff");
			if(strlen(t1)>1)
				{
					thisupdate=atol(t1);
					if((thisupdate>lastUpdate) || (strcmp(VERSION,vers)!=0))
						{
							GtkWidget* dialog;

							if(strcmp(VERSION,vers)!=0)
								{
									debugFree(&kkeditupdatemessage,"doNagStuff kkeditupdatemessage");
									asprintf(&kkeditupdatemessage,"%s <b>%s</b> %s <b>%s</b>\n%s:\n<b>%s</b>\n",gettext("KKEdit update available to"),vers,gettext("from"),VERSION,gettext("From here"),MYWEBSITE);
								}

							if(lastPlugUpdate<atol(plugt))
								{
									debugFree(&plugupdatemessage,"doNagStuff plugupdatemessage");
									asprintf(&plugupdatemessage,"\n%s:\n<b>https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins</b>",gettext("Plugin updates are available from here"));
									lastPlugUpdate=thisupdate;
								}

							dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",gettext("Updates Available"));
							gtk_message_dialog_format_secondary_markup((GtkMessageDialog*)dialog,"%s%s",kkeditupdatemessage,plugupdatemessage);

							gtk_dialog_run(GTK_DIALOG (dialog));
							gtk_widget_destroy(dialog);
							debugFree(&kkeditupdatemessage,"doNagStuff kkeditupdatemessage");
							debugFree(&plugupdatemessage,"doNagStuff plugupdatemessage");
							lastUpdate=thisupdate;
						}
				}
			debugFree(&command,"command from donagstuff");
		}

	if((nagScreen==false))
		doNagScreen();

	asprintf(&control,"echo \"quit\" > %s/updatecontrol",tmpFolderName);
	system(control);
	debugFree(&control,"control from do nag stuff");
#endif
}

#ifndef _USEQT5_

void activate(GApplication* application)
{
}

void open(GApplication* application,GFile** files,gint n_files,const gchar* hint)
{
	char*	filepath=NULL;

	g_application_hold(application);

	for(int i=0; i<n_files; i++)
		{
			filepath=g_file_get_path(files[i]);
			if(filepath!=NULL)
				{
					openFile(filepath,0,true);
					debugFree(&filepath,"open filepath");
				}
		}

	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);

	g_application_release(application);
}

void appStart(GApplication  *application,gpointer data)
{
	int	w,h;

	g_application_hold(application);

	setlocale(LC_ALL,"");
	bindtextdomain("kkedit",LOCALEDIR);
	textdomain("kkedit");
	bind_textdomain_codeset("kkedit","UTF-8");

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
	setSensitive();

	if((timeToNag==true) && (autoCheck==true))
		doNagStuff();

	gtk_widget_set_size_request(mainWindow,100,100);
}

int getWorkspaceNumber(void)
{
	int				retnum=666;
	
	GdkDisplay*		display;
	GdkWindow*		root_win;
	Atom			_net_current_desktop,type;
	int				format;
	unsigned long	n_items, bytes_after;
	unsigned char*	data_return=0;

	display=gdk_screen_get_display(gdk_screen_get_default());
	root_win=gdk_screen_get_root_window(gdk_screen_get_default());

	_net_current_desktop=gdk_x11_get_xatom_by_name_for_display(display,"_NET_CURRENT_DESKTOP");

	XGetWindowProperty(GDK_DISPLAY_XDISPLAY(display),GDK_WINDOW_XID (root_win),_net_current_desktop,0,G_MAXLONG,False,XA_CARDINAL,&type,&format,&n_items,&bytes_after,&data_return);

	if(type==XA_CARDINAL && format==32 && n_items>0)
		{
			retnum=(int)data_return[0];
			XFree(data_return);
		}
	return retnum;
}
#endif

#ifdef _USEQT5_
void qtAppStart(int argc, char **argv)
{
	setlocale(LC_ALL,"");
	bindtextdomain("kkedit",LOCALEDIR);
	textdomain("kkedit");
	bind_textdomain_codeset("kkedit","UTF-8");

	init();
	
	buildMainGuiQT();

//TODO//
//	if(onExitSaveSession==true)
//		restoreSession(NULL,(void*)restoreBookmarks);

//	refreshMainWindow();

	buildFindReplace();

	for(unsigned int j=0; j<g_slist_length(findList); j++)
		{
			reinterpret_cast<QComboBox*>(findDropBox)->addItem((const char*)g_slist_nth_data(findList,j));
		}
	reinterpret_cast<QComboBox*>(findDropBox)->setCurrentIndex(g_slist_length(findList)-1);

	for(unsigned int j=0; j<g_slist_length(findList); j++)
		{
			reinterpret_cast<QComboBox*>(replaceDropBox)->addItem((const char*)g_slist_nth_data(replaceList,j));
		}
	reinterpret_cast<QComboBox*>(replaceDropBox)->setCurrentIndex(g_slist_length(findList)-1);

//TODO//
	buildDocViewer();

//#ifdef _BUILDDOCVIEWER_
//	buildGtkDocViewer();
//#endif

//	gtk_window_get_size((GtkWindow*)window,&w,&h);
//	gtk_paned_set_position((GtkPaned*)mainVPane,toolOutHeight);
//
//	gtk_paned_set_position((GtkPaned*)secondWindowVPane,topVPaneHite);
//	gtk_paned_set_position((GtkPaned*)mainWindowVPane,bottomVPaneHite);
//

//TODO//
	setSensitive();
//
//
//	if((timeToNag==true) && (autoCheck==true))
//		doNagStuff();
//
}
#endif

int main (int argc, char **argv)
{

	int				status;
	char*			filename;
	bool			safeflag=false;
	GOptionContext*	context;

	singleOverRide=false;
	loadPluginsFlag=true;

	GOptionEntry	entries[]=
{
    {"multiple",'m',0,G_OPTION_ARG_NONE,&singleOverRide,"Multiple instance mode",NULL},
    { "safe",'s',0,G_OPTION_ARG_NONE,&safeflag,"Safe mode ( disable all plugins and use new instance )",NULL},
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

	asprintf(&filename,"%s/" KKEDITFOLDER "/kkedit.rc",getenv("HOME"));
	loadVarsFromFile(filename,kkedit_startup_vars);
	debugFree(&filename,"main filename");
#ifndef _USEQT5_

	gtk_init(&argc,&argv);

	asprintf(&dbusname,"org.keithhedger%i.KKEdit",getWorkspaceNumber());
	if((singleOverRide==true) || (singleUse==false))
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_OPEN));
	else
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_HANDLES_OPEN));

	g_signal_connect(mainApp,"activate",G_CALLBACK(activate),NULL);
	g_signal_connect(mainApp,"startup",G_CALLBACK (appStart),NULL);
	g_signal_connect(mainApp,"open",G_CALLBACK (open),NULL);

	status=g_application_run(mainApp,argc,argv);

	g_object_unref(mainApp);
#else
	QApplication app(argc,argv);

	qtAppStart(argc,argv);

	for(int j=1;j<argc;j++)
		openFile(argv[j],0,true);

	if(getuid()!=0)
		app.setWindowIcon(QIcon::fromTheme(PACKAGE,QIcon(DATADIR"/pixmaps/KKEdit.png")));
	else
		app.setWindowIcon(QIcon::fromTheme(PACKAGE "Root",QIcon(DATADIR"/pixmaps/KKEditRoot.png")));
	//app.setStyleSheet("QMenu { columnCount: 1;}");
	status=app.exec();
	doShutdown(NULL,1);
#endif
	delete history;
	delete globalSlice;

	return status;
}

#if 0
int main(int argc,char **argv)
{
#ifndef _USEQT5_
	UniqueApp*			app;
	UniqueMessageData*	message=NULL;
	UniqueCommand 		command;
	UniqueResponse 		response;
	UniqueBackend*		back;
#endif
	char*				dbusname;
	int					w,h;

	setlocale(LC_ALL,"");
	bindtextdomain("kkedit",LOCALEDIR);
	textdomain("kkedit");
	bind_textdomain_codeset("kkedit","UTF-8");

#ifndef _USEQT5_
	gtk_init(&argc,&argv);
	back=unique_backend_create();
	asprintf(&dbusname,"org.keithhedger%i.KKEdit",unique_backend_get_workspace(back));
	app=unique_app_new(dbusname,NULL);
	message=unique_message_data_new();
#endif
	readConfig();

	if((argc>1) && (strcmp(argv[1],"-m")==0))
		singleOverRide=true;

	if((argc>1) && (strcmp(argv[1],"-s")==0))
		{
			singleOverRide=true;
			loadPluginsFlag=false;
		}

#ifndef _USEQT5_
	if((unique_app_is_running(app)==true) && (singleUse==true) && (singleOverRide==false))
		{
			if(argc==1)
				{
					command=UNIQUE_ACTIVATE;
					response=unique_app_send_message(app,command,NULL);
				}
			else
				{
					command=UNIQUE_OPEN;
					unique_message_data_set_uris(message,argv);
					response=unique_app_send_message(app,command,message);
				}

			g_object_unref(app);
			unique_message_data_free(message);

			if(response==UNIQUE_RESPONSE_OK)
				return 0;
			else
				printf("FAIL\n");
				return(1);
			//handle_fail_or_user_cancel();
		}
#endif
//	else
//		{
			init();

//
//			for(int j=1; j<argc; j++)
//				{
//				printf("%i - %sn",j,argv[j]);
//				}




#ifndef _USEQT5_
			buildMainGui();
#else
			QApplication	app(argc,argv);

			buildMainGuiQT();
#endif
			if(onExitSaveSession==true)
#ifndef _USEQT5_
				restoreSession(NULL,(void*)restoreBookmarks);
#else
//TODO//
				restoreSession(NULL,(long)restoreBookmarks);	
#endif

#ifndef _USEQT5_
			for(int j=1; j<argc; j++)
				{
					if((strncasecmp(argv[j],"-m",2)!=0) && (strncasecmp(argv[j],"-s",2)!=0))
						openFile(argv[j],0,true);
				}
#else
			for (int j=1;j<app.arguments().count();j++)
				{
					if((strncasecmp(app.arguments().at(j).toUtf8().constData(),"-m",2)!=0) && (strncasecmp(app.arguments().at(j).toUtf8().constData(),"-s",2)!=0))
						openFile(app.arguments().at(j).toUtf8().constData(),0,true);
				}
#endif
			refreshMainWindow();

			buildFindReplace();

#ifdef _BUILDDOCVIEWER_
			buildGtkDocViewer();
#endif

#ifndef _USEQT5_
			unique_app_watch_window(app,(GtkWindow*)window);
			g_signal_connect(app,"message-received",G_CALLBACK(messageReceived),NULL);

			gtk_window_get_size((GtkWindow*)window,&w,&h);
			gtk_paned_set_position((GtkPaned*)mainVPane,toolOutHeight);

			gtk_paned_set_position((GtkPaned*)secondWindowVPane,topVPaneHite);
			gtk_paned_set_position((GtkPaned*)mainWindowVPane,bottomVPaneHite);

			gtk_widget_hide(toolOutVBox);
			if(getuid()!=0)
				{
					gtk_window_set_default_icon_name(PACKAGE);
					gtk_window_set_icon_name((GtkWindow*)window,PACKAGE);
				}
			else
				{
					gtk_window_set_default_icon_name(PACKAGE "Root");
					gtk_window_set_icon_name((GtkWindow*)window,PACKAGE "Root");
				}
#else
//TODO//
#endif
			setSensitive();

			if((timeToNag==true) && (autoCheck==true))
				doNagStuff();

#ifndef _USEQT5_
			gtk_main();
#else
//window->show();
			app.exec();
#endif
			delete history;
			delete globalSlice;
//		}
}
#endif
