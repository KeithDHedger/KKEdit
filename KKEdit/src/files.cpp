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
#include "encoding.h"

GtkWidget*	vbox;
char*		saveFileName=NULL;
char*		saveFilePath=NULL;
bool		dropTextFile=false;

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
	gtk_signal_connect(GTK_OBJECT(button),"clicked",G_CALLBACK(closeTab),(void*)page->tabVbox);
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

void dropText(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	gchar**			array=NULL;
	int				cnt;
	char*			filename;
	FILE*			fp;
	char*			command;
	GString*		str;
	char			line[1024];
	pageStruct*		page=(pageStruct*)user_data;
	GtkTextIter		iter;
	GtkTextMark*	mark;

	array=gtk_selection_data_get_uris(selection_data);
	if(array==NULL)
		{
			gtk_drag_finish (context,true,true,time);
			return;
		}

	cnt=g_strv_length(array);

	if(dropTextFile==false)
		{
			dropTextFile=true;

			for(int j=0;j<cnt;j++)
				{
					str=g_string_new(NULL);
					filename=g_filename_from_uri(array[j],NULL,NULL);
					asprintf(&command,"cat %s",filename);
					fp=popen(command, "r");
					while(fgets(line,1024,fp))
						g_string_append_printf(str,"%s",line);
					gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(page->buffer),str->str,str->len);

					mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
					gtk_text_iter_forward_chars(&iter,str->len);
					gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);

					g_free(command);
					g_string_free(str,true);
					pclose(fp);
				}

			g_strfreev(array);
		}
	else
			dropTextFile=false;

	gtk_drag_finish (context,true,true,time);
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
	refreshMainWindow();
	return(retval);
}

bool saveFile(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start,end;
	gchar*		text;
	FILE*		fd=NULL;
	GtkWidget*	dialog;

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
			else
				{
					dialog=gtk_message_dialog_new((GtkWindow*)window,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"Can't save file '%s' :(",page->filePath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
					return(false);
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

			fd=fopen(saveFilePath,"w");
			if (fd!=NULL)
				{
					page->filePath=saveFilePath;
					page->fileName=saveFileName;

					gtk_text_buffer_set_modified ((GtkTextBuffer*)page->buffer,FALSE);

					gtk_widget_set_tooltip_text(page->tabName,page->filePath);
					gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)saveFileName);

					fprintf(fd,"%s",text);
					fclose(fd);
				}
			else
				{
					dialog=gtk_message_dialog_new((GtkWindow*)window,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"Can't save file '%s' :(",page->filePath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
					return(false);
				}

			saveFileName=NULL;
			saveFilePath=NULL;
		}
	if(page->lang==NULL)
		setLanguage(page);
	switchPage(notebook,NULL,currentTabNumber,NULL);
	setSensitive();
	return(true);
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
			gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);
			
			g_string_free(str,true);
			g_free (filepath);
			g_free (filename);
			setSensitive();
		}

	gtk_widget_destroy (dialog);
	refreshMainWindow();
}

void reloadFile(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gchar*		buffer;
	long		filelen;
	GtkTextIter	start;
	GtkTextIter	end;

	if(page->filePath!=NULL)
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
							for(int loop2=0;loop2<(int)g_list_length(page->markList);loop2++)
								{
									gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&markiter,((bookMarkStruct*)listelement->data)->mark);
									fprintf(fd,"%i ",gtk_text_iter_get_line(&markiter));
									fprintf(fd,"%s\n",((bookMarkStruct*)listelement->data)->label);
									listelement=g_list_next(listelement);
								}
						}
					fprintf(fd,"-1 endmarks\n");
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
							if((bool)data==true)
								{
									gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
									intarg++;
									gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
									addBookmark(NULL,NULL);
								}
							fgets(buffer,2048,fd);
							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
						}
				}
			fclose(fd);
			g_free(filename);
		}

		showHideWidget(lineNumberWidget,showJumpToLine);
		showHideWidget(findApiWidget,showFindAPI);
		showHideWidget(findDefWidget,showFindDef);
		showHideWidget(liveSearchWidget,showLiveSearch);
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

pageStruct* makeNewPage(void)
{
	pageStruct*		page;

	page=(pageStruct*)malloc(sizeof(pageStruct));

	page->pane=gtk_vpaned_new();
	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	page->pageWindow2=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

	page->buffer=gtk_source_buffer_new(NULL);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive),NULL);
	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	g_signal_connect(G_OBJECT(page->view),"populate-popup",G_CALLBACK(populatePopupMenu),NULL);
	page->view2=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	setFilePrefs(page->view);

	gtk_paned_add1(GTK_PANED(page->pane),(GtkWidget*)page->pageWindow);
	gtk_container_add (GTK_CONTAINER(page->pageWindow),(GtkWidget*)page->view);
	g_signal_connect(G_OBJECT(page->view),"button-release-event",G_CALLBACK(whatPane),(void*)1);

	page->rebuildMenu=true;
	page->isFirst=true;
	page->itsMe=false;
	page->markList=NULL;
	page->inTop=true;
	page->gFile=NULL;
	page->monitor=NULL;
	page->isSplit=false;
	page->lang=NULL;
	page->tabVbox=NULL;
//dnd
	gtk_drag_dest_set((GtkWidget*)page->view,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets((GtkWidget*)page->view);
	gtk_drag_dest_add_text_targets((GtkWidget*)page->view);
	g_signal_connect_after(G_OBJECT(page->view),"drag-data-received",G_CALLBACK(dropText),(void*)page);

	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),FALSE);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive),NULL);
	gtk_widget_grab_focus((GtkWidget*)page->view);

	return(page);
}


guint get_encoding_code(void)
{
	static guint code = END_CODE;
	const gchar *env;
	guint i, j = 1;
	
	if (code == END_CODE) {
		env = g_getenv("LC_ALL");
		if (!env)
			env = g_getenv("LANG");
		if (env && strlen(env) >= 2)
			while (code == END_CODE && j < END_CODE) {
				for (i = 0; i < MAX_COUNTRY_NUM; i++) {
					if (!country_table[j][i])
						break;
					if (strncmp(env, country_table[j][i], strlen(country_table[j][i])) == 0) {
						code = j;
						break;
					}
				}
				j++;
			}
		if (code == END_CODE)
			code = 0;
	}
	
	return code;
}

EncArray *get_encoding_items(guint code)
{
	gint i;
	static EncArray *array = NULL;
	
	if (!array) {
		array = (EncArray*)g_malloc(sizeof(EncArray));
		for (i = 0; i < ENCODING_MAX_ITEM_NUM; i++)
			array->item[i] = encoding_table[code][i] ?
				encoding_table[code][i] : NULL;
	}
	
	return array;
}


static const gchar *detect_charset_cylillic(const gchar *text)
{
	guint8 c = *text;
	gboolean noniso = FALSE;
	guint32 xc = 0, xd = 0, xef = 0;
	
	const gchar *charset = get_encoding_items(get_encoding_code())->item[OPENI18N];
	
	while ((c = *text++) != '\0') {
		if (c >= 0x80 && c <= 0x9F)
			noniso = TRUE;
		else if (c >= 0xC0 && c <= 0xCF)
			xc++;
		else if (c >= 0xD0 && c <= 0xDF)
			xd++;
		else if (c >= 0xE0)
			xef++;
	}
	
	if (!noniso && ((xc + xef) < xd))
		charset = "ISO-8859-5";
	else if ((xc + xd) < xef)
		charset = "CP1251";
	
	return charset;
}

static const gchar *detect_charset_chinese(const gchar *text)
{
	guint8 c = *text;
	
	const gchar *charset = get_encoding_items(get_encoding_code())->item[IANA];
	
	while ((c = *text++) != '\0') {
		if (c >= 0x81 && c <= 0x87) {
			charset = "GB18030";
			break;
		}
		else if (c >= 0x88 && c <= 0xA0) {
			c = *text++;
			if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = "GB18030";
				break;
			} //else GBK/Big5-HKSCS cannot determine
		}
		else if ((c >= 0xA1 && c <= 0xC6) || (c >= 0xC9 && c <= 0xF9)) {
			c = *text++;
			if (c >= 0x40 && c <= 0x7E)
				charset = "BIG5";
			else if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = "GB18030";
				break;
			}
		}
		else if (c >= 0xC7) {
			c = *text++;
			if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = "GB18030";
				break;
			}
		}
	}
	
	return charset;
}

static const gchar *detect_charset_japanese(const gchar *text)
{
	guint8 c = *text;
	gchar *charset = NULL;
	
	while (charset == NULL && (c = *text++) != '\0') {
		if (c >= 0x81 && c <= 0x9F) {
			if (c == 0x8E) /* SS2 */ {
				c = *text++;
				if ((c >= 0x40 && c <= 0xA0) || (c >= 0xE0 && c <= 0xFC))
					charset = "CP932";
			}
			else if (c == 0x8F) /* SS3 */ {
				c = *text++;
				if (c >= 0x40 && c <= 0xA0)
					charset = "CP932";
				else if (c >= 0xFD)
					break;
			}
			else
				charset = "CP932";
		}
		else if (c >= 0xA1 && c <= 0xDF) {
			c = *text++;
			if (c <= 0x9F)
				charset = "CP932";
			else if (c >= 0xFD)
				break;
		}
		else if (c >= 0xE0 && c <= 0xEF) {
			c = *text++;
			if (c >= 0x40 && c <= 0xA0)
				charset = "CP932";
			else if (c >= 0xFD)
				break;
		}
		else if (c >= 0xF0)
			break;
	}
	
	if (charset == NULL)
		charset = "EUC-JP";
	
	return charset;
}

static const gchar *detect_charset_korean(const gchar *text)
{
	guint8 c = *text;
	gboolean noneuc = FALSE;
	gboolean nonjohab = FALSE;
	gchar *charset = NULL;
	
	while (charset == NULL && (c = *text++) != '\0') {
		if (c >= 0x81 && c < 0x84) {
			charset = "CP949";
		}
		else if (c >= 0x84 && c < 0xA1) {
			noneuc = TRUE;
			c = *text++;
			if ((c > 0x5A && c < 0x61) || (c > 0x7A && c < 0x81))
				charset = "CP1361";
			else if (c == 0x52 || c == 0x72 || c == 0x92 || (c > 0x9D && c < 0xA1)
				|| c == 0xB2 || (c > 0xBD && c < 0xC1) || c == 0xD2
				|| (c > 0xDD && c < 0xE1) || c == 0xF2 || c == 0xFE)
				charset = "CP949";
		}
		else if (c >= 0xA1 && c <= 0xC6) {
			c = *text++;
			if (c < 0xA1) {
				noneuc = TRUE;
				if ((c > 0x5A && c < 0x61) || (c > 0x7A && c < 0x81))
					charset = "CP1361";
				else if (c == 0x52 || c == 0x72 || c == 0x92 || (c > 0x9D && c < 0xA1))
					charset = "CP949";
				else if (c == 0xB2 || (c > 0xBD && c < 0xC1) || c == 0xD2
					|| (c > 0xDD && c < 0xE1) || c == 0xF2 || c == 0xFE)
					nonjohab = TRUE;
			}
		}
		else if (c > 0xC6 && c <= 0xD3) {
			c = *text++;
			if (c < 0xA1)
				charset = "CP1361";
		}
		else if (c > 0xD3 && c < 0xD8) {
			nonjohab = TRUE;
			c = *text++;
		}
		else if (c >= 0xD8) {
			c = *text++;
			if (c < 0xA1)
				charset = "CP1361";
		}
		if (noneuc && nonjohab)
			charset = "CP949";
	}
	
	if (charset == NULL) {
		if (noneuc)
			charset = "CP949";
		else
			charset = "EUC-KR";
	}
	
	return charset;
}


static gboolean detect_noniso(const gchar *text)
{
	guint8 c = *text;
	
	while ((c = *text++) != '\0') {
		if (c >= 0x80 && c <= 0x9F)
			return TRUE;
	}
	return FALSE;
}


const gchar *get_default_charset(void)
{
	const gchar *charset;
	
	g_get_charset(&charset);
	
	return charset;
}

const gchar *detect_charset(const gchar *text)
{
	guint8 c = *text;
	const gchar *charset = NULL;
	
	if (g_utf8_validate(text, -1, NULL)) {
		while ((c = *text++) != '\0') {
			if (c > 0x7F) {
				charset = "UTF-8";
				break;
			}
			if (c == 0x1B) /* ESC */ {
				c = *text++;
				if (c == '$') {
					c = *text++;
					switch (c) {
					case 'B': // JIS X 0208-1983
					case '@': // JIS X 0208-1978
						charset = "ISO-2022-JP";
						continue;
					case 'A': // GB2312-1980
						charset = "ISO-2022-JP-2";
						break;
					case '(':
						c = *text++;
						switch (c) {
						case 'C': // KSC5601-1987
						case 'D': // JIS X 0212-1990
							charset = "ISO-2022-JP-2";
						}
						break;
					case ')':
						c = *text++;
						if (c == 'C')
							charset = "ISO-2022-KR"; // KSC5601-1987
					}
					break;
				}
			}
		}
		if (!charset)
			charset = get_default_charset();
	}
	
	if (!charset) {
		switch (get_encoding_code()) {
		case LATINC:
		case LATINC_UA:
		case LATINC_TJ:
			charset = detect_charset_cylillic(text); // fuzzy...
			break;
		case CHINESE_CN:
		case CHINESE_TW:
		case CHINESE_HK:
			charset = detect_charset_chinese(text);
			break;
		case JAPANESE:
			charset = detect_charset_japanese(text);
			break;
		case KOREAN:
			charset = detect_charset_korean(text);
			break;
		case VIETNAMESE:
		case THAI:
		case GEORGIAN:
			charset = get_encoding_items(get_encoding_code())->item[OPENI18N];
			break;
		default:
			if (strcmp(get_default_charset(), "UTF-8") != 0)
				charset = get_default_charset();
			else if (detect_noniso(text))
				charset = get_encoding_items(get_encoding_code())->item[CODEPAGE];
			else
				charset = get_encoding_items(get_encoding_code())->item[OPENI18N];
			if (!charset)
				charset = get_encoding_items(get_encoding_code())->item[IANA];					
		}
	}
	
	return charset;
}

bool openFile(const gchar *filepath,int linenumber)
{
	GtkTextIter		iter;
	gchar*			buffer=NULL;
	long			filelen;
	GtkWidget*		label;
	gchar*			filename=g_path_get_basename(filepath);
	pageStruct*		page;
	GtkTextMark*	scroll2mark=gtk_text_mark_new(NULL,true);
	char*			str=NULL;
	char*			recenturi;
	int				linenum=linenumber-1;

	gchar *contents;
	gsize length;
	GError *err = NULL;
	const gchar *charset;

	if(!g_file_test(filepath,G_FILE_TEST_EXISTS))
		return(false);
	if(linenum<0)
		linenum=0;

	page=makeNewPage();
	page->tabVbox=gtk_vbox_new(true,4);

	page->filePath=strdup(filepath);
	page->fileName=strdup(filename);

	label=makeNewTab(page->fileName,page->filePath,page);
	setLanguage(page);


g_file_get_contents(filepath, &contents, &length, &err);
	GRegex*					regex;
	GMatchInfo*				match_info=NULL;
	GRegexCompileFlags		compileflags=(GRegexCompileFlags)(G_REGEX_MULTILINE|G_REGEX_EXTENDED);
	GRegexMatchFlags		matchflags=(GRegexMatchFlags)(G_REGEX_MATCH_NOTBOL|G_REGEX_MATCH_NOTEOL);
	char*					reptext=NULL;
	char*					searchtext=NULL;
	char*					replacetext=NULL;

			compileflags=(GRegexCompileFlags)(compileflags|G_REGEX_CASELESS);


		charset = detect_charset(contents);
		if (charset == NULL)
//getCharset((char*)filepath);
			charset = get_default_charset();
//charset=currentCharset;
gsize	br;
	br=length;

	if (length)
		do {
			if (err) {
				charset = "ISO-8859-1";
				g_error_free(err);
				err = NULL;
			}
			str = g_convert(contents,br, "UTF-8", charset, NULL, &br, &err);
		} while (err);
	else
		str = g_strdup("");
	g_free(contents);

searchtext="[^[:print:]\n\r]";
replacetext="";
regex=g_regex_new(searchtext,(GRegexCompileFlags)compileflags,matchflags,NULL);
printf("QQQQQQQQQQQQQ\n");
contents=strdup(str);
str=g_regex_replace(regex,str,br,0,replacetext,matchflags,NULL);
//str=g_regex_replace_full(regex,contents,br,0,replacetext,matchflags,&match_info,NULL);
//str=g_regex_escape_nul(str,br);

printf("ZZZZZZZZZZ%s\n%i\n",str,(int)br);
//	gtk_text_buffer_insert(buffer, &iter, str, strlen(str));
	gtk_source_buffer_begin_not_undoable_action(page->buffer);
		gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &iter);
gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,str,strlen(str));
				g_free(str);

/*
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
*/
	gtk_source_buffer_end_not_undoable_action(page->buffer);

	page->gFile=g_file_new_for_path(page->filePath);
	page->monitor=g_file_monitor(page->gFile,(GFileMonitorFlags)G_FILE_MONITOR_NONE,NULL,NULL);
	g_signal_connect(G_OBJECT(page->monitor),"changed",G_CALLBACK(fileChangedOnDisk),(void*)page);

	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);

	str=g_file_get_path(page->gFile);
	recenturi=g_filename_to_uri(str,NULL,NULL);
	gtk_recent_manager_add_item(gtk_recent_manager_get_default(),recenturi);
	g_free(filename);
	g_free(recenturi);
	g_free(str);

//connect to ntebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pane));
	g_object_set_data(G_OBJECT(page->tabVbox),"pagedata",(gpointer)page);

	gtk_notebook_append_page(notebook,page->tabVbox,label);
	gtk_notebook_set_tab_reorderable(notebook,page->tabVbox,true);
	gtk_notebook_set_current_page(notebook,currentPage);
	currentPage++;
	gtk_widget_grab_focus((GtkWidget*)page->view);

   /* move cursor to the linenumber */
	gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,linenum,0);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
	gtk_text_iter_set_line(&iter,linenum);
	gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(page->buffer),scroll2mark,&iter);  
	gtk_text_view_scroll_to_mark((GtkTextView*)page->view,scroll2mark,0,true,0,0.5);
	gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(page->buffer),scroll2mark);

	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);

	gtk_widget_show_all((GtkWidget*)notebook);

	return TRUE;
}

void newFile(GtkWidget* widget,gpointer data)
{
	GtkTextIter	iter;
	GtkWidget*	label;
	pageStruct*	page;

	page=makeNewPage();
	page->tabVbox=gtk_vbox_new(true,4);
	page->filePath=NULL;

	asprintf(&page->fileName,"Untitled-%i",untitledNumber);
	untitledNumber++;

	label=makeNewTab(page->fileName,NULL,page);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);

//connect to ntebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pane));
	g_object_set_data(G_OBJECT(page->tabVbox),"pagedata",(gpointer)page);

	gtk_notebook_append_page(notebook,page->tabVbox,label);
	gtk_notebook_set_tab_reorderable(notebook,page->tabVbox,true);
	gtk_notebook_set_current_page(notebook,currentPage);
	currentPage++;
	gtk_widget_show_all((GtkWidget*)notebook);

}