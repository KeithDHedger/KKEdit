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

int show_question(void)
{
	GtkWidget	*dialog;
	gint			result;

	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,"Save file before closing?");

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_SAVE,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Warning unsaved data!");

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	return(result);
}

void closeTab(GtkWidget* widget,gpointer data)
{
	int	thispage;
	int	result;

	if(data==NULL)
		thispage=gtk_notebook_get_current_page(notebook);
	else
		thispage=gtk_notebook_page_num(notebook,(GtkWidget *)data);

	pageStruct* page=(pageStruct*)g_list_nth_data(pageList,thispage);

	if(gtk_text_buffer_get_modified((GtkTextBuffer*)page->buffer))
		{
			result=show_question();
			switch(result)
				{
					case GTK_RESPONSE_YES:
						saveFile(NULL,NULL);
						break;
					case GTK_RESPONSE_NO:
						break;
					default:
						return;
						break;
				}
		}
		
	gtk_notebook_remove_page(notebook,thispage);
	g_free(page->filePath);
	pageList=g_list_remove(pageList,(gconstpointer)page);
	g_free(page);
	currentPage--;
}

