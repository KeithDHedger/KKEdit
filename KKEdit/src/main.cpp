/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include "kkedit-includes.h"
#include <time.h>

bool	singleOverRide=false;
bool	loadPlugins=true;

void readConfig(void)
{
	char*	filename;
	char*	windowdata=NULL;

	args mydata[]={
					//bools
					{"indentcode",3,&indent},
					{"showlinenumbers",3,&lineNumbers},
					{"wrapline",3,&lineWrap},
					{"highlightcurrentline",3,&highLight},
					{"singleuse",3,&singleUse},
					{"noduplicates",3,&noDuplicates},
					{"warning",3,&noWarnings},
					{"savesessiononexit",3,&onExitSaveSession},
					{"restorebookmarks",3,&restoreBookmarks},
					{"nagscreen",3,&nagScreen},
					{"readlink",3,&readLinkFirst},
					//strings
					{"stylename",2,&styleName},
					{"higlightcolour",2,&highlightColour},
					{"toolbarlayout",2,&toolBarLayout},
					{"font",2,&fontAndSize},
					{"terminalcommand",2,&terminalCommand},
					{"rootcommand",2,&rootCommand},
					{"defaultbrowser",2,&browserCommand},
					//ints
					{"tabwidth",1,&tabWidth},
					{"depth",1,&depth},
					{"funcsort",1,&listFunction},
					{NULL,0,NULL}
				  };

	args mydata2[]={
					//bools
					{"insenssearch",3,&insensitiveSearch},
					{"useregex",3,&useRegex},
					{"wrapsearch",3,&wrapSearch},
					{"replaceall",3,&replaceAll},
					{"allfiles",3,&findInAllFiles},
					{"showbmbar",3,&showBMBar},
					{"showtoolbar",3,&showToolBar},
					{"showstatusbar",3,&showStatus},
					{"highlightall",3,&hightlightAll},
					//strings
					{"windowsize",2,&windowdata},
					//ints
					{"toolouthite",1,&toolOutHeight},
					{"nagtime",1,&lastNagTime},
					{"lastupdate",1,&lastUpdate},
					{"lastplugupdate",1,&lastPlugUpdate},
					{NULL,0,NULL}
				  };
	

	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	loadVarsFromFile(filename,mydata);
	debugFree(filename,"readConfig filename");

	asprintf(&filename,"%s/.KKEdit/kkedit.window.rc",getenv("HOME"));
	loadVarsFromFile(filename,mydata2);
	if(windowdata!=NULL)
		sscanf(windowdata,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);
	debugFree(filename,"readConfig filename");
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
	tabWidth=4;
	depth=1;
	singleUse=true;
	fontAndSize=strdup("mono 10");
	terminalCommand=strdup("xterm -e");
	rootCommand=strdup(GTKSUPATH);
	windowWidth=800;
	windowHeight=400;
	windowX=-1;
	windowY=-1;
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
	globalPlugins=new PluginClass(loadPlugins);
//set up plugin data		
	globalPlugins->globalPlugData=(plugData*)malloc(sizeof(plugData));
	globalPlugins->globalPlugData->dataDir=DATADIR;
	globalPlugins->globalPlugData->gPlugFolder=globalPlugins->plugFolderPaths[GLOBALPLUGS];
	globalPlugins->globalPlugData->lPlugFolder=globalPlugins->plugFolderPaths[LOCALPLUGS];
	globalPlugins->globalPlugData->htmlFile=htmlFile;
	globalPlugins->globalPlugData->thePage=&thePage;
	globalPlugins->globalPlugData->currentTab=-1;
	globalPlugins->globalPlugData->tmpFolder=tmpFolderName;

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
}

void doNagScreen(void)
{
	GtkWidget* dialog;

	dialog=gtk_message_dialog_new((GtkWindow*)window,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"Please donate");
	gtk_message_dialog_format_secondary_markup((GtkMessageDialog*)dialog,"If you have a PayPal account you can donate any amount you like by logging into yor account and click the 'Send Money' tab, enter my email address\n<b>%s</b>\nand then send it.\n\nThank you for helping to support Free software.",MYEMAIL);

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
	
	asprintf(&command,"KKEditProgressBar \"Checking for updates ...\" %s/updatecontrol &",tmpFolderName);
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
									asprintf(&kkeditupdatemessage,"KKEdit update available to <b>%s</b> from <b>%s</b>\nFrom here:\n<b>%s</b>\n",vers,VERSION,MYWEBSITE);
								}
						
							if(lastPlugUpdate<atol(plugt))
								{
									free(plugupdatemessage);
									asprintf(&plugupdatemessage,"\nPlugin updates are available from here:\n<b>https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins</b>"); 
									lastPlugUpdate=thisupdate;
								}

							dialog=gtk_message_dialog_new((GtkWindow*)window,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"Updates Available");
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
			loadPlugins=false;
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

