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

GtkWidget* makeNewTab(char* name,char* tooltip,pageStruct* page)
{
	GtkWidget*	evbox=gtk_event_box_new();
	GtkWidget*	hbox=gtk_hbox_new(false,0);
	GtkWidget*	label=gtk_label_new(name);
	GtkWidget*	close=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	GtkWidget*	button=gtk_button_new();
	GtkRcStyle*	style=gtk_rc_style_new();

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(label,tooltip);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

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
//	GtkSourceLanguage*			lang=NULL;
//	GtkSourceLanguageManager*	lm=NULL;
	GtkTextIter						iter;
	gchar*							buffer;
	long								filelen;
	GtkWidget*						label;
	gchar*							filename=g_path_get_basename(filepath);
//	char*								mimetype;
//	gboolean							result_uncertain;
	pageStruct*						page=(pageStruct*)malloc(sizeof(pageStruct));

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
//	lm=gtk_source_language_manager_new();

	asprintf(&page->filePath,"%s",filepath);
	page->buffer=gtk_source_buffer_new(NULL);

//	g_object_ref(lm);
//	g_object_set_data_full(G_OBJECT(page->buffer),"languages-manager",lm,(GDestroyNotify)g_object_unref);

	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	setFilePrefs(page->view);

	gtk_container_add(GTK_CONTAINER(page->pageWindow),GTK_WIDGET(page->view));

	vbox=gtk_vbox_new(true,4);
	label=makeNewTab((char*)filename,(char*)filepath,page);

	g_object_set_data(G_OBJECT(vbox),"pagedata",(gpointer)page);

	gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(page->pageWindow));

	gtk_notebook_append_page(notebook,vbox,label);
	gtk_notebook_set_tab_reorderable(notebook,vbox,true);

//	lm=gtk_source_language_manager_get_default();
//	mimetype=g_content_type_guess (filepath,NULL,0,&result_uncertain);
//	if (result_uncertain)
//		{
///			g_free(mimetype);
//			mimetype=NULL;
//		}

//	lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);

//	if (lang==NULL)
//		{
//			getMimeType((char*)filepath,&mimetype);
//			lang=gtk_source_language_manager_guess_language(lm,filepath,mimetype);
//			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
//			if (lang!=NULL)
//				gtk_source_buffer_set_language(page->buffer,lang);
//		}
//	else
//		{
//			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
//			gtk_source_buffer_set_language(page->buffer,lang);
//		}

	setLanguage(page);

	g_file_get_contents(filepath,&buffer,(gsize*)&filelen,NULL);

	gtk_source_buffer_begin_not_undoable_action(page->buffer);
		gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &iter);
		gtk_text_buffer_insert (GTK_TEXT_BUFFER(page->buffer),&iter,buffer,filelen);
		g_free (buffer);
	gtk_source_buffer_end_not_undoable_action (page->buffer);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),FALSE);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive), NULL);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);

	gtk_widget_show_all((GtkWidget*)window);
	gtk_notebook_set_current_page(notebook,currentPage);
	currentPage++;
	page->rebuildMenu=true;
	gtk_widget_grab_focus((GtkWidget*)page->view);

	return TRUE;
}

char*	saveFileName=NULL;
char*	saveFilePath=NULL;

void getSaveFile(void)
{
	GtkWidget*	dialog;

	dialog=gtk_file_chooser_dialog_new("Save File",(GtkWindow*)window, GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);

	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER (dialog),TRUE);
	if(saveFileName!=NULL)
		{
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),g_path_get_dirname(saveFilePath));
			gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),saveFileName);
		}
 	else
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER (dialog),"Untitled");

	if(gtk_dialog_run (GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			saveFilePath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			saveFileName=g_path_get_basename(saveFilePath);
		}

	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}

bool saveFile(GtkWidget* widget,gpointer data)
{
	pageStruct* page=getPageStructPtr(-1);
	GtkTextIter	start,end;
	gchar*		text;
	char*			newlabel;

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
	text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer, &start, &end, FALSE);

	gtk_text_buffer_set_modified ((GtkTextBuffer*)page->buffer,FALSE);
	if(page->filePath!=NULL && data==NULL)
		g_file_set_contents(page->filePath,text,-1,NULL);
	else
		{
			if(data!=NULL)
				{
					saveFilePath=page->filePath;
					saveFileName=(char*)gtk_label_get_text((GtkLabel*)page->tabName);
				}
			getSaveFile();
			page->filePath=saveFilePath;

			gtk_widget_set_tooltip_text(page->tabName,page->filePath);
			gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)saveFileName);
			g_file_set_contents(page->filePath,text,-1,NULL);

			g_free(newlabel);

			saveFileName=NULL;
			saveFilePath=NULL;
		}
	setLanguage(page);
	return(true);
}

void newFile(GtkWidget* widget,gpointer data)
{
	GtkTextIter						iter;
	GtkWidget*						label;
	const char*						filename="Untitled";
	pageStruct*						page=(pageStruct*)malloc(sizeof(pageStruct));

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	page->filePath=NULL;
	page->buffer=gtk_source_buffer_new(NULL);

	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	setFilePrefs(page->view);
	gtk_container_add(GTK_CONTAINER(page->pageWindow),GTK_WIDGET(page->view));

	vbox=gtk_vbox_new(true,4);
	label=makeNewTab((char*)filename,NULL,page);

	g_object_set_data(G_OBJECT(vbox),"pagedata",(gpointer)page);

	gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(page->pageWindow));

	gtk_notebook_append_page(notebook,vbox,label);
	gtk_notebook_set_tab_reorderable(notebook,vbox,true);

	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive), NULL);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);

	gtk_widget_show_all((GtkWidget*)window);
	gtk_notebook_set_current_page(notebook,currentPage);
	currentPage++;
	page->rebuildMenu=true;
	gtk_widget_grab_focus((GtkWidget*)page->view);
}

