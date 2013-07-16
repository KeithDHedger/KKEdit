/******************************************************
*
*     ©keithhedger Tue 16 Jul 20:46:19 BST 2013
*     kdhedger68713@gmail.com
*
*     spellcheck.cpp
* 
******************************************************/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

#include "config.h"
#include "globals.h"

void checkWord(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char*		command;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	printf("word=%s\n",selection);
	asprintf(&command,"echo -n \"%s\"|ispell -a|grep \":\"",selection);
	system(command);
	g_free(selection);

}