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
#include <unique/unique.h>

#include "globals.h"
#include "files.h"
#include "callbacks.h"
#include "guis.h"
#include "config.h"

bool singleOverRide=false;

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
	if(fd!=NULL)
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
					if(strcasecmp(name,"singleuse")==0)
							singleUse=(bool)atoi(strarg);

					if(strcasecmp(name,"savesessiononexit")==0)
							onExitSaveSession=(bool)atoi(strarg);
					if(strcasecmp(name,"restorebookmarks")==0)
							restoreBookmarks=(bool)atoi(strarg);

					if(strcasecmp(name,"showjtoline")==0)
							showJumpToLine=(bool)atoi(strarg);
					if(strcasecmp(name,"showfindapi")==0)
							showFindAPI=(bool)atoi(strarg);
					if(strcasecmp(name,"showfinddef")==0)
							showFindDef=(bool)atoi(strarg);
					if(strcasecmp(name,"showlivesearch")==0)
							showLiveSearch=(bool)atoi(strarg);

					if(strcasecmp(name,"tabwidth")==0)
							tabWidth=atoi(strarg);
					if(strcasecmp(name,"depth")==0)
							depth=atoi(strarg);

					if(strcasecmp(name,"font")==0)
						{
							sscanf(buffer,"%*s %s %i",(char*)&strarg,(int*)&intarg);
							asprintf(&fontAndSize,"%s %i",strarg,intarg);
						}
					if(strcasecmp(name,"terminalcommand")==0)
						{
							g_free(terminalCommand);
							sscanf(buffer,"%*s %"VALIDCHARS"s",(char*)&strarg);
							terminalCommand=strdup(strarg);
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
				}
			fclose(fd);
		}
	g_free(filename);
}

void init(void)
{
	char*				filename;
#ifdef _ASPELL_
	AspellCanHaveError*	possible_err;
#endif
	indent=true;
	lineNumbers=true;
	lineWrap=true;
	highLight=true;
	tabWidth=4;
	depth=1;
	singleUse=true;
	fontAndSize=strdup("mono 10");
	terminalCommand=strdup("xterm -e");
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
	showJumpToLine=true;
	showFindAPI=true;
	showFindDef=true;
	showLiveSearch=true;

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);

	readConfig();

	tmpIndent=indent;
	tmpLineNumbers=lineNumbers;
	tmpLineWrap=lineWrap;
	tmpHighLight=highLight;
	tmpSingleUse=singleUse;
	tmpTabWidth=tabWidth;
	tmpDepth=depth;
	tmpSaveSessionOnExit=onExitSaveSession;
	tmpRestoreBookmarks=restoreBookmarks;

	tmpShowJumpToLine=showJumpToLine;
	tmpShowFindAPI=showFindAPI;
	tmpShowFindDef=showFindDef;
	tmpShowLiveSearch=showLiveSearch;

	filename=tempnam(NULL,"KKEdit");
	asprintf(&htmlFile,"%s.html",filename);
	asprintf(&htmlURI,"file://%s.html",filename);
	g_free(filename);

#ifdef _ASPELL_
	aspellConfig=new_aspell_config();
	possible_err=new_aspell_speller(aspellConfig);

	if(aspell_error_number(possible_err)!= 0)
		puts(aspell_error_message(possible_err));
	else
		spellChecker=to_aspell_speller(possible_err);
#endif

}

int main(int argc,char **argv)
{
	UniqueApp*			app;
	UniqueMessageData*	message=NULL;
	UniqueCommand 		command;
	UniqueResponse 		response;
	UniqueBackend*		back;
	char*				dbusname;

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

			for(int j=1;j<argc;j++)
				openFile(argv[j],0);

			refreshMainWindow();

			buildFindReplace();

#ifdef BUILDDOCVIEWER
			buildGtkDocViewer();
#endif
			unique_app_watch_window(app,(GtkWindow*)window);
			g_signal_connect(app,"message-received",G_CALLBACK(messageReceived),NULL);
			setSensitive();

			gtk_main();
		}
}
