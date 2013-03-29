/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
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

#include "globals.h"

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

bool open_file(GtkSourceBuffer *sBuf, const gchar *filename)
{
	GtkSourceLanguageManager *lm;
	GtkSourceLanguage *language = NULL;
	GError *err = NULL;
	gboolean reading;
	GtkTextIter iter;
	GIOChannel *io;
	gchar *buffer;

	g_return_val_if_fail (sBuf != NULL, FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_SOURCE_BUFFER (sBuf), FALSE);

    /* get the Language for C source mimetype */
	lm = (GtkSourceLanguageManager*)g_object_get_data (G_OBJECT (sBuf), "languages-manager");
    
    // 改用下面 gtk_source_language_manager_get_language(GtkSourceLanguageManager*, const gchar*)這個method
    //language = gtk_source_languages_manager_get_language_from_mime_type (lm, "text/x-csrc");
	language = gtk_source_language_manager_get_language(lm, "c");
								
	g_print("Language: [%s]\n", gtk_source_language_get_name(language));

	if (language == NULL)
		{
			g_print ("No language found for mime type `%s'\n", "text/x-csrc");
        // 我用的版本好像沒這個attribute
        //g_object_set (G_OBJECT (sBuf), "highlight", FALSE, NULL);
		}
	else
		{
			gtk_source_buffer_set_language (sBuf, language);
        // 我用的版本好像沒這個attribute
        //g_object_set (G_OBJECT (sBuf), "highlight", TRUE, NULL);
		}

    /* Now load the file from Disk */
    io = g_io_channel_new_file (filename, "r", &err);
	if (!io)
		{
			g_print("error: %s %s\n", (err)->message, filename);
			return FALSE;
		}

	if (g_io_channel_set_encoding (io, "utf-8", &err) != G_IO_STATUS_NORMAL)
		{
			g_print("err: Failed to set encoding:\n%s\n%s", filename, (err)->message);
			return FALSE;
		}

	gtk_source_buffer_begin_not_undoable_action (sBuf);

    //gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sBuf), "", 0);
	buffer = (gchar*)g_malloc (4096);
	reading = TRUE;
	while (reading)
		{
			gsize bytes_read;
			GIOStatus status;

			status = g_io_channel_read_chars (io, buffer, 4096, &bytes_read, &err);
			switch (status)
				{
					case G_IO_STATUS_EOF:
						reading = FALSE;

					case G_IO_STATUS_NORMAL:
						if (bytes_read == 0)
							continue;		
						gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (sBuf), &iter);
						gtk_text_buffer_insert (GTK_TEXT_BUFFER(sBuf),&iter,buffer,bytes_read);
						break;

					case G_IO_STATUS_AGAIN:
						continue;

					case G_IO_STATUS_ERROR:

					default:
						g_print("err (%s): %s", filename, (err)->message);
                /* because of error in input we clear already loaded text */
						gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sBuf), "", 0);

						reading = FALSE;
						break;
			}
		}
	g_free (buffer);

	gtk_source_buffer_end_not_undoable_action (sBuf);
	g_io_channel_unref (io);

	if (err)
		{
			g_error_free (err);
			return FALSE;
		}

	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (sBuf), FALSE);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (sBuf), &iter);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (sBuf), &iter);

	g_object_set_data_full (G_OBJECT (sBuf),"filename", g_strdup (filename), (GDestroyNotify) g_free);

	return TRUE;
}

int main(int argc,char **argv)
{
	GtkWidget*		vbox;
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GtkWidget*		scrolled_win;
	GtkWidget*		sourceview;
	GtkSourceLanguageManager *lm;
	GtkSourceBuffer *buffer;
	PangoFontDescription *font_desc;
	
	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	notebook=(GtkNotebook*)gtk_notebook_new();
	vbox=gtk_vbox_new(FALSE,0);
	menubar=gtk_menu_bar_new();

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)menubar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)notebook,true,true,0);

	scrolled_win=gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win),GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
	lm=gtk_source_language_manager_new();
   
	buffer=GTK_SOURCE_BUFFER(gtk_source_buffer_new(NULL));
	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(buffer),"languages-manager",lm,(GDestroyNotify) g_object_unref);

	sourceview=gtk_source_view_new_with_buffer(buffer);
    
	font_desc=pango_font_description_from_string("mono 12");
	gtk_widget_modify_font(sourceview, font_desc);
	pango_font_description_free(font_desc);

	gtk_container_add(GTK_CONTAINER(scrolled_win),GTK_WIDGET(sourceview));
	gtk_container_add(GTK_CONTAINER(notebook),scrolled_win);


	menunav=gtk_menu_item_new_with_label("Navigation");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menunav),menu);

//accel_group = gtk_accel_group_new();
//  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  
//file menu
	menufile=gtk_menu_item_new_with_label("File");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufile),menu);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(shutdown),NULL);

//edit menu
	menuedit=gtk_menu_item_new_with_label("Edit");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuedit),menu);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_UNDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_REDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND_AND_REPLACE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//bookmarks
	menubookmark=gtk_menu_item_new_with_label("Bookmarks");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubookmark),menu);
	menuitem=gtk_menu_item_new_with_label("Add Bookmark");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menufile);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menuedit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menunav);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menubookmark);

//GtkStockItem* xx;
//gtk_stock_lookup(GTK_STOCK_NEW,xx);
//printf("\n%s\n",xx);

	gtk_container_add(GTK_CONTAINER(window),(GtkWidget*)vbox);
	
	gtk_widget_show_all(window);
	
	open_file(buffer, "/media/LinuxData/Development/Projects/KKEdit/KKEdit/src/main.cpp");
	
	gtk_main();
}
