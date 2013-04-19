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

bool			indent;
bool			lineNumbers;
bool			lineWrap;
bool			highLight;
int				tabWidth;
char*			fontAndSize;
char*			terminalCommand;

int				windowWidth;
int				windowHeight;
int				windowX=-1;
int				windowY=-1;
bool			wrapSearch;
bool			insensitiveSearch;
bool			replaceAll;

int				currentTabNumber;

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

int getTabFromPath(char* filepath)
{
	pageStruct*	page;
	int			numpages=gtk_notebook_get_n_pages(notebook);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if(strstr(page->filePath,filepath)!=NULL)
				return(loop);
		}
	return(-1);
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

void getTagList(char* filepath,void* ptr)
{
	char*	command;
	gchar*	stdout=NULL;
	gchar*	stderr=NULL;
	gint	retval=0;

	asprintf(&command,"ctags -x %s",filepath);
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			asprintf((char**)ptr,"%s",stdout);
			g_free(stdout);
			g_free(stderr);
		}
}


functionData* getFunctionByName(char* name)
{
	pageStruct*	page=getPageStructPtr(-1);
	int			numpages=gtk_notebook_get_n_pages(notebook);
	char*		lineptr;
	char*		functions;
	int			line;
	char		file[4096];
	char*		command;
	gchar*		stdout=NULL;
	gchar*		stderr=NULL;
	gint		retval=0;
	char		function[1024];
	char*		functionDefine;

	functionData* fdata;

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			getTagList(page->filePath,&functions);
			
			lineptr=functions;

			while (lineptr!=NULL)
				{
					sscanf (lineptr,"%s",function);
					if((strncasecmp(name,function,strlen(name))==0))
						{
							fdata=(functionData*)malloc(sizeof(functionData));
							sscanf (lineptr,"%[A-Za-z0-9_-]s",function);
							asprintf(&fdata->name,"%s",function);
							sscanf (lineptr,"%*s %[A-Za-z0-9_-]s",function);
							asprintf(&fdata->type,"%s",function);
							sscanf (lineptr,"%*s %*s %i",&fdata->line);
							sscanf (lineptr,"%*s %*s %*i %[A-Za-z0-9_-./]s",function);
							asprintf(&fdata->file,"%s",function);
							sscanf (lineptr,"%*s %*s %*i %*s %"VALIDCHARS"s",function);
							asprintf(&fdata->define,"%s",function);
							return(fdata);
						}

					lineptr=strchr(lineptr,'\n');
					if (lineptr!=NULL)
						lineptr++;
				}
		}

//not in any open files
//check ./
	asprintf(&command,"ctags -xR %s",g_path_get_dirname(page->filePath));
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			g_free(stderr);
		}

	lineptr=stdout;
	while (lineptr!=NULL)
		{
			sscanf (lineptr,"%s",function);
			if((strncasecmp(name,function,strlen(name))==0))
				{
					fdata=(functionData*)malloc(sizeof(functionData));
					sscanf (lineptr,"%[A-Za-z0-9_-]s",function);
					asprintf(&fdata->name,"%s",function);
					sscanf (lineptr,"%*s %[A-Za-z0-9_-]s",function);
					asprintf(&fdata->type,"%s",function);
					sscanf (lineptr,"%*s %*s %i",&fdata->line);
					sscanf (lineptr,"%*s %*s %*i %[A-Za-z0-9_-./]s",function);
					asprintf(&fdata->file,"%s",function);
					sscanf (lineptr,"%*s %*s %*i %*s %"VALIDCHARS"s",function);
					asprintf(&fdata->define,"%s",function);
					g_free(stdout);
					return(fdata);
				}

			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;

		}

	return(NULL);
}





