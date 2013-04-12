/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
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
GtkWidget*		redoMenu;
GtkWidget*		undoMenu;
GtkWidget*		saveMenu;

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

void runCommand(char* commandtorun,void* ptr,bool interm)
{
	char*	command;
	gchar*	stdout=NULL;
	gchar*	stderr=NULL;
	gint	retval=0;

	if(interm==true)
		asprintf(&command,"%s %s",terminalCommand,commandtorun);
	else
		asprintf(&command,"%s",commandtorun);

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
			g_free(command);
		}
}


