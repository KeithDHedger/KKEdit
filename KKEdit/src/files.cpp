/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
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
char*		saveFileName=NULL;
char*		saveFilePath=NULL;

GtkWidget* makeNewTab(char* name,char* tooltip,pageStruct* page)
{
	GtkWidget*	evbox=gtk_event_box_new();
	GtkWidget*	hbox=gtk_hbox_new(false,0);
	GtkWidget*	label=gtk_label_new(name);
	GtkWidget*	close=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	GtkWidget*	button=gtk_button_new();
	GtkRcStyle*	style=gtk_rc_style_new();


//	GtkWidget*		menuitem;
//	GtkWidget*		menu;
//	GtkWidget*		menutab;
//
//	//menutab=gtk_menu_item_new_with_label("File");
//	menu=gtk_menu_new();
//	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menutab),menu);
//	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//gtk_box_pack_start(GTK_BOX(hbox),menutab,false,false,0);


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
	PangoFontDescription*	font_desc;

	gtk_source_view_set_auto_indent(sourceview,indent);
	gtk_source_view_set_show_line_numbers(sourceview,lineNumbers);
	gtk_source_view_set_highlight_current_line(sourceview,highLight);

	if(lineWrap==true)
		gtk_text_view_set_wrap_mode((GtkTextView *)sourceview,GTK_WRAP_WORD_CHAR);
	else
		gtk_text_view_set_wrap_mode((GtkTextView *)sourceview,GTK_WRAP_NONE);

	gtk_source_view_set_tab_width(sourceview,tabWidth);

	font_desc=pango_font_description_from_string(fontAndSize);
	gtk_widget_modify_font((GtkWidget*)sourceview,font_desc);
	pango_font_description_free(font_desc);
}

void resetAllFilePrefs(void)
{
	pageStruct*	page;

	for(int loop=0;loop<gtk_notebook_get_n_pages(notebook);loop++)
		{
			page=getPageStructPtr(loop);
			setFilePrefs(page->view);
		}
}

bool openFile(const gchar *filepath,int linenumber)
{
	GtkTextIter		iter;
	gchar*			buffer;
	long			filelen;
	GtkWidget*		label;
	gchar*			filename=g_path_get_basename(filepath);
	pageStruct*		page=(pageStruct*)malloc(sizeof(pageStruct));
	GtkTextMark*	scroll2mark=gtk_text_mark_new(NULL,true);

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	asprintf(&page->filePath,"%s",filepath);
	asprintf(&page->fileName,"%s",filename);
	
	page->buffer=gtk_source_buffer_new(NULL);
	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	setFilePrefs(page->view);

	gtk_container_add(GTK_CONTAINER(page->pageWindow),GTK_WIDGET(page->view));

	vbox=gtk_vbox_new(true,4);
	label=makeNewTab(page->fileName,page->filePath,page);

	g_object_set_data(G_OBJECT(vbox),"pagedata",(gpointer)page);

	gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(page->pageWindow));

	gtk_notebook_append_page(notebook,vbox,label);
	gtk_notebook_set_tab_reorderable(notebook,vbox,true);

	setLanguage(page);

	g_file_get_contents(filepath,&buffer,(gsize*)&filelen,NULL);

	gtk_source_buffer_begin_not_undoable_action(page->buffer);
		gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &iter);
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,buffer,filelen);
		g_free (buffer);
	gtk_source_buffer_end_not_undoable_action(page->buffer);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),FALSE);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive),NULL);

 
	gtk_widget_show_all((GtkWidget*)window);
	gtk_notebook_set_current_page(notebook,currentPage);
	currentPage++;
	page->rebuildMenu=true;
	page->isFirst=true;
	page->itsMe=false;
	page->gFile=g_file_new_for_path(page->filePath);
	page->monitor=g_file_monitor(page->gFile,(GFileMonitorFlags)G_FILE_MONITOR_NONE,NULL,NULL);
	g_signal_connect(G_OBJECT(page->monitor),"changed",G_CALLBACK(fileChangedOnDisk),(void*)page);

	gtk_widget_grab_focus((GtkWidget*)page->view);
	
   /* move cursor to the linenumber */
	gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,linenumber,0);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
	gtk_text_iter_set_line(&iter,linenumber);
	gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(page->buffer),scroll2mark,&iter);  
	gtk_text_view_scroll_to_mark((GtkTextView*)page->view,scroll2mark,0,true,0,0.5);
	gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(page->buffer),scroll2mark);

	g_free(filename);

	return TRUE;
}

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
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),"Untitled");

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
		{
			saveFilePath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			saveFileName=g_path_get_basename(saveFilePath);
		}

	gtk_widget_destroy(dialog);
	gtk_widget_show_all(window);
}

bool saveFile(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start,end;
	gchar*		text;
	FILE*		fd=NULL;

	page->itsMe=true;
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
	text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer, &start, &end, FALSE);

	gtk_text_buffer_set_modified ((GtkTextBuffer*)page->buffer,FALSE);
	if(page->filePath!=NULL && data==NULL)
		{
			fd=fopen(page->filePath,"w");
			if (fd!=NULL)
				{
					fprintf(fd,"%s",text);
					fclose(fd);
				}
		}
	else
		{
			if(data!=NULL)
				{
					saveFilePath=page->filePath;
					saveFileName=page->fileName;
				}
			getSaveFile();
			page->filePath=saveFilePath;
			page->fileName=saveFileName;

			gtk_widget_set_tooltip_text(page->tabName,page->filePath);
			gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)saveFileName);

			fd=fopen(page->filePath,"w");
			if (fd!=NULL)
				{
					fprintf(fd,"%s",text);
					fclose(fd);
				}

			saveFileName=NULL;
			saveFilePath=NULL;
		}
	setLanguage(page);
	switchPage(notebook,NULL,currentTabNumber,NULL);
	//page->itsMe=false;
	return(true);
}

void newFile(GtkWidget* widget,gpointer data)
{
	GtkTextIter	iter;
	GtkWidget*	label;
	pageStruct*	page=(pageStruct*)malloc(sizeof(pageStruct));

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	page->filePath=NULL;
	page->buffer=gtk_source_buffer_new(NULL);
	asprintf(&page->fileName,"Untitled");
	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	setFilePrefs(page->view);
	gtk_container_add(GTK_CONTAINER(page->pageWindow),GTK_WIDGET(page->view));

	vbox=gtk_vbox_new(true,4);
	label=makeNewTab(page->fileName,NULL,page);

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
	page->isFirst=true;

	gtk_widget_grab_focus((GtkWidget*)page->view);
}

