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

GtkWidget*	vbox;

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
	gtk_signal_connect(GTK_OBJECT(button),"clicked",G_CALLBACK(closeTab),(void*)vbox);
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

	GtkWidget*						scrolled_win;
	GtkWidget*						sourceview;
	GtkWidget*						label;
	gchar*							filename=g_path_get_basename(filepath);
	char*								mimetype;
	gboolean							result_uncertain;

	scrolled_win=gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win),GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	lm=gtk_source_language_manager_new();

	buffers[currentPage]=GTK_SOURCE_BUFFER(gtk_source_buffer_new(NULL));
	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(buffers[currentPage]),"languages-manager",lm,(GDestroyNotify)g_object_unref);

	sourceview=gtk_source_view_new_with_buffer(buffers[currentPage]);

	setFilePrefs(sourceview);

	gtk_container_add(GTK_CONTAINER(scrolled_win),GTK_WIDGET(sourceview));

	vbox=gtk_vbox_new(true,4);
	label=makeNewTab((char*)filename,(char*)filepath);
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
				gtk_source_buffer_set_language(buffers[currentPage],lang);
		}
	else
		{
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			gtk_source_buffer_set_language(buffers[currentPage],lang);
		}

	g_file_get_contents(filepath,&buffer,(gsize*)&filelen,NULL);

	gtk_source_buffer_begin_not_undoable_action(buffers[currentPage]);
		gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (buffers[currentPage]), &iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER(buffers[currentPage]),&iter,buffer,filelen);
		g_free (buffer);
	gtk_source_buffer_end_not_undoable_action (buffers[currentPage]);
	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffers[currentPage]),FALSE);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (buffers[currentPage]),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER (buffers[currentPage]),&iter);

	g_object_set_data_full (G_OBJECT(buffers[currentPage]),"filename",g_strdup(filepath),(GDestroyNotify) g_free);
	currentPage++;

	return TRUE;
}

bool saveFile(GtkWidget* widget,gpointer data)
{
//	widg=gtk_notebook_get_tab_label          (GtkNotebook *notebook,GtkWidget *child);
	int thispage=gtk_notebook_get_current_page(notebook);
	printf("\n%i\n",thispage);
		
	GtkWidget*	page=gtk_notebook_get_nth_page(notebook,thispage);
	GtkWidget*	widg=gtk_notebook_get_tab_label(notebook,page);
	printf("%s\n",gtk_widget_get_tooltip_text(widg));
	
	GtkTextIter						start,end;
	gchar*	text;

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)buffers[thispage],&start);
	gtk_text_buffer_get_end_iter ((GtkTextBuffer*)buffers[thispage],&end);
	text=gtk_text_buffer_get_text ((GtkTextBuffer*)buffers[thispage], &start, &end, FALSE);       
	gtk_text_buffer_set_modified ((GtkTextBuffer*)buffers[thispage], FALSE);
//gtk_widget_set_sensitive (editor->text_view, TRUE);

/* set the contents of the file to the text from the buffer */
//if (filename != NULL)        
   g_file_set_contents (gtk_widget_get_tooltip_text(widg), text, -1,NULL);
}





