/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * globals.cpp
 *
*/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <webkit/webkit.h>

#include "globals.h"

GtkWidget*		window=NULL;
GtkNotebook*	notebook=NULL;
GtkWidget*		menubar=NULL;
GtkWidget*		menufile;
GtkWidget*		menuedit;
GtkWidget*		menufunc;
GtkWidget*		menunav;
GtkWidget*		menutools;
GtkWidget*		menuhelp;

GtkWidget*		menubookmark;
GtkWidget*		menubookmarksub;

GtkWidget*		redoMenu;
GtkWidget*		undoMenu;
GtkWidget*		saveMenu;
GtkWidget*		saveAsMenu;

int				currentPage=0;

GtkWidget*		prefswin;
bool			indent;
bool			lineNumbers;
bool			lineWrap;
bool			highLight;
bool			singleUse;
int				tabWidth;
char*			fontAndSize;
char*			terminalCommand;
int				depth;

GtkWidget*		fontBox;
GtkWidget*		terminalBox;
bool			tmpIndent;
bool			tmpLineNumbers;
bool			tmpLineWrap;
bool			tmpHighLight;
bool			tmpSingleUse;
int				tmpTabWidth;
int				tmpDepth;

GtkWidget*		toolNameWidget;
GtkWidget*		commandLineWidget;
GtkWidget*		inTermWidget;
GtkWidget*		syncWidget;
GtkWidget*		ignoreWidget;
GtkWidget*		pasteWidget;
GtkWidget*		replaceWidget;

bool			inTerm=false;
bool			runSync=true;
bool			ignoreOut=true;
bool			pasteOut=false;
bool			replaceOut=false;
bool			editTool=false;

int				windowWidth;
int				windowHeight;
int				windowX=-1;
int				windowY=-1;
bool			wrapSearch;
bool			insensitiveSearch;
bool			replaceAll;

int				currentTabNumber;
int 			untitledNumber=1;

GtkToolItem*	newButton;
GtkToolItem*	openButton;
GtkToolItem*	saveButton;
GtkToolItem*	saveasButton;
GtkToolItem*	closeButton;
GtkToolItem*	redoButton;
GtkToolItem*	undoButton;

GtkWidget*		findReplaceDialog;
GtkWidget*		findBox;
GtkWidget*		replaceBox;

GtkWidget*		docView;
WebKitWebView*	webView;

pageStruct* getPageStructPtr(int pagenum)
{
	int			thispage;
	GtkWidget*	pageBox;

	if(pagenum==-1)
		thispage=gtk_notebook_get_current_page(notebook);
	else
		thispage=pagenum;

	pageBox=gtk_notebook_get_nth_page(notebook,thispage);
	if(pageBox==NULL)
		return(NULL);
	else
		return((pageStruct*)g_object_get_data(G_OBJECT(pageBox),"pagedata"));
}

void getMimeType(char* filepath,void* ptr)
{
	char*	command;
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint	retval=0;

	asprintf(&command,"file -b --mime-type %s",filepath);
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			asprintf((char**)ptr,"%s",stdout);
			g_free(stdout);
			g_free(stderr);
		}
}

void setLanguage(pageStruct* page)
{
	GtkSourceLanguage*			lang=NULL;
	GtkSourceLanguageManager*	lm=NULL;
	char*						mimetype;
	gboolean					result_uncertain;

	lm=gtk_source_language_manager_new();

	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(page->buffer),"languages-manager",lm,(GDestroyNotify)g_object_unref);
	lm=gtk_source_language_manager_get_default();
	mimetype=g_content_type_guess(page->filePath,NULL,0,&result_uncertain);
	if (result_uncertain)
		{
			g_free(mimetype);
			mimetype=NULL;
		}

	lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);

	if (lang==NULL)
		{
			getMimeType((char*)page->filePath,&mimetype);
			lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			if (lang!=NULL)
				gtk_source_buffer_set_language(page->buffer,lang);
		}
	else
		{
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			gtk_source_buffer_set_language(page->buffer,lang);
		}

}

void runCommand(char* commandtorun,void* ptr,bool interm,int flags)
{
	char*	command;
	gchar*	stdout=NULL;
	gchar*	stderr=NULL;
	gint	retval=0;

	if(interm==true)
		{
			asprintf(&command,"%s %s",terminalCommand,commandtorun);
			flags=8;
		}
	else
		asprintf(&command,"%s",commandtorun);

	if(flags==8)
		{
			g_spawn_command_line_async(command,NULL);
		}
	else
		{
			g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
			if (retval==0)
				{
					if(stdout!=NULL)
						{
							stdout[strlen(stdout)-1]=0;
							if(ptr!=NULL)
								asprintf((char**)ptr,"%s",stdout);
							g_free(stdout);
						}
					if(stderr!=NULL)
						g_free(stderr);
				}
		}
	g_free(command);
}

functionData* getFunctionByName(char* name)
{
	pageStruct*	page;
	int			numpages=gtk_notebook_get_n_pages(notebook);
	char*		lineptr;
	char*		functions=NULL;
	char*		dirname;
	gchar*		stdout=NULL;
	char		function[1024];

	functionData* fdata;
	page=getPageStructPtr(-1);
	if(page==NULL)
		return(NULL);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if(page->filePath!=NULL)
				{
					getRecursiveTagList(page->filePath,&functions);
					lineptr=functions;

					while (lineptr!=NULL)
						{
							sscanf (lineptr,"%s",function);
							if((strncasecmp(name,function,strlen(name))==0))
								{
									fdata=(functionData*)malloc(sizeof(functionData));
									sscanf (lineptr,"%"VALIDFUNCTIONCHARS"s",function);
									asprintf(&fdata->name,"%s",function);
									sscanf (lineptr,"%*s %"VALIDFUNCTIONCHARS"s",function);
									asprintf(&fdata->type,"%s",function);
									sscanf (lineptr,"%*s %*s %i",&fdata->line);
									sscanf (lineptr,"%*s %*s %*i %"VALIDFILENAMECHARS"s",function);
									asprintf(&fdata->file,"%s",function);
									sscanf (lineptr,"%*s %*s %*i %*s %"VALIDCHARS"s",function);
									asprintf(&fdata->define,"%s",function);
									fdata->intab=loop;
									return(fdata);
								}

							lineptr=strchr(lineptr,'\n');
							if (lineptr!=NULL)
								lineptr++;
						}
				}
		}

//not in any open files
//check ./ from all files

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if(page->filePath!=NULL)
				{
					asprintf(&dirname,"%s",g_path_get_dirname(page->filePath));
					getRecursiveTagList(dirname,&stdout);

					lineptr=stdout;
					while (lineptr!=NULL)
						{
							sscanf (lineptr,"%s",function);
							if((strncasecmp(name,function,strlen(name))==0))
								{
									fdata=(functionData*)malloc(sizeof(functionData));
									sscanf (lineptr,"%"VALIDFUNCTIONCHARS"s",function);
									asprintf(&fdata->name,"%s",function);
									sscanf (lineptr,"%*s %"VALIDFUNCTIONCHARS"s",function);
									asprintf(&fdata->type,"%s",function);
									sscanf (lineptr,"%*s %*s %i",&fdata->line);
									sscanf (lineptr,"%*s %*s %*i %"VALIDFILENAMECHARS"s",function);
									asprintf(&fdata->file,"%s",function);
									sscanf (lineptr,"%*s %*s %*i %*s %"VALIDCHARS"s",function);
									asprintf(&fdata->define,"%s",function);
									fdata->intab=-1;
									return(fdata);
								}

							lineptr=strchr(lineptr,'\n');
							if (lineptr!=NULL)
								lineptr++;
						}
					if(stdout!=NULL)
						g_free(stdout);
					if(dirname!=NULL)
						g_free(dirname);
				}
		}

	return(NULL);
}

void destroyData(functionData* fdata)
{
	if(fdata!=NULL)
		{
			g_free(fdata->name);
			g_free(fdata->type);
			g_free(fdata->file);
			g_free(fdata->define);
			g_free(fdata);
		}
}

void getRecursiveTagList(char* filepath,void* ptr)
{
	FILE*		fp;
	char		line[1024];
	GString*	str=g_string_new(NULL);
	char*		command;

	if(filepath==NULL)
		return;

	asprintf(&command,"find %s -maxdepth %i|ctags -L - -x",filepath,depth);

	fp=popen(command, "r");
	while(fgets(line,1024,fp))
		{
			g_string_append_printf(str,"%s",line);
		}
	pclose(fp);
	asprintf((char**)ptr,"%s",str->str);
	g_string_free(str,true);
	g_free(command);
}

