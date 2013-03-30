/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 * files.cpp
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
#include "callbacks.h"

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

void setFilePrefs(GtkWidget* sourceview)
{
	PangoFontDescription*		font_desc;

	if(indent==true)
		gtk_source_view_set_auto_indent((GtkSourceView*)sourceview,true);

	if(lineNumbers==true)
		gtk_source_view_set_show_line_numbers((GtkSourceView*)sourceview,true);

	if(lineWrap==true)
		gtk_text_view_set_wrap_mode((GtkTextView *)sourceview,GTK_WRAP_WORD);

	font_desc=pango_font_description_from_string(fontAndSize);
	gtk_widget_modify_font(sourceview,font_desc);
	pango_font_description_free(font_desc);


}

bool openFile(const gchar *filepath)
{
	GtkSourceLanguage*			lang=NULL;
	GtkSourceLanguageManager*	lm=NULL;
	GError*							err=NULL;
	GtkTextIter						iter;

	gchar*							buffer;
	long								filelen;
//	PangoFontDescription*		font_desc;

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

	setFilePrefs(sourceview);
//	gtk_text_view_set_wrap_mode((GtkTextView *)sourceview,GTK_WRAP_WORD);
//	gtk_source_view_set_show_line_numbers((GtkSourceView*)sourceview,true);
//	gtk_source_view_set_auto_indent((GtkSourceView*)sourceview,true);

//	font_desc=pango_font_description_from_string("mono 12");
//	gtk_widget_modify_font(sourceview, font_desc);
//	pango_font_description_free(font_desc);

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
