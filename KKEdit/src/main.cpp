/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include "kkedit-includes.h"

bool	singleOverRide=false;
bool	loadPluginsFlag=true;

void readConfig(void)
{
	char*	filename;

	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	loadVarsFromFile(filename,kkedit_rc);
	debugFree(filename,"readConfig filename");

	asprintf(&filename,"%s/.KKEdit/kkedit.window.rc",getenv("HOME"));
	loadVarsFromFile(filename,kkedit_window_rc);
	if(windowAllocData!=NULL)
		sscanf(windowAllocData,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);

	if(docWindowAllocData!=NULL)
		sscanf(docWindowAllocData,"%i %i %i %i",(int*)&docWindowWidth,(int*)&docWindowHeight,(int*)&docWindowX,(int*)&docWindowY);

	debugFree(filename,"readConfig filename");
}
readco
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
	fontAndSize=strdup("mono 10");
	terminalCommand=strdup("xterm -e");
	rootCommand=strdup(GTKSUPATH);
	autoShowMinChars=4;
	autoShowComps=false;

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

	asprintf(&filename,"%s/.KKEdit/tools",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	debugFree(filename,"init filename");

	schemeManager=gtk_source_style_scheme_manager_get_default();
	asprintf(&filename,"%s/.gnome2/gedit/styles",getenv("HOME"));
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	debugFree(filename,"init filename");
	asprintf(&filename,"%s/styles",DATADIR);
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	debugFree(filename,"init filename");

//toolbar layout
	toolBarLayout=strdup("NOSsXCPsURsFGsE9ADL");

	readConfig();
	loadKeybindings();

	styleScheme=gtk_source_style_scheme_manager_get_scheme(schemeManager,styleName);

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

	history=new HistoryClass;
	globalSlice->setReturnDupString(true);

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
}

void doNagScreen(void)
{
	GtkWidget* dialog;

	dialog=gtk_message_dialog_new((GtkWindow*)window,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",gettext("Please donate"));
	gtk_message_dialog_format_secondary_markup((GtkMessageDialog*)dialog,"%s\n<b>%s</b>\n%s\n\n%s",gettext("If you have a PayPal account you can donate any amount you like by logging into yor account and click the 'Send Money' tab, enter my email address"),MYEMAIL,gettext("and then send it."),gettext("Thank you for helping to support Free software."));

	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

void doNagStuff(void)
{
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

	debugFree(command,"command pulse from donagstuff");

	asprintf(&control,"echo \"pulse\" > \"%s/updatecontrol\"",tmpFolderName);
	system(control);
	debugFree(control,"control from donagstuff");

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
			debugFree(control,"control from do nag stuff");
			if(strlen(t1)>1)
				{
					thisupdate=atol(t1);
					if((thisupdate>lastUpdate) || (strcmp(VERSION,vers)!=0))
						{
							GtkWidget* dialog;

							if(strcmp(VERSION,vers)!=0)
								{
									free(kkeditupdatemessage);
									asprintf(&kkeditupdatemessage,"%s <b>%s</b> %s <b>%s</b>\n%s:\n<b>%s</b>\n",gettext("KKEdit update available to"),vers,gettext("from"),VERSION,gettext("From here"),MYWEBSITE);
								}

							if(lastPlugUpdate<atol(plugt))
								{
									free(plugupdatemessage);
									asprintf(&plugupdatemessage,"\n%s:\n<b>https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins</b>",gettext("Plugin updates are available from here"));
									lastPlugUpdate=thisupdate;
								}

							dialog=gtk_message_dialog_new((GtkWindow*)window,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",gettext("Updates Available"));
							gtk_message_dialog_format_secondary_markup((GtkMessageDialog*)dialog,"%s%s",kkeditupdatemessage,plugupdatemessage);

							gtk_dialog_run(GTK_DIALOG (dialog));
							gtk_widget_destroy(dialog);
							free(kkeditupdatemessage);
							free(plugupdatemessage);
							lastUpdate=thisupdate;
						}
				}
			debugFree(command,"command from donagstuff");
		}

	if((nagScreen==false))
		doNagScreen();

	asprintf(&control,"echo \"quit\" > %s/updatecontrol",tmpFolderName);
	system(control);
	debugFree(control,"control from do nag stuff");
}

int main(int argc,char **argv)
{
	UniqueApp*			app;
	UniqueMessageData*	message=NULL;
	UniqueCommand 		command;
	UniqueResponse 		response;
	UniqueBackend*		back;
	char*				dbusname;
	int					w,h;

	setlocale(LC_ALL,"");
	bindtextdomain("kkedit",LOCALEDIR);
	textdomain("kkedit");
	bind_textdomain_codeset("kkedit","UTF-8");

	gtk_init(&argc,&argv);
	back=unique_backend_create();
	asprintf(&dbusname,"org.keithhedger%i.KKEdit",unique_backend_get_workspace(back));
	app=unique_app_new(dbusname,NULL);
	message=unique_message_data_new();

	readConfig();
	if((argc>1) && (strcmp(argv[1],"-m")==0))
		singleOverRide=true;

	if((argc>1) && (strcmp(argv[1],"-s")==0))
		{
			singleOverRide=true;
			loadPluginsFlag=false;
		}

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
			//handle_fail_or_user_cancel();
		}
	else
		{
			init();
			buildMainGui();

			if(onExitSaveSession==true)
				restoreSession(NULL,(void*)restoreBookmarks);

			for(int j=1; j<argc; j++)
				{
					if((strncasecmp(argv[j],"-m",2)!=0) && (strncasecmp(argv[j],"-s",2)!=0))
						openFile(argv[j],0,true);
				}
			refreshMainWindow();

			buildFindReplace();

#ifdef _BUILDDOCVIEWER_
			buildGtkDocViewer();
#endif
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
			setSensitive();

			if(timeToNag==true)
				doNagStuff();

			gtk_main();

			delete history;
			delete globalSlice;
		}
}