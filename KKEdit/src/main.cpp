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

void closeTab(GtkWidget* widget,gpointer data)
{
	printf("close tab\n");
	printf("XXX%i\n",GPOINTER_TO_INT(data));
}

GtkWidget* makeNewTab(char* name,char* tooltip)
{
	GtkWidget*	evbox=gtk_event_box_new();
	GtkWidget*	hbox=gtk_hbox_new(false,0);
	GtkWidget*	label=gtk_label_new(name);
	GtkWidget*	close=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	GtkWidget*	button=gtk_button_new();

	gtk_button_set_relief ((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(evbox,tooltip);
	gtk_box_pack_start(GTK_BOX(hbox),label,true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	gtk_container_add(GTK_CONTAINER (button),close);
	gtk_container_add(GTK_CONTAINER(evbox),hbox);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",G_CALLBACK(closeTab),(void*)currentBuffer);
	gtk_widget_show_all(evbox);
	return(evbox);
}

bool openFile(const gchar *filepath)
{
	GtkSourceLanguage*			lang=NULL;
	GtkSourceLanguageManager*	lm=NULL;
	GError*							err=NULL;
	GtkTextIter						iter;

	gchar*							buffer;
	long								filelen;
	PangoFontDescription*		font_desc;

	GtkWidget*						scrolled_win;
	GtkWidget*						sourceview;
	GtkWidget*						label;
	gchar*							filename=g_path_get_basename(filepath);
	GtkWidget*						vbox;
	char*								mimetype;
	gboolean							result_uncertain;

	scrolled_win=gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win),GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	lm=gtk_source_language_manager_new();
   
	buffers[currentBuffer]=GTK_SOURCE_BUFFER(gtk_source_buffer_new(NULL));
	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(buffers[currentBuffer]),"languages-manager",lm,(GDestroyNotify)g_object_unref);

	sourceview=gtk_source_view_new_with_buffer(buffers[currentBuffer]);
	gtk_text_view_set_wrap_mode((GtkTextView *)sourceview,GTK_WRAP_WORD);

	font_desc=pango_font_description_from_string("mono 12");
	gtk_widget_modify_font(sourceview, font_desc);
	pango_font_description_free(font_desc);

	gtk_container_add(GTK_CONTAINER(scrolled_win),GTK_WIDGET(sourceview));

	//label=gtk_label_new(filename);
	label=makeNewTab((char*)filename,(char*)filepath);
	vbox=gtk_vbox_new(true,4);
	gtk_notebook_append_page(notebook,vbox,label);
	gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(scrolled_win));

	lm=gtk_source_language_manager_get_default();
	mimetype=g_content_type_guess (filepath,NULL,0,&result_uncertain);
	if (result_uncertain)
		{
			g_free (mimetype);
			mimetype=NULL;
		}

	lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);

	if (lang==NULL)
		{
			getMimeType((char*)filepath,&mimetype);
			lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			if (lang!=NULL)
				gtk_source_buffer_set_language(buffers[currentBuffer],lang);
		}
	else
		{
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			gtk_source_buffer_set_language(buffers[currentBuffer],lang);
		}

	g_file_get_contents(filepath,&buffer,(gsize*)&filelen,NULL);

	gtk_source_buffer_begin_not_undoable_action(buffers[currentBuffer]);
		gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (buffers[currentBuffer]), &iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER(buffers[currentBuffer]),&iter,buffer,filelen);
		g_free (buffer);
	gtk_source_buffer_end_not_undoable_action (buffers[currentBuffer]);
	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffers[currentBuffer]),FALSE);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (buffers[currentBuffer]),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER (buffers[currentBuffer]),&iter);

	g_object_set_data_full (G_OBJECT(buffers[currentBuffer]),"filename",g_strdup(filepath),(GDestroyNotify) g_free);

	return TRUE;
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
			currentBuffer++;
			g_free (filename);
	}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
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
	gtk_widget_set_size_request(window,800,600);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	notebook=(GtkNotebook*)gtk_notebook_new();
	vbox=gtk_vbox_new(FALSE,0);
	menubar=gtk_menu_bar_new();

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)menubar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)notebook,true,true,0);

	menunav=gtk_menu_item_new_with_label("Navigation");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menunav),menu);
  
//file menu
	menufile=gtk_menu_item_new_with_label("File");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufile),menu);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doOpenFile),NULL);
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
			openFile(argv[j]);
			//printf("\n%s\n",argv[j]);
			currentBuffer++;
		}

	gtk_widget_show_all(window);

	gtk_main();
}
