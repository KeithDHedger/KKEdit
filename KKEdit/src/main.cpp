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

					if(strcasecmp(name,"insenssearch")==0)
							insensitiveSearch=(bool)atoi(strarg);
					if(strcasecmp(name,"wrapsearch")==0)
							wrapSearch=(bool)atoi(strarg);

					if(strcasecmp(name,"savesessiononexit")==0)
							saveSessionOnExit=(bool)atoi(strarg);

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
							asprintf(&terminalCommand,"%s",strarg);
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
				}
			fclose(fd);
		}
	g_free(filename);
}

void init(void)
{
	char*	filename;

	indent=true;
	lineNumbers=true;
	lineWrap=true;
	highLight=true;
	tabWidth=4;
	depth=1;
	singleUse=true;

	asprintf(&fontAndSize,"%s","mono 10");
	asprintf(&terminalCommand,"%s","xterm");
	windowWidth=800;
	windowHeight=400;
	windowX=-1;
	windowY=-1;
	wrapSearch=true;
	insensitiveSearch=true;
	replaceAll=false;
	saveSessionOnExit=false;

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
	tmpSaveSessionOnExit=saveSessionOnExit;
}

int main(int argc,char **argv)
{
	UniqueApp*			app;
	UniqueMessageData*	message=NULL;
	UniqueCommand 		command;
	UniqueResponse 		response;
	UniqueBackend*		back;
	char*				dbusname;

#if 0
//char* src="/usr/share/gtk-doc/html/glib/glib.devhelp2:    <keyword type=\"function\" name=\"g_string_free_to_bytes ()\" link=\"glib-Strings.html#g-string-free-to-bytes\" since=\"2.34\"/>";
char* deststr;
char* src="this is a string slice test";

//
//deststr=slice(src,4,10);
//printf("--%s--\n",deststr);
//deststr=sliceBetween(src,"link=\"","\"");
//printf("%s\n",src);

deststr=sliceLen(src,-90,100);
if(deststr!=NULL)
printf("--%s--\n",deststr);

deststr=sliceStrLen(src,"",5);
if(deststr!=NULL)
printf("--%s--\n",deststr);


return 0;
#endif

	gtk_init(&argc,&argv);

	back=unique_backend_create();
	asprintf(&dbusname,"org.keithhedger%i.KKEdit",unique_backend_get_workspace(back));
	app=unique_app_new(dbusname,NULL);
	message=unique_message_data_new();

	init();

	if((unique_app_is_running(app)==true) && (singleUse==true))
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

			if(saveSessionOnExit==true)
				restoreSession(NULL,NULL);

			for(int j=1;j<argc;j++)
				openFile(argv[j],0);

			gtk_widget_show_all(window);
			buildFindReplace();
#ifdef BUILDDOCVIEWER
			buildGtkDocViewer();
#endif
			unique_app_watch_window(app,(GtkWindow*)window);
			g_signal_connect(app,"message-received",G_CALLBACK(messageReceived),NULL);

			gtk_main();
		}
}
