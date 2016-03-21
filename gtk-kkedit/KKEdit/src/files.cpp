/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:49:15 GMT 2015 kdhedger68713@gmail.com

 * This file (files.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kkedit-includes.h"
#include <iconv.h>

GtkWidget*	vbox;
char*		saveFileName=NULL;
char*		saveFilePath=NULL;
bool		dropTextFile=false;
char		*convertedData=NULL;
long		dataLen=0;

VISIBLE void saveVarsToFile(char* filepath,args* dataptr)
{
	ERRDATA
	FILE*	fd=NULL;
	int		cnt=0;
	GSList*	list=NULL;

	fd=fopen(filepath,"w");
	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
							case TYPEINT:
//				printf("%s\n%s\n%i\n%i\n",filepath,dataptr[cnt].name,dataptr[cnt].type,*(int*)(dataptr[cnt].data));
								fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
								break;
							case TYPESTRING:
//				printf("%s\n%s\n%i\n%s\n",filepath,dataptr[cnt].name,dataptr[cnt].type,*(char**)(dataptr[cnt].data));
								fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
								break;
							case TYPEBOOL:
//				printf("%s\n%s\n%i\n%i\n",filepath,dataptr[cnt].name,dataptr[cnt].type,(int)*(bool*)(dataptr[cnt].data));
								fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
								break;
							case TYPELIST:
								list=*(GSList**)((dataptr[cnt].data));
								if(g_slist_length(list)>maxFRHistory)
									list=g_slist_nth(list,g_slist_length(list)-maxFRHistory);
								while(list!=NULL)
									{
										if(strlen((char*)list->data)>0)
											{
												fprintf(fd,"%s\t%s\n",dataptr[cnt].name,(char*)list->data);
											}
										list=list->next;
									}
								break;
						}
					cnt++;
				}
			fclose(fd);
		}
	ERRDATA
}

VISIBLE void loadVarsFromFile(char* filepath,args* dataptr)
{
	ERRDATA
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	argname=NULL;
	char*	strarg=NULL;
	char*	liststr=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					sscanf(buffer,"%ms %ms",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) && (argname!=NULL) && (strcmp(argname,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
											case TYPEINT:
												*(int*)dataptr[cnt].data=atoi(strarg);
												break;
											case TYPESTRING:
												if(*(char**)(dataptr[cnt].data)!=NULL)
													{
														ERRDATA debugFree(&*(char**)(dataptr[cnt].data));
													}
												sscanf(buffer,"%*s %m[^\n]s",(char**)dataptr[cnt].data);
												break;
											case TYPEBOOL:
												*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
												break;
											case TYPELIST:
												sscanf(buffer,"%*s\t%m[^\n]s",&liststr);
												*(GSList**)dataptr[cnt].data=g_slist_append(*(GSList**)dataptr[cnt].data,liststr);
												break;
										}
								}
							cnt++;
						}
					freeAndNull(&argname);
					freeAndNull(&strarg);
				}
			fclose(fd);
		}
	ERRDATA
}

GtkWidget* makeNewTab(char* name,char* tooltip,pageStruct* page)
{
	ERRDATA
	GtkWidget*	evbox=gtk_event_box_new();
	GtkWidget*	hbox;
	GtkWidget*	label;
//	GtkWidget*	close=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	GtkWidget*	close=gtk_image_new_from_icon_name(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
//	GtkWidget*	close==createNewImageMenuItem(GTK_STOCK_CLOSE,);

	GtkWidget*	button=gtk_button_new();

//TODO//
//	GtkRcStyle*	style=gtk_rc_style_new();
	char*		correctedname;

	hbox=creatNewBox(NEWHBOX,false,0);
	ERRDATA rebuildTabsMenu();

	correctedname=truncateWithElipses(name,maxTabChars);

	label=gtk_label_new(correctedname);
	ERRDATA debugFree(&correctedname);

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(label,tooltip);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
	gtk_container_add(GTK_CONTAINER(button),close);

	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	gtk_container_add(GTK_CONTAINER(evbox),hbox);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(closeTab),(void*)page->tabVbox);
	g_signal_connect(G_OBJECT(evbox),"button-press-event",G_CALLBACK(tabPopUp),(void*)page);

	page->tabName=label;

//TODO//
//	style->xthickness=style->ythickness=0;
//	gtk_widget_modify_style (button, style);
//	g_object_unref(G_OBJECT(style));

	gtk_widget_show_all(evbox);

	ERRDATA
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newTab");

	ERRDATA return(evbox);
}

void setFilePrefs(pageStruct* page)
{
	ERRDATA
	PangoFontDescription*	font_desc;
	GdkColor				color;
	GtkTextAttributes*		attr;

	ERRDATA
	gtk_source_view_set_auto_indent(page->view,indent);
	gtk_source_view_set_show_line_numbers(page->view,lineNumbers);
	gtk_source_view_set_highlight_current_line(page->view,highLight);
	gtk_source_view_set_show_line_marks(page->view,showBMBar);
	gdk_color_parse(highlightColour,&color);
	//TODO//
#ifndef _USEGTK3_
	gtk_source_view_set_mark_category_background(page->view,MARK_TYPE_1,&color);
#endif
	if(lineWrap==true)
		{
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);		
			gtk_text_view_set_wrap_mode((GtkTextView *)page->view,GTK_WRAP_WORD_CHAR);
		}
	else
		{
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);		
			gtk_text_view_set_wrap_mode((GtkTextView *)page->view,GTK_WRAP_NONE);
		}

	gtk_source_view_set_tab_width(page->view,tabWidth);

	font_desc=pango_font_description_from_string(fontAndSize);
//TODO//
#ifndef _USEGTK3_
	gtk_widget_modify_font((GtkWidget*)page->view,font_desc);
#endif
	pango_font_description_free(font_desc);

	attr=gtk_text_view_get_default_attributes((GtkTextView*)page->view);
	g_object_set((gpointer)page->highlightTag,"background",gdk_color_to_string((const GdkColor*)&attr->appearance.fg_color),"foreground",gdk_color_to_string((const GdkColor*)&attr->appearance.bg_color),NULL);
	gtk_text_attributes_unref(attr);

	if(noSyntax==true)
		gtk_source_buffer_set_highlight_syntax (page->buffer,false);
	else
		gtk_source_buffer_set_highlight_syntax (page->buffer,true);

//TODO//
#ifndef _USEGTK3_
	if(autoShowComps==true)
		gtk_source_completion_unblock_interactive(page->completion);
	else
		gtk_source_completion_block_interactive(page->completion);
#endif

	ERRDATA
	createCompletion(page);
	ERRDATA
}

void resetAllFilePrefs(void)
{
	ERRDATA
	pageStruct*			page;

	ERRDATA
	styleScheme=gtk_source_style_scheme_manager_get_scheme(schemeManager,styleName);

	for(int loop=0; loop<gtk_notebook_get_n_pages(mainNotebook); loop++)
		{
			page=getPageStructPtr(loop);
			gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);
			setFilePrefs(page);
		}
	ERRDATA
}

void dropText(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	ERRDATA
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

	ERRDATA
	array=gtk_selection_data_get_uris(selection_data);
	if(array==NULL)
		{
			gtk_drag_finish (context,true,true,time);
			ERRDATA return;
		}

	cnt=g_strv_length(array);

	if(dropTextFile==false)
		{
			dropTextFile=true;

			for(int j=0; j<cnt; j++)
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

					ERRDATA debugFree(&command);
					g_string_free(str,true);
					pclose(fp);
				}

			g_strfreev(array);
		}
	else
		dropTextFile=false;

	ERRDATA
	gtk_drag_finish (context,true,true,time);
	ERRDATA
}

bool getSaveFile(void)
{
	ERRDATA
	GtkWidget*	dialog;
	bool		retval=false;

	dialog=gtk_file_chooser_dialog_new(gettext("Save File"),(GtkWindow*)mainWindow, GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);

	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER (dialog),TRUE);
	if(saveFileName!=NULL)
		{
			if(saveFilePath!=NULL)
				gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),g_path_get_dirname(saveFilePath));
			gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),saveFileName);
		}
	else
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),gettext("Untitled"));

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
		{
			saveFilePath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			saveFileName=g_path_get_basename(saveFilePath);
			retval=true;
		}

	gtk_widget_destroy(dialog);
	refreshMainWindow();
	ERRDATA return(retval);
}

VISIBLE bool saveFile(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start,end;
	gchar*		text;
	FILE*		fd=NULL;
	GtkWidget*	dialog;

	ERRDATA
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
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Can't save file '%s' :("),page->filePath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
					ERRDATA return(false);
				}
		}
	else
		{
			if(data!=NULL)
				{
					saveFilePath=page->filePath;
					saveFileName=page->fileName;
					if(page->dirName!=NULL)
						{
							ERRDATA debugFree(&page->dirName);
						}
					page->dirName=g_path_get_dirname(page->filePath);
				}

			saveFileName=page->fileName;
			if(getSaveFile()==false)
				{
					ERRDATA return(false);
				}

			fd=fopen(saveFilePath,"w");
			ERRDATA
			if (fd!=NULL)
				{
					page->filePath=saveFilePath;
					page->fileName=saveFileName;
					if(page->dirName!=NULL)
						{
							ERRDATA debugFree(&page->dirName);
						}
					page->dirName=g_path_get_dirname(page->filePath);

					gtk_text_buffer_set_modified ((GtkTextBuffer*)page->buffer,FALSE);

					gtk_widget_set_tooltip_text(page->tabName,page->filePath);
					gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)saveFileName);

					fprintf(fd,"%s",text);
					fclose(fd);
				}
			else
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Can't save file '%s' :("),page->filePath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
					ERRDATA return(false);
				}

			saveFileName=NULL;
			saveFilePath=NULL;
		}
	if(page->lang==NULL)
		setLanguage(page);
	switchPage(mainNotebook,page->tabVbox,currentTabNumber,NULL);
	setSensitive();

	ERRDATA
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"saveFile");

	ERRDATA return(true);
}

VISIBLE void openAsHexDump(GtkWidget *widget,gpointer user_data)
{
	ERRDATA
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

	ERRDATA
	dialog=gtk_file_chooser_dialog_new(gettext("Open File"),NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
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
					ERRDATA debugFree(&convstr);
				}
			else
				{
					gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,str->str,-1);
				}
			gtk_source_buffer_end_not_undoable_action(page->buffer);
			gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);

			g_string_free(str,true);
			ERRDATA debugFree(&filepath);
			ERRDATA debugFree(&filename);
			setSensitive();
		}

	gtk_widget_destroy (dialog);
	refreshMainWindow();
	ERRDATA
}

VISIBLE void reloadFile(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(-1);
	gchar*		buffer;
	long		filelen;
	GtkTextIter	start;
	GtkTextIter	end;

	ERRDATA
	if(page->filePath!=NULL)
		{
			g_file_get_contents(page->filePath,&buffer,(gsize*)&filelen,NULL);
			gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
			gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
			gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
			gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,buffer,filelen);
			ERRDATA debugFree(&buffer);
		}
	ERRDATA
}

VISIBLE void saveSession(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*		page;
	FILE*			fd=NULL;
	char*			filename;
	GtkTextMark*	mark;
	GtkTextIter		iter;
	int				linenumber;
	GtkTextIter		markiter;
	GList*			ptr;

	ERRDATA
	asprintf(&filename,"%s/." KKEDITVERS,getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	ERRDATA debugFree(&filename);
	asprintf(&filename,"%s/." KKEDITVERS "/session",getenv("HOME"));
	fd=fopen(filename,"w");
	if (fd!=NULL)
		{
			for(int loop=0; loop<gtk_notebook_get_n_pages(mainNotebook); loop++)
				{
					page=getPageStructPtr(loop);
					mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
					linenumber=gtk_text_iter_get_line(&iter);
					fprintf(fd,"%i %s\n",linenumber,page->filePath);

					ptr=newBookMarksList;
					while(ptr!=NULL)
						{
							if(((bookMarksNew*)ptr->data)->page==page)
								{
									gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&markiter,(GtkTextMark*)((bookMarksNew*)ptr->data)->mark);
									fprintf(fd,"%i ",gtk_text_iter_get_line(&markiter));
									fprintf(fd,"%s\n",((bookMarksNew*)ptr->data)->label);
								}
							ptr=g_list_next(ptr);
						}
					fprintf(fd,"-1 endmarks\n");
				}

			ERRDATA
			fclose(fd);
			ERRDATA debugFree(&filename);
		}
	ERRDATA
}

VISIBLE void restoreSession(GtkWidget* widget,gpointer data)
{
	ERRDATA
	FILE			*fd=NULL;
	char			*filename;
	char			buffer[2048];
	int				intarg;
	char			strarg[2048];
	pageStruct		*page;
	GtkTextIter		markiter;
	int				currentline;
	TextBuffer		*buf=new TextBuffer;

	ERRDATA

	closeAllTabs(NULL,NULL);
	while(gtk_events_pending())
		gtk_main_iteration_do(false);

	doUpdateWidgets=false;
	showBarberPole(gettext("Restoring Session ..."));

	if(data==NULL)
		asprintf(&filename,"%s/." KKEDITVERS "/session",getenv("HOME"));
	else
		asprintf(&filename,"%s",(char*)data);

	fd=fopen(filename,"r");
	if (fd!=NULL)
		{
			if(data!=NULL)
				fgets(buffer,2048,fd);

			while(fgets(buffer,2048,fd)!=NULL)
				{
					sscanf(buffer,"%i %[^\n]s",(int*)&currentline,(char*)&strarg);
					if(openFile(strarg,currentline,true)==false)
						{
							intarg=999;
							while(intarg!=-1)
								{
									fgets(buffer,2048,fd);
									sscanf(buffer,"%i",(int*)&intarg);
								}
						}
					else
						{
							fgets(buffer,2048,fd);
							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
							page=getPageStructPtr(currentPage-1);
							gtk_notebook_set_current_page(mainNotebook,currentPage-1);
							while(intarg!=-1)
								{
									gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
									gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
									toggleBookmark(NULL,&markiter);
									fgets(buffer,2048,fd);
									sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
								}
							buf->textBuffer=(GtkTextBuffer*)page->buffer;
							buf->scroll2Line((GtkTextView*)page->view,currentline);					
						}
				}
			fclose(fd);
			ERRDATA debugFree(&filename);
		}

	ERRDATA delete buf;

	while(gtk_events_pending())
		gtk_main_iteration_do(false);
	currentTabNumber=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)-1;
	setWidgets();
	setSensitive();
	ERRDATA
	killBarberPole();
	ERRDATA
}

int showFileChanged(char* filename)
{
	ERRDATA
	GtkWidget*	dialog;
	gint		result;
	char*		message;

	ERRDATA
	asprintf(&message,gettext("File %s Has Changed on disk\nDo you want to reload it?"),filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_REFRESH,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),gettext("Warning file changed!"));

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	ERRDATA debugFree(&message);
	ERRDATA return(result);
}

void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data)
{
	ERRDATA
	pageStruct*		page=(pageStruct*)user_data;
	GtkTextIter		start;
	GtkTextIter		end;
	gchar*			buffer;
	long			filelen;
	int				answer;

	ERRDATA
	if(G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT==event_type)
		{
			if((page->itsMe==false) && (page->showingChanged==false))
				{
					page->showingChanged=true;

					if(noWarnings==false)
						answer=showFileChanged(page->fileName);
					else
						answer=GTK_RESPONSE_YES;

					page->showingChanged=false;
					if(answer==GTK_RESPONSE_YES)
						{
							g_file_get_contents(page->filePath,&buffer,(gsize*)&filelen,NULL);
							gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
							gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
							gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
							gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,buffer,filelen);
							ERRDATA debugFree(&buffer);
						}
				}
			else
				page->itsMe=false;
		}
	ERRDATA
}

void add_source_mark_pixbufs (GtkSourceView *view)
{
	ERRDATA
	GdkColor	color;
	GtkImage*	image;
	GdkPixbuf*	pbuf;

	ERRDATA
	image=(GtkImage*)gtk_image_new_from_file(DATADIR"/pixmaps/BookMark.png");
	pbuf=gtk_image_get_pixbuf(image);

	gdk_color_parse(highlightColour,&color);
//TODO//
#ifndef _USEGTK3_
	gtk_source_view_set_mark_category_background(view,MARK_TYPE_1,&color);
	gtk_source_view_set_mark_category_icon_from_pixbuf (view,MARK_TYPE_1,pbuf);
	gtk_source_view_set_mark_category_priority(view,MARK_TYPE_1,1);
#endif
	ERRDATA
}

gboolean clickInView(GtkWidget* widget,gpointer data)
{
	ERRDATA
	if(sessionBusy==true)
		{
			ERRDATA return(false);
		}

	ERRDATA
	if((statusMessage!=NULL))
		{
			ERRDATA debugFree(&statusMessage);
			statusMessage=NULL;
			
		}
	ERRDATA return(false);
}

pageStruct* makeNewPage(void)
{
	ERRDATA
	pageStruct*			page;
	GtkTextIter			iter;
	GtkTextAttributes*	attr;

	ERRDATA
	page=(pageStruct*)malloc(sizeof(pageStruct));
	page->buffer=NULL;
	page->userDataList=NULL;
	page->filePath=NULL;
	page->realFilePath=NULL;

#ifdef _USEGTK3_
	page->pane=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
#else
	page->pane=gtk_vpaned_new();
#endif
	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);
	page->pageWindow2=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);

	page->buffer=gtk_source_buffer_new(NULL);
	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

//completion
	page->completion=gtk_source_view_get_completion(GTK_SOURCE_VIEW(page->view));
	g_object_set(page->completion,"remember-info-visibility",true,NULL);
	g_object_set(page->completion,"select-on-show",true,NULL);
	g_object_set(page->completion,"show-headers",true,NULL);
	g_object_set(page->completion,"show-icons",false,NULL);
	g_object_set(page->completion,"accelerators",0,NULL);
	createCompletion(page);

	g_signal_connect(G_OBJECT(page->view),"populate-popup",G_CALLBACK(populatePopupMenu),NULL);
	page->view2=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);

	attr=gtk_text_view_get_default_attributes((GtkTextView*)page->view);
	page->highlightTag=gtk_text_buffer_create_tag((GtkTextBuffer*)page->buffer,"highlighttag","background",gdk_color_to_string((const GdkColor*)&attr->appearance.fg_color),"foreground",gdk_color_to_string((const GdkColor*)&attr->appearance.bg_color),NULL);
	gtk_text_attributes_unref(attr);

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
	page->showingChanged=false;
	page->backMark=gtk_text_mark_new("back-mark",true);
	page->regexList=NULL;
	page->regexMatchNumber=-1;

	ERRDATA
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&iter);
	gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(page->buffer),page->backMark,&iter);

//dnd
	gtk_drag_dest_set((GtkWidget*)page->view,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets((GtkWidget*)page->view);
	gtk_drag_dest_add_text_targets((GtkWidget*)page->view);
	g_signal_connect_after(G_OBJECT(page->view),"drag-data-received",G_CALLBACK(dropText),(void*)page);
	g_signal_connect(G_OBJECT(page->view),"button-press-event",G_CALLBACK(clickInView),NULL);

	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),FALSE);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive),NULL);
	gtk_widget_grab_focus((GtkWidget*)page->view);

	g_signal_connect(page->view, "line-mark-activated",G_CALLBACK(line_mark_activated),page);
	add_source_mark_pixbufs(GTK_SOURCE_VIEW(page->view));
//status bar
	g_signal_connect(G_OBJECT(page->buffer),"mark-set",G_CALLBACK(updateStatusBar),(void*)page);
	ERRDATA

	ERRDATA return(page);
}

void convertContents(char *data,int datalen)
{
	ERRDATA
 	const gchar	*charset;
	iconv_t		cd;
    size_t		len_src;
    size_t		len_dst;
	char		*startptr;

	charset=detect_charset(data);
	if (charset==NULL)
		charset=get_default_charset();

	dataLen=-1;
	len_src=datalen;
	len_dst=len_src * 8;
    cd=iconv_open("UTF-8",charset);
	convertedData=(char*)malloc(datalen*8);
	startptr=convertedData;

    iconv(cd,&data,&len_src,&startptr,&len_dst);
    iconv_close(cd);

	ERRDATA dataLen=(long)startptr-(long)convertedData;
}

VISIBLE bool openFile(const gchar *filepath,int linenumber,bool warn)
{
	ERRDATA
	GtkTextIter				startiter;
	GtkTextIter				enditer;
	GtkWidget*				label;
	gchar*					filename;
	pageStruct*				page;
	char*					str=NULL;
	char*					recenturi;
	int						linenum=linenumber-1;
	gchar*					contents=NULL;
	gsize					length;
	GError*					err=NULL;
	GtkWidget*				dialog;
	char*					filepathcopy=NULL;
	char*					tpath;
	int						whattodo;
	const gchar				*end;

	busyFlag=true;
	sessionBusy=true;

	if(readLinkFirst==true)
		filepathcopy=realpath(filepath,NULL);
	else
		filepathcopy=strdup(filepath);

	filename=g_path_get_basename(filepathcopy);

	for(int j=0; j<gtk_notebook_get_n_pages(mainNotebook); j++)
		{
			page=getPageStructPtr(j);
			if(noDuplicates==true)
				{
					tpath=realpath(filepath,NULL);
					if((tpath!=NULL) && (page->realFilePath!=NULL) && (strcmp(page->realFilePath,tpath)==0))
						{
							gtk_notebook_set_current_page(mainNotebook,j);
							gotoLine(NULL,(gpointer)(long)linenumber);
							busyFlag=false;
							sessionBusy=false;
							ERRDATA debugFree(&tpath);
							ERRDATA return(true);
						}
					ERRDATA debugFree(&tpath);
				}
		}

	if(!g_file_test(filepath,G_FILE_TEST_EXISTS))
		{
			if(warn==true)
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("File '%s' doesn't exist :("),filepathcopy);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
				}
			busyFlag=false;
			sessionBusy=false;
			ERRDATA return(false);
		}

	if(access(filepathcopy,R_OK)!=0)
		{
			if(warn==true)
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Can't open file '%s' :("),filepath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
				}
			busyFlag=false;
			sessionBusy=false;
			ERRDATA return(false);
		}

	if(linenum<0)
		linenum=0;

	page=makeNewPage();
	//page->tabVbox=gtk_vbox_new(true,4);
	page->tabVbox=creatNewBox(NEWVBOX,true,4);
	page->filePath=strdup(filepathcopy);
	page->fileName=strdup(filename);
	page->dirName=g_path_get_dirname(filepathcopy);
	ERRDATA

	page->realFilePath=realpath(filepath,NULL);

	label=NULL;
	label=makeNewTab(page->fileName,page->filePath,page);

	setLanguage(page);

	g_file_get_contents(filepathcopy,&contents,&length,&err);

	if(contents==NULL)
		{
			if(warn==true)
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Can't open file '%s' :("),filepath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
				}
			busyFlag=false;
			sessionBusy=false;
			ERRDATA return(false);
		}

	convertContents((char*)contents,length);
	ERRDATA debugFree(&contents);

	if(dataLen>0)
		{
			gtk_source_buffer_begin_not_undoable_action(page->buffer);
			gtk_text_buffer_get_start_iter ( GTK_TEXT_BUFFER (page->buffer), &startiter);
			gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &enditer);
			gtk_text_buffer_delete ( GTK_TEXT_BUFFER (page->buffer),&startiter, &enditer);
			gtk_text_buffer_get_start_iter ( GTK_TEXT_BUFFER (page->buffer), &startiter);
			whattodo=GTK_RESPONSE_YES;
			if(g_utf8_validate((const gchar*)convertedData,dataLen,&end)==false)
				whattodo=yesNo(gettext("Contains non text data, continue loading?\n"),(char*)filepath);
			if(whattodo==GTK_RESPONSE_YES)
				{
					//g_utf8_validate(convertedData,dataLen,&end);
					dataLen=(long)end-(long)convertedData;
					gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&startiter,convertedData,dataLen);
				}
			else
				{
					busyFlag=false;
					sessionBusy=false;
					gtk_widget_destroy(label);
					ERRDATA return(false);
				}
			gtk_source_buffer_end_not_undoable_action(page->buffer);
		}
	else
		{
			gtk_source_buffer_begin_not_undoable_action(page->buffer);
			gtk_text_buffer_get_start_iter ( GTK_TEXT_BUFFER (page->buffer), &startiter);
			gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &enditer);
			gtk_text_buffer_delete ( GTK_TEXT_BUFFER (page->buffer),&startiter, &enditer);
			gtk_text_buffer_get_start_iter ( GTK_TEXT_BUFFER (page->buffer), &startiter);
			gtk_source_buffer_end_not_undoable_action(page->buffer);
		}
	ERRDATA debugFree(&convertedData);

	page->gFile=g_file_new_for_path(page->filePath);
	page->monitor=g_file_monitor_file(page->gFile,(GFileMonitorFlags)G_FILE_MONITOR_NONE,NULL,NULL);
	g_signal_connect(G_OBJECT(page->monitor),"changed",G_CALLBACK(fileChangedOnDisk),(void*)page);

	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);

	str=g_file_get_path(page->gFile);
	recenturi=g_filename_to_uri(str,NULL,NULL);
	gtk_recent_manager_add_item(gtk_recent_manager_get_default(),recenturi);
	ERRDATA debugFree(&filename);
	ERRDATA debugFree(&recenturi);
	ERRDATA debugFree(&str);
	ERRDATA debugFree(&filepathcopy);

//connect to mainNotebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pane));
	g_object_set_data(G_OBJECT(page->tabVbox),"pagedata",(gpointer)page);

	if(openInThisTab==-1)
		currentPage=gtk_notebook_append_page(mainNotebook,page->tabVbox,label);
	else
		currentPage=gtk_notebook_insert_page(mainNotebook,page->tabVbox,label,openInThisTab+1);

	gtk_notebook_set_tab_reorderable(mainNotebook,page->tabVbox,true);
	gtk_notebook_set_current_page(mainNotebook,currentPage);

	if(openInThisTab==-1)
		currentPage++;

	gtk_widget_grab_focus((GtkWidget*)page->view);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);
	gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);

	gtk_widget_show_all((GtkWidget*)mainNotebook);
	setToobarSensitive();

	setFilePrefs(page);

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"openFile");

	/* move cursor to the linenumber */
	if(fromGOpen==true)
		{
			gtk_widget_show_all((GtkWidget*)(GtkTextView*)page->view);
			if(linenum>0)
				while (gtk_events_pending ())
					gtk_main_iteration ();
		}

	gotoLine(NULL,(void*)(long)(linenum+1));

	busyFlag=false;
	sessionBusy=false;
	ERRDATA return(TRUE);
}

VISIBLE void newFile(GtkWidget* widget,gpointer data)
{
	ERRDATA
	GtkTextIter	iter;
	GtkWidget*	label;
	pageStruct*	page;

	ERRDATA
	page=makeNewPage();
	//page->tabVbox=gtk_vbox_new(true,4);
	page->tabVbox=creatNewBox(NEWVBOX,true,4);
	page->filePath=NULL;
	page->dirName=NULL;

	asprintf(&page->fileName,"%s-%i",gettext("Untitled"),untitledNumber);
	untitledNumber++;

	label=makeNewTab(page->fileName,NULL,page);
	gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);

	/* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);

//connect to ntebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pane));
	g_object_set_data(G_OBJECT(page->tabVbox),"pagedata",(gpointer)page);

	gtk_notebook_append_page(mainNotebook,page->tabVbox,label);
	gtk_notebook_set_tab_reorderable(mainNotebook,page->tabVbox,true);
	gtk_notebook_set_current_page(mainNotebook,currentPage);
	setToobarSensitive();
	currentPage++;
	gtk_widget_show_all((GtkWidget*)mainNotebook);
	setFilePrefs(page);

	ERRDATA
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newFile");
	ERRDATA
}
