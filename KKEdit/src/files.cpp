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

GtkWidget* makeNewTab(char* name,char* tooltip,pageStruct* page)
{
	GtkWidget*	evbox=gtk_event_box_new();
	GtkWidget*	hbox=gtk_hbox_new(false,0);
	GtkWidget*	label=gtk_label_new(name);
	GtkWidget*	close=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	GtkWidget*	button=gtk_button_new();
	GtkRcStyle*	style=gtk_rc_style_new();

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(evbox,tooltip);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

//	gtk_widget_set_size_request(button,20,20);
	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
	gtk_container_add(GTK_CONTAINER(button),close);

	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	gtk_container_add(GTK_CONTAINER(evbox),hbox);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",G_CALLBACK(closeTab),(void*)vbox);
	page->tabName=label;

	style->xthickness=style->ythickness=0;
	gtk_widget_modify_style (button, style);
	g_object_unref(G_OBJECT(style));
  
	gtk_widget_show_all(evbox);
	return(evbox);
}

void setFilePrefs(GtkSourceView* sourceview)
{
	PangoFontDescription*		font_desc;

	if(indent==true)
		gtk_source_view_set_auto_indent(sourceview,true);

	if(lineNumbers==true)
		gtk_source_view_set_show_line_numbers(sourceview,true);

	if(lineWrap==true)
		gtk_text_view_set_wrap_mode((GtkTextView *)sourceview,GTK_WRAP_WORD);

	font_desc=pango_font_description_from_string(fontAndSize);
	gtk_widget_modify_font((GtkWidget*)sourceview,font_desc);
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

	GtkWidget*						label;
	gchar*							filename=g_path_get_basename(filepath);
	char*								mimetype;
	gboolean							result_uncertain;

	pageStruct*						page=(pageStruct*)malloc(sizeof(pageStruct));

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	lm=gtk_source_language_manager_new();

	asprintf(&page->filePath,"%s",filepath);
	page->buffer=gtk_source_buffer_new(NULL);
	pageList=g_list_insert(pageList,(gpointer)page,currentPage);

	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(page->buffer),"languages-manager",lm,(GDestroyNotify)g_object_unref);

	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	setFilePrefs(page->view);

	gtk_container_add(GTK_CONTAINER(page->pageWindow),GTK_WIDGET(page->view));

	vbox=gtk_vbox_new(true,4);
	label=makeNewTab((char*)filename,(char*)filepath,page);
	gtk_notebook_append_page(notebook,vbox,label);


//	gtk_notebook_set_tab_reorderable(notebook,vbox,true);
g_object_set_data(G_OBJECT(vbox),"p1 data",(gpointer)currentPage);

	gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(page->pageWindow));

	lm=gtk_source_language_manager_get_default();
	mimetype=g_content_type_guess (filepath,NULL,0,&result_uncertain);
	if (result_uncertain)
		{
			g_free(mimetype);
			mimetype=NULL;
		}

	lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);

	if (lang==NULL)
		{
			getMimeType((char*)filepath,&mimetype);
			lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			if (lang!=NULL)
				gtk_source_buffer_set_language(page->buffer,lang);
		}
	else
		{
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			gtk_source_buffer_set_language(page->buffer,lang);
		}

	g_file_get_contents(filepath,&buffer,(gsize*)&filelen,NULL);

	gtk_source_buffer_begin_not_undoable_action(page->buffer);
		gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER(page->buffer),&iter,buffer,filelen);
		g_free (buffer);
	gtk_source_buffer_end_not_undoable_action (page->buffer);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),FALSE);
	//gtk_signal_connect(GTK_OBJECT(GTK_TEXT_BUFFER(page->buffer)),"modified-changed",G_CALLBACK(setSensitive),NULL);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive), NULL);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);

	g_object_set_data_full (G_OBJECT(page->buffer),"filename",g_strdup(filepath),(GDestroyNotify) g_free);
	gtk_widget_show_all((GtkWidget*)window);
	gtk_notebook_set_current_page(notebook,currentPage);
	currentPage++;
	page->rebuildMenu=true;
	gtk_widget_grab_focus((GtkWidget*)page->view);
	return TRUE;
}

bool saveFile(GtkWidget* widget,gpointer data)
{
	int			thispage=gtk_notebook_get_current_page(notebook);
	pageStruct* page=(pageStruct*)g_list_nth_data(pageList,thispage);
	GtkTextIter	start,end;
	gchar*		text;
	
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_get_end_iter ((GtkTextBuffer*)page->buffer,&end);
	text=gtk_text_buffer_get_text ((GtkTextBuffer*)page->buffer, &start, &end, FALSE);

	gtk_text_buffer_set_modified ((GtkTextBuffer*)page->buffer,FALSE);
	g_file_set_contents(page->filePath,text,-1,NULL);

	return(true);
}

void newFile(GtkWidget* widget,gpointer data)
{
	GtkSourceLanguage*			lang=NULL;
	GtkSourceLanguageManager*	lm=NULL;
	GError*							err=NULL;
	GtkTextIter						iter;

	gchar*							buffer;
	long								filelen;

	GtkWidget*						label;
	const char*						filename="Untitled";
	const char*						mimetype="text/plain";


	pageStruct*						page=(pageStruct*)malloc(sizeof(pageStruct));

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	lm=gtk_source_language_manager_new();
	page->filePath=NULL;
//	asprintf(&page->filePath,"%s",filepath);
	page->buffer=gtk_source_buffer_new(NULL);
	pageList=g_list_insert(pageList,(gpointer)page,currentPage);

	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(page->buffer),"languages-manager",lm,(GDestroyNotify)g_object_unref);

	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	setFilePrefs(page->view);

	gtk_container_add(GTK_CONTAINER(page->pageWindow),GTK_WIDGET(page->view));

	vbox=gtk_vbox_new(true,4);
	label=makeNewTab((char*)filename,NULL,page);
	gtk_notebook_append_page(notebook,vbox,label);
	gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(page->pageWindow));

	//lm=gtk_source_language_manager_get_default();
	//mimetype=g_content_type_guess (filepath,NULL,0,&result_uncertain);
	//if (result_uncertain)
	//	{
	//		g_free(mimetype);
	//		mimetype=NULL;
	//	}

	//lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);

	//if (lang==NULL)
	//	{
	//		getMimeType((char*)filepath,&mimetype);
	//		lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
	//if (lang!=NULL)
	//	gtk_source_buffer_set_language(page->buffer,lang);
		//}
	//else
	//	{
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
	//		gtk_source_buffer_set_language(page->buffer,lang);
		//}

//	g_file_get_contents(filepath,&buffer,(gsize*)&filelen,NULL);

//	gtk_source_buffer_begin_not_undoable_action(page->buffer);
//		gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &iter);
//		gtk_text_buffer_insert (GTK_TEXT_BUFFER(page->buffer),&iter,buffer,filelen);
//		g_free (buffer);
//	gtk_source_buffer_end_not_undoable_action (page->buffer);
//	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),FALSE);
	//gtk_signal_connect(GTK_OBJECT(GTK_TEXT_BUFFER(page->buffer)),"modified-changed",G_CALLBACK(setSensitive),NULL);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive), NULL);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);

	//g_object_set_data_full (G_OBJECT(page->buffer),"filename",g_strdup(filepath),(GDestroyNotify) g_free);
	gtk_widget_show_all((GtkWidget*)window);
	gtk_notebook_set_current_page(notebook,currentPage);
	currentPage++;
	page->rebuildMenu=true;
	gtk_widget_grab_focus((GtkWidget*)page->view);
	//return TRUE;
}

