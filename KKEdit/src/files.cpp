/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:49:15 GMT 2015 kdhedger68713@gmail.com

 * This file(files.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not,see <http://www.gnu.org/licenses/>.
 */

#include "kkedit-includes.h"
#include <iconv.h>
#ifdef _USEGTK3_
#include <gtksourceview/gtksource.h>
#endif

GtkWidget	*vbox;
char		*saveFileName=NULL;
char		*saveFilePath=NULL;
bool		dropTextFile=false;
char		*convertedData=NULL;
long		dataLen=0;

VISIBLE void saveVarsToFile(char *filepath,args *dataptr)
{
	ERRDATA
	FILE	*fd=NULL;
	int		cnt=0;
	GSList	*list=NULL;

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

VISIBLE int loadVarsFromFile(char *filepath,args *dataptr)
{
	ERRDATA
	FILE		*fd=NULL;
	char		buffer[2048];
	int			cnt;
	char		*argname=NULL;
	char		*strarg=NULL;
	char		*liststr=NULL;
	int			retval=NOERR;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					sinkReturnStr=fgets(buffer,2048,fd);
					sscanf(buffer,"%ms %ms",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) &&(argname!=NULL) &&(strcmp(argname,dataptr[cnt].name)==0))
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
	else
		{
			retval=NOOPENFILE;
		}

	return(retval);
}

GtkWidget *makeNewTab(char *name,char *tooltip,pageStruct *page)
{
	ERRDATA
	GtkWidget	*evbox=gtk_event_box_new();
	GtkWidget	*hbox;
	GtkWidget	*label;
	GtkWidget	*close=gtk_image_new_from_icon_name(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);

	GtkWidget	*button=gtk_button_new();

	char		*correctedname;

	hbox=createNewBox(NEWHBOX,false,0);
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
	page->pageID=pageID++;
	page->tabButton=button;
#ifdef _USEGTK3_
	applyCSS(button,tabBoxProvider);
	gtk_style_context_reset_widgets(gdk_screen_get_default());
#else
	GtkRcStyle	*style=gtk_rc_style_new();
	style->xthickness=style->ythickness=tabsSize;
	gtk_widget_modify_style(button,style);
	g_object_unref(G_OBJECT(style));
#endif
	gtk_widget_show_all(evbox);

	ERRDATA
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newTab");

	ERRDATA return(evbox);
}

#if 0

const char *resetcss=" \
* { \
  color: inherit; \
  font-size: inherit; \
  background-color: initial; \
  font-family: inherit; \
  font-style: inherit; \
  font-variant: inherit; \
  font-weight: inherit; \
  text-shadow: inherit; \
  icon-shadow: inherit; \
  box-shadow: initial; \
  margin-top: initial; \
  margin-left: initial; \
  margin-bottom: initial; \
  margin-right: initial; \
  padding-top: initial; \
  padding-left: initial; \
  padding-bottom: initial; \
  padding-right: initial; \
  border-top-style: initial; \
  border-top-width: initial; \
  border-left-style: initial; \
  border-left-width: initial; \
  border-bottom-style: initial; \
  border-bottom-width: initial; \
  border-right-style: initial; \
  border-right-width: initial; \
  border-top-left-radius: initial; \
  border-top-right-radius: initial; \
  border-bottom-right-radius: initial; \
  border-bottom-left-radius: initial; \
  outline-style: initial; \
  outline-width: initial; \
  outline-offset: initial; \
  background-clip: initial; \
  background-origin: initial; \
  background-size: initial; \
  background-position: initial; \
  border-top-color: initial; \
  border-right-color: initial; \
  border-bottom-color: initial; \
  border-left-color: initial; \
  outline-color:  initial; \
  background-repeat: initial; \
  background-image: initial; \
  border-image-source: initial; \
  border-image-repeat: initial; \
  border-image-slice: initial; \
  border-image-width: initial; \
  transition-property: initial; \
  transition-duration: initial; \
  transition-timing-function: initial; \
  transition-delay: initial; \
  engine: initial; \
  gtk-key-bindings: initial; \
 \
  -GtkWidget-focus-line-width: 0; \
  -GtkWidget-focus-padding: 0; \
  -GtkNotebook-initial-gap: 0; \
} \
 \
* { \
  color: green; \
  font-family: Monospace; \
  border: 100px solid; \
} \
 \
:selected { \
  background-color: darkGreen; \
  color: black; \
} \
 \
";

#endif

void setFilePrefs(pageStruct *page)
{
	ERRDATA
	PangoFontDescription	*font_desc;
	GtkTextAttributes		*attr;

	ERRDATA
	gtk_source_view_set_auto_indent(page->view,indent);
	gtk_source_view_set_show_line_numbers(page->view,lineNumbers);
	gtk_source_view_set_highlight_current_line(page->view,highLight);
	gtk_source_view_set_show_line_marks(page->view,showBMBar);
	gtk_source_view_set_draw_spaces(page->view,(GtkSourceDrawSpacesFlags)(showWhiteSpace * GTK_SOURCE_DRAW_SPACES_ALL));

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	if(lineWrap==true)
		gtk_text_view_set_wrap_mode((GtkTextView *)page->view,GTK_WRAP_WORD_CHAR);
	else
		gtk_text_view_set_wrap_mode((GtkTextView *)page->view,GTK_WRAP_NONE);

	gtk_source_view_set_tab_width(page->view,tabWidth);

	font_desc=pango_font_description_from_string(fontAndSize);

	attr=gtk_text_view_get_default_attributes((GtkTextView*)page->view);

#ifdef _USEGTK3_
//hack for gtk3 again!
	GdkRGBA	bcol;
	GdkRGBA	fcol;

	bcol.red=(double)attr->appearance.fg_color.red / 65535.0 ;
	bcol.green=(gdouble)attr->appearance.fg_color.green / 65535.0;
	bcol.blue=(gdouble)attr->appearance.fg_color.blue / 65535.0;
	bcol.alpha=1.0;

	fcol.red=(gdouble)attr->appearance.bg_color.red / 65535.0 ;
	fcol.green=(gdouble)attr->appearance.bg_color.green / 65535.0;
	fcol.blue=(gdouble)attr->appearance.bg_color.blue / 65535.0;
	fcol.alpha=1.0;

	g_object_set((gpointer)page->highlightTag,"background",gdk_rgba_to_string(&bcol),"foreground",gdk_rgba_to_string(&fcol),NULL);
#else
	g_object_set((gpointer)page->highlightTag,"background",gdk_color_to_string((const GdkColor*)&attr->appearance.fg_color),"foreground",gdk_color_to_string((const GdkColor*)&attr->appearance.bg_color),NULL);
#endif
	gtk_text_attributes_unref(attr);

	if(noSyntax==true)
		gtk_source_buffer_set_highlight_syntax(page->buffer,false);
	else
		gtk_source_buffer_set_highlight_syntax(page->buffer,true);

#ifndef _USEGTK3_
	if(autoShowComps==true)
		gtk_source_completion_unblock_interactive(page->completion);
	else
		gtk_source_completion_block_interactive(page->completion);
#endif

	ERRDATA	createCompletion(page);

//set font
#ifdef _USEGTK3_	
	char	*fontcss=NULL;

	sinkReturn=asprintf(&fontcss,"* {\n \
  font-family: %s;\n \
  font-size: %ipx;\n \
}\n",pango_font_description_get_family(font_desc),pango_font_description_get_size(font_desc)/PANGO_SCALE);

	gtk_css_provider_load_from_data((GtkCssProvider*)provider,fontcss,-1,NULL);
	gtk_style_context_reset_widgets(gdk_screen_get_default());
	debugFree(&fontcss);
//set tabs size gtk3
	applyCSS(page->tabButton,tabBoxProvider);
	gtk_style_context_reset_widgets(gdk_screen_get_default());
#else
	gtk_widget_modify_font((GtkWidget*)page->view,font_desc);
//set tabs size gtk2
	GtkRcStyle	*style=gtk_rc_style_new();
	style->xthickness=style->ythickness=tabsSize;
	gtk_widget_modify_style(page->tabButton,style);
	g_object_unref(G_OBJECT(style));
#endif

	pango_font_description_free(font_desc);
}

void resetAllFilePrefs(void)
{
	ERRDATA
	pageStruct			*page;

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
	char			*filename;
	FILE			*fp;
	char			*command;
	GString		*str;
	char			line[1024];
	pageStruct		*page=(pageStruct*)user_data;
	GtkTextIter		iter;
	GtkTextMark	*mark;

	ERRDATA
	array=gtk_selection_data_get_uris(selection_data);
	if(array==NULL)
		{
			gtk_drag_finish(context,true,true,time);
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
					sinkReturn=asprintf(&command,"cat %s",filename);
					fp=popen(command,"r");
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
	gtk_drag_finish(context,true,true,time);
	ERRDATA
}

bool getSaveFile(void)
{
	ERRDATA
	GtkWidget	*dialog;
	bool		retval=false;

#ifdef _USEGTK3_
	dialog=gtk_file_chooser_dialog_new(SAVE_TT_LABEL,(GtkWindow*)mainWindow,GTK_FILE_CHOOSER_ACTION_SAVE,CANCEL_LABEL,GTK_RESPONSE_CANCEL,MENU_SAVE_LABEL,GTK_RESPONSE_ACCEPT,NULL);
#else
	dialog=gtk_file_chooser_dialog_new(SAVE_TT_LABEL,(GtkWindow*)mainWindow,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);
#endif

	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),TRUE);
	if(saveFileName!=NULL)
		{
			if(saveFilePath!=NULL)
				gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),g_path_get_dirname(saveFilePath));
			gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),saveFileName);
		}
	else
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),UNTITLED_LABEL);

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

VISIBLE bool saveFile(GtkWidget *widget,gpointer data)
{
	ERRDATA
	pageStruct	*page=getPageStructPtr(-1);
	GtkTextIter	start,end;
	gchar		*text;
	FILE		*fd=NULL;
	GtkWidget	*dialog;

	ERRDATA
	page->itsMe=true;
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
	text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,FALSE);
	if(page->filePath!=NULL && data==NULL)
		{
			fd=fopen(page->filePath,"w");
			if(fd!=NULL)
				{
					fputs(text,fd);
					fclose(fd);
					gtk_text_buffer_set_modified((GtkTextBuffer*)page->buffer,false);
				}
			else
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,CANT_SAVE_LABEL,page->filePath);
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
			if(fd!=NULL)
				{
					page->filePath=saveFilePath;
					page->fileName=saveFileName;
					if(page->dirName!=NULL)
						{
							ERRDATA debugFree(&page->dirName);
						}
					page->dirName=g_path_get_dirname(page->filePath);

					gtk_text_buffer_set_modified((GtkTextBuffer*)page->buffer,FALSE);

					gtk_widget_set_tooltip_text(page->tabName,page->filePath);
					gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)saveFileName);

					fprintf(fd,"%s",text);
					fclose(fd);
				}
			else
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,CANT_SAVE_LABEL,page->filePath);
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
	setChangedSensitive((GtkTextBuffer*)page->buffer,page);
	ERRDATA
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"saveFile");

	ERRDATA return(true);
}

VISIBLE void openAsHexDump(GtkWidget *widget,gpointer user_data)
{
	ERRDATA
	GtkWidget		*dialog;
	char			*filepath;
	char			*filename;
	int				pagenum;
	FILE			*fp;
	char			line[1024];
	GString		*str=g_string_new(NULL);
	char			*command;
	GtkTextIter		iter;
	pageStruct		*page;
	char			*convstr=NULL;

	ERRDATA
	dialog=gtk_file_chooser_dialog_new(OPEN_TT_LABEL,NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
		{
			filepath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			filename=g_path_get_basename(filepath);
			newFile(NULL,NULL);
			pagenum=currentPage-1;
			page=getPageStructPtr(pagenum);
			sinkReturn=asprintf(&command,"hexdump -C %s",filepath);
			fp=popen(command,"r");
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
			setChangedSensitive((GtkTextBuffer*)page->buffer,page);
		}

	gtk_widget_destroy(dialog);
	refreshMainWindow();
	ERRDATA
}

VISIBLE void reloadFile(GtkWidget *widget,gpointer data)
{
	ERRDATA
	pageStruct	*page=getPageStructPtr(-1);
	gchar		*buffer;
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

VISIBLE void saveSession(GtkWidget *widget,gpointer data)
{
	ERRDATA
	pageStruct		*page;
	FILE			*fd=NULL;
	char			*filename;
	GtkTextMark	*mark;
	GtkTextIter		iter;
	int				linenumber;
	GtkTextIter		markiter;
	GList			*ptr;

	ERRDATA
	sinkReturn=asprintf(&filename,"%s/%s",getenv("HOME"),APPFOLDENAME);
	g_mkdir_with_parents(filename,493);
	ERRDATA debugFree(&filename);
	sinkReturn=asprintf(&filename,"%s/%s/session",getenv("HOME"),APPFOLDENAME);
	fd=fopen(filename,"w");
	if(fd!=NULL)
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

VISIBLE void restoreSession(GtkWidget *widget,gpointer data)
{
	ERRDATA
	FILE			*fd=NULL;
	char			*filename;
	char			buffer[2048];
	int				intarg;
	char			strarg[2048];
	pageStruct		*page=NULL;
	GtkTextIter		markiter;
	int				currentline;
	TextBuffer		*buf=new TextBuffer;

	ERRDATA

	loadingSession=true;
	closeAllTabs(NULL,NULL);
	showBarberPole(DIALOG_POLE_RESTORING);

	if(data==NULL)
		sinkReturn=asprintf(&filename,"%s/%s/session",getenv("HOME"),APPFOLDENAME);
	else
		sinkReturn=asprintf(&filename,"%s",(char*)data);

	fd=fopen(filename,"r");
	if(fd!=NULL)
		{
			if(data!=NULL)
				sinkReturnStr=fgets(buffer,2048,fd);

			while(fgets(buffer,2048,fd)!=NULL)
				{
					sscanf(buffer,"%i %[^\n]s",(int*)&currentline,(char*)&strarg);
					if(openFile(strarg,currentline,true)==false)
						{
							intarg=999;
							while(intarg!=-1)
								{
									sinkReturnStr=fgets(buffer,2048,fd);
									sscanf(buffer,"%i",(int*)&intarg);
								}
						}
					else
						{
							sinkReturnStr=fgets(buffer,2048,fd);
							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
							page=getPageStructPtr(currentPage-1);
							gtk_notebook_set_current_page(mainNotebook,currentPage-1);
							while(intarg!=-1)
								{
									gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
									gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
									toggleBookmark(NULL,&markiter);
									sinkReturnStr=fgets(buffer,2048,fd);
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

	currentTabNumber=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)-1;

	for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
		{
			pageStruct *page=getPageStructPtr(j);
			if(page!=NULL)
				{
					buf=new TextBuffer((GtkTextBuffer*)page->buffer);
					buf->scroll2CentreScreen((GtkTextView*)page->view,true);
					delete buf;
				}
		}
	loadingSession=false;
	updateStatusBar((GtkTextBuffer*)page->buffer,NULL,NULL,NULL);
	killBarberPole();
	ERRDATA
}

int showFileChanged(char *filename)
{
	ERRDATA
	GtkWidget	*dialog;
	gint		result;
	char		*message;

	ERRDATA
	sinkReturn=asprintf(&message,DIALOG_FILE_CHANGED_LABEL,filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,"%s",message);

#ifdef _USEGTK3_
	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,REFRESH_LABEL,GTK_RESPONSE_YES,NULL);
#else
	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_REFRESH,GTK_RESPONSE_YES,NULL);
#endif
	gtk_window_set_title(GTK_WINDOW(dialog),DIALOG_WARN_CHANGED_LABEL);

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	ERRDATA debugFree(&message);
	ERRDATA return(result);
}

void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data)
{
	ERRDATA
	pageStruct		*page=(pageStruct*)user_data;
	GtkTextIter		start;
	GtkTextIter		end;
	gchar			*buffer;
	long			filelen;
	int				answer;

	ERRDATA
	if(G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT==event_type)
		{
			if((page->itsMe==false) &&(page->showingChanged==false))
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

void add_source_mark_pixbufs(GtkSourceView *view)
{
	ERRDATA

	GtkImage	*image;
	GdkPixbuf	*pbuf;

	ERRDATA
	image=(GtkImage*)gtk_image_new_from_file(DATADIR"/pixmaps/BookMark.png");
	pbuf=gtk_image_get_pixbuf(image);

#ifdef _USEGTK3_
	GdkRGBA		color;
	gdk_rgba_parse (&color,highlightColour);

	GtkSourceMarkAttributes *attr=gtk_source_mark_attributes_new();

	gtk_source_mark_attributes_set_pixbuf(attr,pbuf);
	gtk_source_mark_attributes_set_background(attr,&color);
	gtk_source_view_set_mark_attributes(view,MARK_TYPE_1,attr,1);
#else
	GdkColor	color;
	gdk_color_parse(highlightColour,&color);
	gtk_source_view_set_mark_category_background(view,MARK_TYPE_1,&color);
	gtk_source_view_set_mark_category_icon_from_pixbuf(view,MARK_TYPE_1,pbuf);
	gtk_source_view_set_mark_category_priority(view,MARK_TYPE_1,1);
#endif
	ERRDATA
}

gboolean resetRegexMark (GtkWidget *widget,GdkEvent *event,pageStruct *page)
{
	if(page==NULL)
		return(false);

	page->regexList=NULL;
	page->regexMatchNumber=-1;
	return(false);
}

pageStruct *makeNewPage(void)
{
	ERRDATA
	pageStruct				*page;
	GtkTextIter				iter;
	GtkTextAttributes		*attr;
	GtkSourceUndoManager	*uman;

	ERRDATA
	page=(pageStruct*)malloc(sizeof(pageStruct));
	page->buffer=NULL;
	page->userDataList=NULL;
	page->filePath=NULL;
	page->realFilePath=NULL;

	page->pageWindow=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow),GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);

	page->buffer=gtk_source_buffer_new(NULL);
	page->view=(GtkSourceView*)gtk_source_view_new_with_buffer(page->buffer);
#ifdef _USEGTK3_
	gtk_style_context_add_provider(gtk_widget_get_style_context((GtkWidget*)page->view),provider,GTK_STYLE_PROVIDER_PRIORITY_USER);
#endif

//completion
	page->completion=gtk_source_view_get_completion(GTK_SOURCE_VIEW(page->view));
	g_object_set(page->completion,"remember-info-visibility",true,NULL);
	g_object_set(page->completion,"select-on-show",true,NULL);
	g_object_set(page->completion,"show-headers",true,NULL);
	g_object_set(page->completion,"show-icons",false,NULL);
	g_object_set(page->completion,"accelerators",0,NULL);
	createCompletion(page);

	g_signal_connect(G_OBJECT(page->view),"populate-popup",G_CALLBACK(populatePopupMenu),NULL);

	attr=gtk_text_view_get_default_attributes((GtkTextView*)page->view);
#ifdef _USEGTK3_
	page->highlightTag=gtk_text_buffer_create_tag((GtkTextBuffer*)page->buffer,"highlighttag","background",gdk_rgba_to_string((const GdkRGBA*)&attr->appearance.fg_color),"foreground",gdk_rgba_to_string((const GdkRGBA*)&attr->appearance.bg_color),NULL);
#else
	page->highlightTag=gtk_text_buffer_create_tag((GtkTextBuffer*)page->buffer,"highlighttag","background",gdk_color_to_string((const GdkColor*)&attr->appearance.fg_color),"foreground",gdk_color_to_string((const GdkColor*)&attr->appearance.bg_color),NULL);
#endif

	gtk_text_attributes_unref(attr);
	gtk_container_add(GTK_CONTAINER(page->pageWindow),(GtkWidget*)page->view);

	page->rebuildMenu=true;
	page->isFirst=true;
	page->itsMe=false;
	page->markList=NULL;
	page->gFile=NULL;
	page->monitor=NULL;
	page->lang=NULL;
	page->tabVbox=NULL;
	page->showingChanged=false;
	page->backMark=gtk_text_mark_new("back-mark",true);
	page->regexList=NULL;
	page->regexMatchNumber=-1;
	page->canUndo=false;
	page->canRedo=false;
	page->isDirty=false;
	page->searchString=NULL;
	page->replaceString=NULL;

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&iter);
	gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(page->buffer),page->backMark,&iter);

//dnd
	gtk_drag_dest_set((GtkWidget*)page->view,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets((GtkWidget*)page->view);
	gtk_drag_dest_add_text_targets((GtkWidget*)page->view);
	g_signal_connect_after(G_OBJECT(page->view),"drag-data-received",G_CALLBACK(dropText),(void*)page);

	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),FALSE);
	uman=gtk_source_buffer_get_undo_manager(page->buffer);
	g_signal_connect(G_OBJECT(uman),"can-undo-changed",G_CALLBACK(markUndo),(void*)page);
	g_signal_connect(G_OBJECT(uman),"can-redo-changed",G_CALLBACK(markRedo),(void*)page);
	g_signal_connect(G_OBJECT(page->buffer),"modified-changed",G_CALLBACK(markDirty),(void*)page);
	g_signal_connect(G_OBJECT(page->buffer),"changed",G_CALLBACK(setChangedSensitive),(void*)page);

	g_signal_connect(page->view,"line-mark-activated",G_CALLBACK(line_mark_activated),page);
	add_source_mark_pixbufs(GTK_SOURCE_VIEW(page->view));
//status bar
	g_signal_connect(G_OBJECT(page->buffer),"mark-set",G_CALLBACK(updateStatusBar),NULL);
	g_signal_connect((GtkWidget*)page->view,"button-release-event",G_CALLBACK(resetRegexMark),(void*)page);

	setPageSensitive();
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
	if(charset==NULL)
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
	GtkWidget				*label;
	gchar					*filename;
	pageStruct				*page;
	char					*str=NULL;
	char					*recenturi;
	int						linenum=linenumber-1;
	gchar					*contents=NULL;
	gsize					length;
	GError					*err=NULL;
	GtkWidget				*dialog;
	char					*filepathcopy=NULL;
	char					*tpath;
	int						whattodo;
	const gchar				*end;

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
					if((tpath!=NULL) &&(page->realFilePath!=NULL) &&(strcmp(page->realFilePath,tpath)==0))
						{
							gtk_notebook_set_current_page(mainNotebook,j);
							gotoLine(NULL,(gpointer)(long)linenumber);
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
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,DIALOG_NO_FILE_LABEL,filepathcopy);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
				}
			ERRDATA return(false);
		}

	if(access(filepathcopy,R_OK)!=0)
		{
			if(warn==true)
				{
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,DIALOG_CANT_OPEN_LABEL,filepath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
				}
		}

	if(linenum<0)
		linenum=0;

	page=makeNewPage();
	page->tabVbox=createNewBox(NEWVBOX,true,4);
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
					dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,DIALOG_CANT_OPEN_LABEL,filepath);
					gtk_dialog_run(GTK_DIALOG(dialog));
					gtk_widget_destroy(dialog);
				}
			ERRDATA return(false);
		}

	convertContents((char*)contents,length);
	ERRDATA debugFree(&contents);

	if(dataLen>0)
		{
			gtk_source_buffer_begin_not_undoable_action(page->buffer);
				gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&startiter);
				gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(page->buffer),&enditer);
				gtk_text_buffer_delete(GTK_TEXT_BUFFER(page->buffer),&startiter,&enditer);
				gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&startiter);
				whattodo=GTK_RESPONSE_YES;
				if(g_utf8_validate((const gchar*)convertedData,dataLen,&end)==false)
					whattodo=yesNo(DIALOG_YESNO_NON_TEXT_LABEL,(char*)filepath);
				if(whattodo==GTK_RESPONSE_YES)
					{
						dataLen=(long)end-(long)convertedData;
						gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&startiter,convertedData,dataLen);
					}
				else
					{
						gtk_widget_destroy(label);
						ERRDATA return(false);
					}
			gtk_source_buffer_end_not_undoable_action(page->buffer);
		}
	else
		{
			gtk_source_buffer_begin_not_undoable_action(page->buffer);
				gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&startiter);
				gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(page->buffer),&enditer);
				gtk_text_buffer_delete(GTK_TEXT_BUFFER(page->buffer),&startiter,&enditer);
				gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&startiter);
			gtk_source_buffer_end_not_undoable_action(page->buffer);
		}
	ERRDATA debugFree(&convertedData);

	page->gFile=g_file_new_for_path(page->filePath);
	page->monitor=g_file_monitor_file(page->gFile,(GFileMonitorFlags)G_FILE_MONITOR_NONE,NULL,NULL);
	g_signal_connect(G_OBJECT(page->monitor),"changed",G_CALLBACK(fileChangedOnDisk),(void*)page);

	str=g_file_get_path(page->gFile);
	recenturi=g_filename_to_uri(str,NULL,NULL);
	gtk_recent_manager_add_item(gtk_recent_manager_get_default(),recenturi);
	ERRDATA debugFree(&filename);
	ERRDATA debugFree(&recenturi);
	ERRDATA debugFree(&str);
	ERRDATA debugFree(&filepathcopy);

//connect to mainNotebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pageWindow));
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
	setFilePrefs(page);

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"openFile");

	/* move cursor to the linenumber */
	if(fromGOpen==true)
		{
			gtk_widget_show_all((GtkWidget*)(GtkTextView*)page->view);
//			if(linenum>0)
//				while(gtk_events_pending())
//					gtk_main_iteration();
		}

	if(loadingSession==false)
		{
			TextBuffer	*buf;
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			gotoLine(NULL,(void*)(long)(linenum+1));
			buf->scroll2CentreScreen((GtkTextView*)page->view,true);
//yet another gtk3 hack;
			buf->scroll2CentreScreen((GtkTextView*)page->view,false);
			delete buf;
		}
	page->canUndo=false;
	page->canRedo=false;
	page->isDirty=false;
	setPageSensitive();
	
	ERRDATA return(TRUE);
}

VISIBLE void newFile(GtkWidget *widget,gpointer data)
{
	ERRDATA
	GtkTextIter	iter;
	GtkWidget	*label;
	pageStruct	*page;

	page=makeNewPage();
	page->tabVbox=createNewBox(NEWVBOX,true,4);
	page->filePath=NULL;
	page->dirName=NULL;

	sinkReturn=asprintf(&page->fileName,"%s-%i",UNTITLED_LABEL,untitledNumber);
	untitledNumber++;

	label=makeNewTab(page->fileName,NULL,page);
	gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);

	/* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(page->buffer),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);

//connect to ntebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pageWindow));
	g_object_set_data(G_OBJECT(page->tabVbox),"pagedata",(gpointer)page);

	gtk_notebook_append_page(mainNotebook,page->tabVbox,label);
	gtk_notebook_set_tab_reorderable(mainNotebook,page->tabVbox,true);

	gtk_notebook_set_current_page(mainNotebook,currentPage);
	currentPage++;
	gtk_widget_show_all((GtkWidget*)mainNotebook);
	setFilePrefs(page);

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newFile");

	setPageSensitive();
}
