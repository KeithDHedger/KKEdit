/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * files.cpp
 *
*/

#include "kkedit-includes.h"


#ifndef _USEQT5_
GtkWidget*	vbox;
#else
//TODO//
#endif

char*		saveFileName=NULL;
char*		saveFilePath=NULL;
bool		dropTextFile=false;

VISIBLE void saveVarsToFile(char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	int		cnt=0;

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
						}
					cnt++;
				}
			fclose(fd);
		}
}

VISIBLE void loadVarsFromFile(char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	argname=NULL;
	char*	strarg=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					sscanf(buffer,"%as %as",&argname,&strarg);
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
													free(*(char**)(dataptr[cnt].data));
												sscanf(buffer,"%*s %a[^\n]s",(char**)dataptr[cnt].data);
												break;
											case TYPEBOOL:
												*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
												break;
										}
								}
							cnt++;
						}
					if(argname!=NULL)
						free(argname);
					if(strarg!=NULL)
						free(strarg);
					argname=NULL;
					strarg=NULL;
				}
			fclose(fd);
		}
}

#ifndef _USEQT5_
GtkWidget* makeNewTab(char* name,char* tooltip,pageStruct* page)
#else
//TODO//
QWidget* makeNewTab(char* name,char* tooltip,pageStruct* page)
#endif
{
#ifdef _USEQT5_
//	char*	correctedname;
//
//	printf(">%s<\n",name);
//
//	correctedname=strdup(truncateWithElipses(name,maxTabChars));
//	page->tabName=truncateWithElipses(name,maxTabChars);
//	printf(">%s<\n",correctedname);
	return(NULL);
#else
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

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newTab");

	return(evbox);
#endif
}

void setFilePrefs(pageStruct* page)
{
#ifndef _USEQT5_
	PangoFontDescription*	font_desc;
	GdkColor				color;
	GtkTextAttributes*		attr;

	gtk_source_view_set_auto_indent(page->view,indent);
	gtk_source_view_set_show_line_numbers(page->view,lineNumbers);
	gtk_source_view_set_highlight_current_line(page->view,highLight);
	gtk_source_view_set_show_line_marks(page->view,showBMBar);
	gdk_color_parse(highlightColour,&color);
	gtk_source_view_set_mark_category_background(page->view,MARK_TYPE_1,&color);

	if(lineWrap==true)
		gtk_text_view_set_wrap_mode((GtkTextView *)page->view,GTK_WRAP_WORD_CHAR);
	else
		gtk_text_view_set_wrap_mode((GtkTextView *)page->view,GTK_WRAP_NONE);

	gtk_source_view_set_tab_width(page->view,tabWidth);

	font_desc=pango_font_description_from_string(fontAndSize);
	gtk_widget_modify_font((GtkWidget*)page->view,font_desc);
	pango_font_description_free(font_desc);

	attr=gtk_text_view_get_default_attributes((GtkTextView*)page->view);
	g_object_set((gpointer)page->highlightTag,"background",gdk_color_to_string((const GdkColor*)&attr->appearance.fg_color),"foreground",gdk_color_to_string((const GdkColor*)&attr->appearance.bg_color),NULL);
	gtk_text_attributes_unref(attr);

	if(noSyntax==true)
		gtk_source_buffer_set_highlight_syntax (page->buffer,false);
	else
		gtk_source_buffer_set_highlight_syntax (page->buffer,true);

	if(autoShowComps==true)
		gtk_source_completion_unblock_interactive(page->completion);
	else
		gtk_source_completion_block_interactive(page->completion);
createCompletion(page);
#endif
}

void resetAllFilePrefs(void)
{
#ifndef _USEQT5_
	pageStruct*			page;

	styleScheme=gtk_source_style_scheme_manager_get_scheme(schemeManager,styleName);

	for(int loop=0; loop<gtk_notebook_get_n_pages(mainNotebook); loop++)
		{
			page=getPageStructPtr(loop);
			gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);
			setFilePrefs(page);
		}
#endif
}

#ifndef _USEQT5_
void dropText(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
#else
//TODO//
void dropText(void)
#endif
{
#ifndef _USEQT5_
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

					debugFree(&command,"dropText command");
					g_string_free(str,true);
					pclose(fp);
				}

			g_strfreev(array);
		}
	else
		dropTextFile=false;

	gtk_drag_finish (context,true,true,time);
#endif
}

bool getSaveFile(void)
{
#ifndef _USEQT5_
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
	return(retval);
#endif
}

VISIBLE bool saveFile(Widget* widget,uPtr data)
//TODO//
{
printf("save %i\n",(int)(long)data);
#ifndef _USEQT5_
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
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Can't save file '%s' :("),page->filePath);
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
					if(page->dirName!=NULL)
						debugFree(&page->dirName,"saveFile dirName");
					page->dirName=g_path_get_dirname(page->filePath);
				}

			saveFileName=page->fileName;
			if(getSaveFile()==false)
				return(false);

			fd=fopen(saveFilePath,"w");
			if (fd!=NULL)
				{
					page->filePath=saveFilePath;
					page->fileName=saveFileName;
					if(page->dirName!=NULL)
						debugFree(&page->dirName,"saveFile dirName");
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
					return(false);
				}

			saveFileName=NULL;
			saveFilePath=NULL;
		}
	if(page->lang==NULL)
		setLanguage(page);
	switchPage(mainNotebook,page->tabVbox,currentTabNumber,NULL);
	setSensitive();

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"saveFile");

	return(true);
#endif
}

VISIBLE void openAsHexDump(Widget *widget,uPtr user_data)
//TODO//
{
printf("triggered openAsHexDump %i\n",(int)(long)user_data);

#ifndef _USEQT5_
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
					debugFree(&convstr,"openAsHexDump convstr");
				}
			else
				{
					gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,str->str,-1);
				}
			gtk_source_buffer_end_not_undoable_action(page->buffer);
			gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);

			g_string_free(str,true);
			debugFree(&filepath,"openAsHexDump filepath");
			debugFree(&filename,"openAsHexDump filename");
			setSensitive();
		}

	gtk_widget_destroy (dialog);
	refreshMainWindow();
#endif
}

VISIBLE void reloadFile(Widget* widget,uPtr data)
//TODO//
{
printf("reloadFile %i\n",(int)(long)data);

#ifndef _USEQT5_
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
			debugFree(&buffer,"reloadFile buffer");
		}
#endif
}

VISIBLE void saveSession(Widget* widget,uPtr data)
//TODO//
{
printf("saveSession %i\n",(int)(long)data);
#ifndef _USEQT5_
	pageStruct*		page;
	FILE*			fd=NULL;
	char*			filename;
	GtkTextMark*	mark;
	GtkTextIter		iter;
	int				linenumber;
	GtkTextIter		markiter;
	GList*			ptr;

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	debugFree(&filename,"saveSession filename");
	asprintf(&filename,"%s/.KKEdit/session",getenv("HOME"));
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

			fclose(fd);
			debugFree(&filename,"saveSession filename");
		}
#endif
}

VISIBLE void restoreSession(Widget* widget,uPtr data)
//TODO//
{
printf("restoreSession %i\n",(int)(long)data);
#ifndef _USEQT5_
	FILE*		fd=NULL;
	char*		filename;
	char		buffer[2048];
	int			intarg;
	char		strarg[2048];
	pageStruct*	page;
	GtkTextIter	markiter;
	int			currentline;
	TextBuffer*	buf=new TextBuffer;

	asprintf(&filename,"%s/.KKEdit/session",getenv("HOME"));
	fd=fopen(filename,"r");
	if (fd!=NULL)
		{
			closeAllTabs(NULL,NULL);
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
									if((bool)data==true)
										{
											gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
											gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
											toggleBookmark(NULL,&markiter);
										}
									fgets(buffer,2048,fd);
									sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
								}
							buf->textBuffer=(GtkTextBuffer*)page->buffer;
							buf->scroll2Line((GtkTextView*)page->view,currentline);					
						}
				}
			fclose(fd);
			debugFree(&filename,"restoreSession filename");
		}
	delete buf;
#endif
}

int showFileChanged(char* filename)
{
#ifndef _USEQT5_
	GtkWidget*	dialog;
	gint		result;
	char*		message;

	asprintf(&message,gettext("File %s Has Changed on disk\nDo you want to reload it?"),filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_REFRESH,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),gettext("Warning file changed!"));

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	debugFree(&message,"showFileChanged message");
	return(result);
#endif
}

#ifndef _USEQT5_
void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data)
#else
//TODO
void fileChangedOnDisk(void)
#endif
{
#ifndef _USEQT5_
	pageStruct*		page=(pageStruct*)user_data;
	GtkTextIter		start;
	GtkTextIter		end;
	gchar*			buffer;
	long			filelen;
	int				answer;

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
							debugFree(&buffer,"fileChangedOnDisk buffer");
						}
				}
			else
				page->itsMe=false;
		}
#endif
}

#ifndef _USEQT5_
void add_source_mark_pixbufs (GtkSourceView *view)
#else
//TODO//
void add_source_mark_pixbufs (void)
#endif
{
#ifndef _USEQT5_
	GdkColor	color;
	GtkImage*	image;
	GdkPixbuf*	pbuf;

	image=(GtkImage*)gtk_image_new_from_file(DATADIR"/pixmaps/BookMark.png");
	pbuf=gtk_image_get_pixbuf(image);

	gdk_color_parse(highlightColour,&color);
	gtk_source_view_set_mark_category_background(view,MARK_TYPE_1,&color);
	gtk_source_view_set_mark_category_icon_from_pixbuf (view,MARK_TYPE_1,pbuf);
	gtk_source_view_set_mark_category_priority(view,MARK_TYPE_1,1);
#endif
}

#ifndef _USEQT5_
gboolean clickInView(GtkWidget* widget,gpointer data)
#else
//TODO//
gboolean clickInView(void)
#endif
{
#ifndef _USEQT5_
	if((statusMessage!=NULL))
		{
			free(statusMessage);
			statusMessage=NULL;
			
		}
	return(false);
#endif
}

pageStruct* makeNewPage(void)
{
#ifdef _USEQT5_
	pageStruct*			page;
	page=(pageStruct*)malloc(sizeof(pageStruct));
	return(page);
#else
	pageStruct*			page;
	GtkTextIter			iter;
	GtkTextAttributes*	attr;

	page=(pageStruct*)malloc(sizeof(pageStruct));
	page->buffer=NULL;
	page->userDataList=NULL;
	page->filePath=NULL;

	page->pane=gtk_vpaned_new();
	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	page->pageWindow2=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

	page->buffer=gtk_source_buffer_new(NULL);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(setSensitive),NULL);
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
	g_signal_connect(G_OBJECT(page->buffer),"mark-set",G_CALLBACK(updateStatuBar),(void*)page);

	return(page);
#endif
}

/*
void MainWindow::loadFile(const QString &fileName)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

*/

void fileErrorMessages(char *message,const gchar *filepath)
{
#ifdef _USEQT5_
	QMessageBox::warning(mainWindow,"File I/O Error",QString("%1 %2").arg(message).arg(filepath));
#endif
}

char* getFileContents(const gchar *filepath,bool warn)
{
	struct stat	sb;
	char*		linkname=NULL;
	ssize_t		r;
	char		*filepathcopy=NULL;
	char		*str=NULL;
	gchar		*contents=NULL;
	gsize		length;
	GError		*err=NULL;
	const gchar	*charset;
	gsize		br;
	bool		ret=false;

//const char* filepath="/tmp/çÇğĞıİöÖşŞüÜ-copy2";
//char* contents="XXX";
//const gchar	*charset;

//charset=detect_charset(filepath);
//printf("charset >%s<n",charset);
//g_file_get_contents(filepath,&contents,NULL,NULL);
//printf("%sn",contents);

//
//	charset=detect_charset(filepath);
//printf("charset >%s<\n",charset);
//
//char* nf=g_convert(filepath,-1,"UTF-8",charset,NULL,&br,&err);
//printf(">>%s<<\n",nf);
//printf("<<>>%s<<>>\n",g_filename_from_uri(filepath, NULL, NULL));
//printf("<<>>%s<<>>\n",g_filename_from_uri(nf, NULL, NULL));
//	ret=g_file_get_contents(g_filename_from_uri(filepath, NULL, NULL),&contents,&length,&err);
//	
//printf(">%s<\n",contents);
//return(NULL);
	if(readLinkFirst==true)
		filepathcopy=realpath(filepath,NULL);
	else
		filepathcopy=strdup(filepath);

	if(!g_file_test(filepath,G_FILE_TEST_EXISTS))
		{
			if(warn==true)
				fileErrorMessages("File doesn't exist",filepath);

			busyFlag=false;
			sessionBusy=false;
			return(NULL);
		}

	if(access(filepathcopy,R_OK)!=0)
		{
			if(warn==true)
				fileErrorMessages("Can't acsess file",filepath);
			busyFlag=false;
			sessionBusy=false;
			return(NULL);
		}

	ret=g_file_get_contents(filepathcopy,&contents,&length,&err);
		
	if(ret==false)
		{
			if(warn==true)
				fileErrorMessages("Can't open file",filepath);
			busyFlag=false;
			sessionBusy=false;
			return(NULL);
		}

	charset=detect_charset(contents);
	if (charset==NULL)
		charset=get_default_charset();

	br=length;
	if(length)
		do
			{
				if(err)
					{
						charset="ISO-8859-1";
						g_error_free(err);
						err=NULL;
					}
				str=g_convert(contents,br,"UTF-8",charset,NULL,&br,&err);
			}
		while(err);
	else
		{
			str=NULL;
		}

	debugFree(&contents,"openFile contents");
	return(str);
}

VISIBLE bool openFile(const gchar *filepath,int linenumber,bool warn)
{
printf("openfile %s\n",filepath);
#ifdef _USEQT5_
	DocumentClass	*doc;
	QFile 			file;
	char			*tstr;
	char			*realfilepath;
	int				tabnum;
	char*			filedata=NULL;

//	realfilepath=realpath(filepath,NULL);
//	//tstr=QString(QFile::decodeName(realfilepath)).toLocal8Bit();
//	//QString str1 = QString(QFile::decodeName(realfilepath));
//	printf(">>%s<<n",filepath);
//	printf(">>%s<<n",realfilepath);
//	QString str1(QFile::decodeName(filepath));
//	qDebug() << "abc" << str1 << "def";
//      QByteArray ba = str1.toLocal8Bit();
//      const char *c_str2 = ba.data();
//      printf("str2: %sn", c_str2);
////	printf(">%s<n",(char*)());
//
//
//	file.setFileName(c_str2);
//	if (!file.open(QFile::ReadOnly | QIODevice::Text))
//		{
//			QMessageBox::warning(mainWindow,"Open File",QString("Cannot read file %1").arg(filepath));
//			return(false);
//		}
//
	doc=new DocumentClass();
//	QTextStream in(&file);
//	in.setAutoDetectUnicode(false);
//	in.setCodec("UTF-8");
//
//	doc->setPlainText(in.readAll());

	filedata=getFileContents(filepath,true);
	if(filedata!=NULL)
		{
			doc->setPlainText(filedata);
			realfilepath=realpath(filepath,NULL);
			doc->setRealpath(realfilepath);
			tstr=strdup(filepath);
			doc->setDirname(strdup(dirname(tstr)));
			debugFree(&tstr,"openFile tstr");
			tstr=strdup(filepath);
			doc->setPathname(strdup(filepath));
			doc->setFilename(strdup(basename(tstr)));
			
const gchar *charset=detect_charset(filepath);
printf("%s\n",charset);
const char* cn=g_convert(filepath,-1,"UTF-8",charset,NULL,NULL,NULL);
printf(">%s<\n",cn);
			doc->setTabname(truncateWithElipses((char*)QString(filepath).toUtf8().constData(),maxTabChars));

			tabnum=((QTabWidget*)mainNotebook)->addTab(doc,doc->getTabname());
			((QTabWidget*)mainNotebook)->setTabToolTip(tabnum,doc->getPathname());
			((QTabWidget*)mainNotebook)->setCurrentIndex(tabnum);
			debugFree(&tstr,"openFile tstr 2");
		}
#else
	GtkTextIter				iter;
	GtkWidget*				label;
	gchar*					filename=g_path_get_basename(filepath);
	pageStruct*				page;
	char*					str=NULL;
	char*					recenturi;
	int						linenum=linenumber-1;
	gchar*					contents=NULL;
	gsize					length;
	GError*					err=NULL;
	const gchar*			charset;
	gsize					br;
	GRegex*					regex;
	GRegexCompileFlags		compileflags=(GRegexCompileFlags)(G_REGEX_MULTILINE|G_REGEX_EXTENDED|G_REGEX_CASELESS);
	GRegexMatchFlags		matchflags=(GRegexMatchFlags)(G_REGEX_MATCH_NOTBOL|G_REGEX_MATCH_NOTEOL);
	GtkWidget*				dialog;
	char*					searchtext=NULL;
	char*					replacetext=NULL;
	struct stat				sb;
	char*					linkname=NULL;
	ssize_t					r;
	char*					filepathcopy=NULL;

	filepathcopy=strdup(filepath);

	lstat(filepath,&sb);
	if(S_ISLNK(sb.st_mode))
		{
			linkname=(char*)malloc(sb.st_size + 1);
			r=readlink(filepathcopy,linkname,sb.st_size+1);
			linkname[r]=0;
		}

	if((readLinkFirst==true) && (S_ISLNK(sb.st_mode)))
		{
			debugFree(&filepathcopy,"openFile filepathcopy");
			filepathcopy=strdup(linkname);
		}


	for(int j=0; j<gtk_notebook_get_n_pages(mainNotebook); j++)
		{
			page=getPageStructPtr(j);
			if(noDuplicates==true)
				{
					if((page->realFilePath!=NULL) && ((strcmp(page->realFilePath,filepathcopy)==0) ||(strcmp(page->filePath,filepathcopy)==0)))
						{
							gtk_notebook_set_current_page(mainNotebook,j);
							return(true);
						}
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
			return(false);
		}

	if(access(filepathcopy,R_OK)!=0)
		{
			if(warn==true)
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Can't open file '%s' :("),filepath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
				}
			return(false);
		}

	if(linenum<0)
		linenum=0;

	page=makeNewPage();
	page->tabVbox=gtk_vbox_new(true,4);

	page->filePath=strdup(filepathcopy);
	page->fileName=strdup(filename);
	page->dirName=g_path_get_dirname(filepathcopy);

	if(S_ISLNK(sb.st_mode))
		page->realFilePath=strdup(linkname);
	else
		page->realFilePath=(char*)strdup(filepath);

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
			return(false);
		}

	charset=detect_charset(contents);
	if (charset==NULL)
		charset=get_default_charset();

	br=length;

	if(length)
		do
			{
				if(err)
					{
						charset="ISO-8859-1";
						g_error_free(err);
						err=NULL;
					}
				str=g_convert(contents,br,"UTF-8",charset,NULL,&br,&err);
			}
		while(err);
	else
		{
			str=g_strdup("");
		}

	debugFree(&contents,"openFile contents");

	searchtext=(char*)"\\00";
	replacetext=(char*)"";

	regex=g_regex_new(searchtext,(GRegexCompileFlags)compileflags,matchflags,NULL);
	str=g_regex_replace(regex,str,br,0,replacetext,matchflags,NULL);

	gtk_source_buffer_begin_not_undoable_action(page->buffer);
	gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (page->buffer), &iter);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&iter,str,strlen(str));
	debugFree(&str,"openFile str");
	gtk_source_buffer_end_not_undoable_action(page->buffer);

	page->gFile=g_file_new_for_path(page->filePath);
	page->monitor=g_file_monitor(page->gFile,(GFileMonitorFlags)G_FILE_MONITOR_NONE,NULL,NULL);
	g_signal_connect(G_OBJECT(page->monitor),"changed",G_CALLBACK(fileChangedOnDisk),(void*)page);

	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);

	str=g_file_get_path(page->gFile);
	recenturi=g_filename_to_uri(str,NULL,NULL);
	gtk_recent_manager_add_item(gtk_recent_manager_get_default(),recenturi);
	debugFree(&filename,"openFile filename");
	debugFree(&recenturi,"openFile recenturi");
	debugFree(&str,"openFile str");
	debugFree(&filepathcopy,"openFile filepathcopy");
	debugFree(&linkname,"openFile linkname");

//connect to ntebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pane));
	g_object_set_data(G_OBJECT(page->tabVbox),"pagedata",(gpointer)page);

	gtk_notebook_append_page(mainNotebook,page->tabVbox,label);
	gtk_notebook_set_tab_reorderable(mainNotebook,page->tabVbox,true);
	gtk_notebook_set_current_page(mainNotebook,currentPage);
	currentPage++;
	gtk_widget_grab_focus((GtkWidget*)page->view);

	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);

	gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);

	gtk_widget_show_all((GtkWidget*)mainNotebook);

	setToobarSensitive();

	setFilePrefs(page);

	/* move cursor to the linenumber */
	gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,linenum,0);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
	gtk_text_iter_set_line(&iter,linenum);
	gtk_text_buffer_move_mark((GtkTextBuffer*)page->buffer,page->backMark,&iter);
	gtk_text_view_scroll_to_mark((GtkTextView*)page->view,page->backMark,0,true,0,0.5);

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"openFile");

	return TRUE;
#endif
}

//TODO//
VISIBLE void newFile(Widget* widget,uPtr data)
{
printf("triggered newfile id %i\n",data);
#ifdef _USEQT5_

	DocumentClass*	doc;
	char*			filename;

	doc=new DocumentClass();
	asprintf(&filename,"%s-%i",gettext("Untitled"),untitledNumber);
	untitledNumber++;
	doc->setFilename(filename);
	((QTabWidget*)mainNotebook)->addTab(doc,doc->getFilename());
	
#else
	GtkTextIter	iter;
	GtkWidget*	label;
	pageStruct*	page;

	page=makeNewPage();
	page->tabVbox=gtk_vbox_new(true,4);
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

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newFile");
#endif
}
