/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include <time.h>

#include "callbacks.h"
#include "guis.h"

bool singleOverRide=false;

void readConfig(void)
{
	FILE*	fd=NULL;
	char*	filename;
	char	buffer[1024];
	char	name[256];
	char	strarg[256];

	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	fd=fopen(filename,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					strarg[0]=0;
					name[0]=0;
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
					if(strcasecmp(name,"singleuse")==0)
						singleUse=(bool)atoi(strarg);
					if(strcasecmp(name,"noduplicates")==0)
						noDuplicates=(bool)atoi(strarg);
					if(strcasecmp(name,"warning")==0)
						noWarnings=(bool)atoi(strarg);

					if(strcasecmp(name,"stylename")==0)
						{
							if(styleName!=NULL)
								g_free(styleName);
							sscanf(buffer,"%*s %"VALIDCHARS"s",(char*)&strarg);
							styleName=strdup(strarg);
						}

					if(strcasecmp(name,"savesessiononexit")==0)
						onExitSaveSession=(bool)atoi(strarg);
					if(strcasecmp(name,"restorebookmarks")==0)
						restoreBookmarks=(bool)atoi(strarg);

					if(strcasecmp(name,"tabwidth")==0)
						tabWidth=atoi(strarg);
					if(strcasecmp(name,"depth")==0)
						depth=atoi(strarg);
					if(strcasecmp(name,"nagscreen")==0)
						nagScreen=(bool)atoi(strarg);

					if(strcasecmp(name,"readlink")==0)
						readLinkFirst=(bool)atoi(strarg);

					if(strcasecmp(name,"higlightcolour")==0)
						{
							sscanf(buffer,"%*s %"VALIDCHARS"s",(char*)&strarg);
							highlightColour=strdup(strarg);
						}

					if(strcasecmp(name,"funcsort")==0)
						listFunction=(int)atoi(strarg);

					if(strcasecmp(name,"toolbarlayout")==0)
						{
							sscanf(buffer,"%*s %"VALIDCHARS"s",(char*)&strarg);
							toolBarLayout=strdup(strarg);
						}

					if(strcasecmp(name,"font")==0)
						{
							sscanf(buffer,"%*s %"VALIDCHARS"s",(char*)&strarg);
							fontAndSize=strdup(strarg);
						}

					if(strcasecmp(name,"terminalcommand")==0)
						{
							sscanf(buffer,"%*s %"VALIDCHARS"s",(char*)&strarg);
							if(strlen(strarg)>0)
								{
									free(terminalCommand);
									terminalCommand=strdup(strarg);
								}
						}
					
					if(strcasecmp(name,"rootcommand")==0)
						{
							sscanf(buffer,"%*s %"VALIDCHARS"s",(char*)&strarg);
							if(strlen(strarg)>0)
								{
									free(rootCommand);
									rootCommand=strdup(strarg);
								}
						}
				}
			fclose(fd);
		}

	g_free(filename);
	asprintf(&filename,"%s/.KKEdit/kkedit.window.rc",getenv("HOME"));
	fd=fopen(filename,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					fgets(buffer,1024,fd);
					sscanf(buffer,"%s %s",(char*)&name,(char*)&strarg);

					if(strcasecmp(name,"windowsize")==0)
						sscanf(buffer,"%*s %i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);
					if(strcasecmp(name,"insenssearch")==0)
						insensitiveSearch=(bool)atoi(strarg);
					if(strcasecmp(name,"useregex")==0)
						useRegex=(bool)atoi(strarg);
					if(strcasecmp(name,"wrapsearch")==0)
						wrapSearch=(bool)atoi(strarg);
					if(strcasecmp(name,"replaceall")==0)
						replaceAll=(bool)atoi(strarg);
					if(strcasecmp(name,"allfiles")==0)
						findInAllFiles=(bool)atoi(strarg);
					if(strcasecmp(name,"showbmbar")==0)
						showBMBar=(bool)atoi(strarg);
					if(strcasecmp(name,"showtoolbar")==0)
						showToolBar=(bool)atoi(strarg);
//status bar
					if(strcasecmp(name,"showstatusbar")==0)
						showStatus=(bool)atoi(strarg);
//highlight all
					if(strcasecmp(name,"highlightall")==0)
						hightlightAll=(bool)atoi(strarg);

				}
			fclose(fd);
		}
	g_free(filename);
}

//void gen_random(char *s, const int len) {
//srand(time(0));
//    static const char alphanum[] =
//        "0123456789"
//        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//        "abcdefghijklmnopqrstuvwxyz";
//
//    for (int i = 0; i < len; ++i) {
//        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
//    }
//
//    s[len] = 0;
//}
void init(void)
{
	char*	filename;
	int		exitstatus;
	char	tmpfoldertemplate[]="/tmp/KKEdit-XXXXXX";

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
	g_free(filename);

	schemeManager=gtk_source_style_scheme_manager_get_default();
	asprintf(&filename,"%s/.gnome2/gedit/styles",getenv("HOME"));
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	g_free(filename);
	asprintf(&filename,"%s/styles",DATADIR);
	gtk_source_style_scheme_manager_append_search_path(schemeManager,filename);
	g_free(filename);

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
	free(filename);

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

	init();

	if((argc>1) && (strcmp(argv[1],"-m")==0))
		singleOverRide=true;

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
			buildMainGui();

			if(onExitSaveSession==true)
				restoreSession(NULL,(void*)restoreBookmarks);

			for(int j=1; j<argc; j++)
				{
					if(strncasecmp(argv[j],"-m",2)!=0)
						openFile(argv[j],0,true);
				}
			refreshMainWindow();

			buildFindReplace();

#ifdef _BUILDDOCVIEWER_
			buildGtkDocViewer();
#endif
			unique_app_watch_window(app,(GtkWindow*)window);
			g_signal_connect(app,"message-received",G_CALLBACK(messageReceived),NULL);

			if((nagScreen==false))
				doNagScreen();


			gtk_window_get_size((GtkWindow*)window,&w,&h);
			gtk_paned_set_position((GtkPaned*)mainVPane,h-120);
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
			gtk_main();
			
			delete history;
			delete globalSlice;
		}
}

