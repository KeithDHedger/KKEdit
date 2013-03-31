/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include <gtk/gtk.h>

#include "globals.h"
#include "files.h"

void tabChanged(GtkWidget* widget,gpointer data)
{
	printf("XXXXXXXXXXX\n here");
}

void doOpenFile(GtkWidget* widget,gpointer data)
{
	GtkWidget *dialog;
	char *filename;

	dialog=gtk_file_chooser_dialog_new("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			openFile(filename);
			g_free (filename);
	}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}

void closeTab(GtkWidget* widget,gpointer data)
{
	int thispage;
	if(data==NULL)
		thispage=gtk_notebook_get_current_page(notebook);
	else
		thispage=gtk_notebook_page_num(notebook,(GtkWidget *)data);

	pageStruct* page=(pageStruct*)g_list_nth_data(pageList,thispage);

	gtk_notebook_remove_page(notebook,thispage);
	g_free(page->filePath);
	pageList=g_list_remove(pageList,(gconstpointer)page);
	g_free(page);
}

