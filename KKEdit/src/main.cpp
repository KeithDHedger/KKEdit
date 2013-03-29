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

/*
void setValue(const char* command,dataType type,void* ptr)
{
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint   retval=0;

	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			switch(type)
				{
					case INT:
						stdout[strlen(stdout)-1]=0;
						*(int*)ptr=atoi(stdout);
						break;

					case STRING:
						stdout[strlen(stdout)-1]=0;
						asprintf((char**)ptr,"%s",stdout);
						break;

					case FLOAT:
						stdout[strlen(stdout)-1]=0;
						*(double*)ptr=atof(stdout);
						break;
				}
		}
	freeAndNull(&stdout);
	freeAndNull(&stderr);
}
*/

void getMimeType(char* filepath,void* ptr)
{
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint   retval=0;

	char*	command;
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



bool open_file(const gchar *filepath)
{
	GtkSourceLanguageManager*	lm;
	GtkSourceLanguage*			language=NULL;
	GError*							err=NULL;
	//gboolean			reading;
	GtkTextIter						iter;
//	GIOChannel *io;
	gchar*							buffer;
	long								filelen;
	PangoFontDescription*		font_desc;

	GtkWidget*						scrolled_win;
	GtkWidget*						sourceview;
	GtkWidget*						label;
	gchar*							filename=g_path_get_basename(filepath);
	GtkWidget*						vbox;

	char*								mime;

	scrolled_win=gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win),GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
	lm=gtk_source_language_manager_new();
   
	buffers[currentBuffer]=GTK_SOURCE_BUFFER(gtk_source_buffer_new(NULL));
	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(buffers[currentBuffer]),"languages-manager",lm,(GDestroyNotify)g_object_unref);

	sourceview=gtk_source_view_new_with_buffer(buffers[currentBuffer]);
    
	font_desc=pango_font_description_from_string("mono 12");
	gtk_widget_modify_font(sourceview, font_desc);
	pango_font_description_free(font_desc);

	gtk_container_add(GTK_CONTAINER(scrolled_win),GTK_WIDGET(sourceview));
	label=gtk_label_new(filename);
	vbox=gtk_vbox_new(true,4);;

	gtk_notebook_append_page(notebook,vbox,label);

	gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(scrolled_win));

//	g_return_val_if_fail(filepath!=NULL,FALSE);

    /* get the Language for C source mimetype */
//	lm=(GtkSourceLanguageManager*)g_object_get_data(G_OBJECT(buffers[currentBuffer]),"languages-manager");

	//getMimeType((char*)filepath,&mime);

 //   language = gtk_source_languages_manager_get_language_from_mime_type (lm,mime);
	//language=gtk_source_language_manager_get_language(lm,"c");
//language=gtk_source_language_manager_guess_language (filepath, NULL);
//	g_print("Language: [%s]\n",gtk_source_language_get_name(language));
GtkSourceLanguage *lang = NULL;
gboolean result_uncertain;
gchar *content_type;

lm=gtk_source_language_manager_get_default();
content_type = g_content_type_guess (filepath, NULL, 0, &result_uncertain);
//if (result_uncertain)
//  {
//    g_free (content_type);
//    content_type = NULL;
//  }

lang = gtk_source_language_manager_guess_language (lm, filepath, content_type);
//gtk_source_buffer_set_language (buffer, lang);
g_free (content_type);

	if (lang==NULL)
		{
			g_print("No language found for mime type `%s'\n","text/x-csrc");
			getMimeType((char*)filepath,&mime);
			content_type=g_content_type_from_mime_type((const gchar *)mime);
			lang = gtk_source_language_manager_guess_language (lm, filepath, content_type);
			g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			gtk_source_buffer_set_language(buffers[currentBuffer],lang);
		}
	else
		{
			g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			gtk_source_buffer_set_language(buffers[currentBuffer],lang);
		}

//    /* Now load the file from Disk */
//	io = g_io_channel_new_file (filepath, "r",&err);
//	if (!io)
//		{
//			g_print("error: %s %s\n",(err)->message,filepath);
//			return FALSE;
//		}
//
//	if (g_io_channel_set_encoding (io, "utf-8", &err) != G_IO_STATUS_NORMAL)
//		{
//			g_print("err: Failed to set encoding:\n%s\n%s", filepath, (err)->message);
//			return FALSE;
//		}
//

g_file_get_contents(filepath,&buffer,(gsize*)&filelen,NULL);
	gtk_source_buffer_begin_not_undoable_action(buffers[currentBuffer]);

//	buffer = (gchar*)g_malloc(4096);
//	reading = TRUE;
//	while (reading)
//		{
//			gsize bytes_read;
//			GIOStatus status;
//
//			status = g_io_channel_read_chars (io, buffer, 4096, &bytes_read, &err);
//			switch (status)
//				{
//					case G_IO_STATUS_EOF:
//						reading = FALSE;
//
//					case G_IO_STATUS_NORMAL:
//						if (bytes_read == 0)
//							continue;		
						gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (buffers[currentBuffer]), &iter);
						gtk_text_buffer_insert (GTK_TEXT_BUFFER(buffers[currentBuffer]),&iter,buffer,filelen);
//						break;
//
//					case G_IO_STATUS_AGAIN:
//						continue;
//
//					case G_IO_STATUS_ERROR:
//
//					default:
//						g_print("err (%s): %s", filepath, (err)->message);
//               /* because of error in input we clear already loaded text */
//						gtk_text_buffer_set_text (GTK_TEXT_BUFFER (buffers[currentBuffer]), "", 0);
//
//						reading = FALSE;
//						break;
//			}
//		}
//	g_free (buffer);

	gtk_source_buffer_end_not_undoable_action (buffers[currentBuffer]);
//	g_io_channel_unref (io);
//
//	if (err)
//		{
//			g_error_free (err);
//			return FALSE;
//		}

	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffers[currentBuffer]), FALSE);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (buffers[currentBuffer]), &iter);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (buffers[currentBuffer]), &iter);

	g_object_set_data_full (G_OBJECT (buffers[currentBuffer]),"filename", g_strdup (filepath), (GDestroyNotify) g_free);

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

//GBookmarkFile *bm=     g_bookmark_file_new                 ();
//gchar* xx=g_bookmark_file_get_mime_type(bm,"/home/keithhedger/Scripts/MountOSX",NULL);
//printf("\n%s\n",xx);
//return 0;



	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,400,400);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	notebook=(GtkNotebook*)gtk_notebook_new();
	vbox=gtk_vbox_new(FALSE,0);
	menubar=gtk_menu_bar_new();

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)menubar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)notebook,true,true,0);

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

	currentBuffer=0;

	gtk_container_add(GTK_CONTAINER(window),(GtkWidget*)vbox);

	for (int j=1;j<argc;j++)
		{
			open_file(argv[j]);
			printf("\n%s\n",argv[j]);
			currentBuffer++;
		}

	gtk_widget_show_all(window);

	gtk_main();
}
