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
#include <glib.h>

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "globals.h"
#include "callbacks.h"
#include "navcallbacks.h"

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

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(label,tooltip);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
	gtk_container_add(GTK_CONTAINER(button),close);

	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	gtk_container_add(GTK_CONTAINER(evbox),hbox);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",G_CALLBACK(closeTab),(void*)vbox);
	gtk_signal_connect(GTK_OBJECT(evbox),"button-press-event",G_CALLBACK(tabPopUp),(void*)page);

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
	gchar*			buffer=NULL;
	long			filelen;
	GtkWidget*		label;
	gchar*			filename=g_path_get_basename(filepath);
	pageStruct*		page=(pageStruct*)malloc(sizeof(pageStruct));
	GtkTextMark*	scroll2mark=gtk_text_mark_new(NULL,true);
	char*			str=NULL;

	if(!g_file_test(filepath,G_FILE_TEST_EXISTS))
		return(false);

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	asprintf(&page->filePath,"%s",filepath);
	asprintf(&page->fileName,"%s",filename);
	
	page->buffer=gtk_source_buffer_new(NULL);
	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	g_signal_connect(G_OBJECT(page->view),"populate-popup",G_CALLBACK(populatePopupMenu),NULL);

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
		if(g_utf8_validate(buffer,-1,NULL)==false)
			{
				str=g_locale_to_utf8(buffer,-1,NULL,NULL,NULL);
				gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,str,-1);
				g_free(str);
			}
		else
			{
				gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,buffer,-1);
				g_free(buffer);
			}
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
	page->markList=NULL;
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

	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);

	g_free(filename);

	return TRUE;
}

bool getSaveFile(void)
{
	GtkWidget*	dialog;
	bool		retval=false;

	dialog=gtk_file_chooser_dialog_new("Save File",(GtkWindow*)window, GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);

	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER (dialog),TRUE);
	if(saveFileName!=NULL)
		{
			if(saveFilePath!=NULL)
				gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),g_path_get_dirname(saveFilePath));
			gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),saveFileName);
		}
 	else
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),"Untitled");

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
		{
			saveFilePath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			saveFileName=g_path_get_basename(saveFilePath);
			retval=true;
		}

	gtk_widget_destroy(dialog);
	gtk_widget_show_all(window);
	return(retval);
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
	if(page->filePath!=NULL && data==NULL)
		{
			fd=fopen(page->filePath,"w");
			if (fd!=NULL)
				{
					fputs(text,fd);
					fclose(fd);
					gtk_text_buffer_set_modified((GtkTextBuffer*)page->buffer,false);
				}
		}
	else
		{
			if(data!=NULL)
				{
					saveFilePath=page->filePath;
					saveFileName=page->fileName;
				}

			saveFileName=page->fileName;
			if(getSaveFile()==false)
				return(false);
			page->filePath=saveFilePath;
			page->fileName=saveFileName;

			gtk_text_buffer_set_modified ((GtkTextBuffer*)page->buffer,FALSE);

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
	setSensitive();
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
	asprintf(&page->fileName,"Untitled-%i",untitledNumber);
	untitledNumber++;
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
	page->gFile=NULL;
	page->monitor=NULL;
	gtk_widget_set_sensitive((GtkWidget*)saveButton,false);
	gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);
	gtk_widget_show_all(window);

	gtk_widget_grab_focus((GtkWidget*)page->view);
}

void openAsHexDump(GtkWidget *widget,gpointer user_data)
{
	GtkWidget*		dialog;
	char*			filepath;
	char*			filename;
	int				pagenum;
	FILE*			fp;
	char			line[1024];
	GString*		str=g_string_new(NULL);
	char*			command;
	GtkTextIter		iter;
	pageStruct*		page;
	char*			convstr=NULL;

	dialog=gtk_file_chooser_dialog_new("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filepath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			filename=g_path_get_basename(filepath);
			newFile(NULL,NULL);
			pagenum=currentPage-1;
			page=getPageStructPtr(pagenum);
			asprintf(&command,"hexdump -C %s",filepath);
			fp=popen(command, "r");
			while(fgets(line,1024,fp))
				g_string_append_printf(str,"%s",line);

			pclose(fp);

			gtk_source_buffer_begin_not_undoable_action(page->buffer);
				gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
				if(g_utf8_validate(str->str,-1,NULL)==false)
					{
						convstr=g_locale_to_utf8(str->str,-1,NULL,NULL,NULL);
						gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,convstr,-1);
						g_free(convstr);
					}
				else
					{
						gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,str->str,-1);
					}
			gtk_source_buffer_end_not_undoable_action(page->buffer);
			gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),true);
			g_string_free(str,true);
			g_free (filepath);
			g_free (filename);
			setSensitive();
		}

	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}

void reloadFile(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gchar*		buffer;
	long		filelen;
	GtkTextIter	start;
	GtkTextIter	end;

	g_file_get_contents(page->filePath,&buffer,(gsize*)&filelen,NULL);

	gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
	gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
	gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,buffer,filelen);
	g_free(buffer);
}

void saveSession(GtkWidget* widget,gpointer data)
{
	pageStruct*		page;
	FILE*			fd=NULL;
	char*			filename;
	GtkTextMark*	mark;
	GtkTextIter		iter;
	int				linenumber;
	GList*			listelement;
	GtkTextIter		markiter;

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);
	asprintf(&filename,"%s/.KKEdit/session",getenv("HOME"));
	fd=fopen(filename,"w");
	if (fd!=NULL)
		{
			for(int loop=0;loop<gtk_notebook_get_n_pages(notebook);loop++)
				{
					page=getPageStructPtr(loop);
					mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
					linenumber=gtk_text_iter_get_line(&iter);
					fprintf(fd,"%s %i\n",page->filePath,linenumber);
					if(page->markList!=NULL)
						{
							listelement=page->markList;
							for(int loop2=0;loop2<g_list_length(page->markList);loop2++)
								{
									gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&markiter,((bookMarkStruct*)listelement->data)->mark);
									fprintf(fd,"%i ",gtk_text_iter_get_line(&markiter));
									fprintf(fd,"%s\n",((bookMarkStruct*)listelement->data)->label);
									listelement=g_list_next(listelement);
								}
						}
					fprintf(fd,"-1 endmarks\n",page->filePath);
				}
			fclose(fd);
			g_free(filename);
		}
}

void restoreSession(GtkWidget* widget,gpointer data)
{
	FILE*		fd=NULL;
	char*		filename;
	char		buffer[2048];
	int			intarg;
	char		strarg[2048];
	pageStruct*	page;
	GtkTextIter	markiter;
	GtkWidget*	menuitem;

	asprintf(&filename,"%s/.KKEdit/session",getenv("HOME"));
	fd=fopen(filename,"r");
	if (fd!=NULL)
		{
			closeAllTabs(NULL,NULL);
			while(fgets(buffer,2048,fd)!=NULL)
				{
					sscanf(buffer,"%s %i",(char*)&strarg,(int*)&intarg);
					openFile(strarg,intarg);
					fgets(buffer,2048,fd);
					sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
					page=getPageStructPtr(currentPage-1);
					gtk_notebook_set_current_page(notebook,currentPage-1);
					while(intarg!=-1)
						{
							gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
							intarg++;
							gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
							addBookmark(NULL,NULL);
							fgets(buffer,2048,fd);
							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
						}
				}
			fclose(fd);
			g_free(filename);
		}
}

int showFileChanged(char* filename)
{
	GtkWidget*	dialog;
	gint		result;
	char*		message;

	asprintf(&message,"File %s Has Changed on disk\nDo you want to reload it?",filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_REFRESH,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Warning file changed!");

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	g_free(message);
	return(result);
}

void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data)
{
	pageStruct*		page=(pageStruct*)user_data;
	GtkTextIter		start;
	GtkTextIter		end;
	gchar*			buffer;
	long			filelen;
	int				answer;

	if(G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT==event_type)
		{
			if(page->itsMe==false)
				{
					answer=showFileChanged(page->fileName);
					if(answer==GTK_RESPONSE_YES)
						{
							g_file_get_contents(page->filePath,&buffer,(gsize*)&filelen,NULL);
							gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
							gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
							gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
							gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,buffer,filelen);
							g_free(buffer);
						}
				}
			else
				page->itsMe=false;
		}
}

