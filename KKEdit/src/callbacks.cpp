/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

void doOpenFile(GtkWidget* widget,gpointer data)
{
	GtkWidget *dialog;
	char *filename;

	dialog=gtk_file_chooser_dialog_new("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			openFile(filename);
			currentBuffer++;
			g_free (filename);
	}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}

