/*
 *
 * ©K. D. Hedger. Fri  6 May 10:52:00 BST 2016 kdhedger68713@gmail.com

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
												fprintf(fd,"%s %s\n",dataptr[cnt].name,(char*)list->data);
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
	int			retval=NOERR;
	varStrings	*vs=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					sinkReturnStr=fgets(buffer,2048,fd);
					vs=allocVStrings(buffer);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((vs->data!=NULL) &&(vs->name!=NULL) &&(strcmp(vs->name,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
											case TYPEINT:
												*(int*)dataptr[cnt].data=atoi(vs->data);
												break;
											case TYPESTRING:
												if(*(char**)(dataptr[cnt].data)!=NULL)
													{
														ERRDATA debugFree(&*(char**)(dataptr[cnt].data));
													}
												*(char**)(dataptr[cnt].data)=(char*)strdup(vs->data);
												break;
											case TYPEBOOL:
												*(bool*)dataptr[cnt].data=(bool)atoi(vs->data);
												break;
											case TYPELIST:
												*(GSList**)dataptr[cnt].data=g_slist_append(*(GSList**)dataptr[cnt].data,strdup(vs->data));
												break;
										}
								}
							cnt++;
						}
					freeAndNull(&vs->name);
					freeAndNull(&vs->data);
					free(vs);
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
	char		*correctedname;

	GtkWidget	*close=gtk_image_new_from_icon_name(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	GtkWidget	*button=gtk_button_new();

	hbox=createNewBox(NEWHBOX,false,0);
	correctedname=truncateWithElipses(name,maxTabChars);

	label=gtk_label_new(correctedname);
	ERRDATA debugFree(&correctedname);

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(label,tooltip);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

#ifdef _USEGTK3_
#if GTK_MINOR_VERSION >=22
	gtk_widget_set_focus_on_click((GtkWidget*)button,false);
#else
	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
#endif
#else
	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
#endif
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


#ifdef _USEGTK3_
	setGtk3NBTabSize();
	applyCSS((GtkWidget*)evbox,tabBoxProvider);
#endif

	gtk_widget_show_all(evbox);

	ERRDATA
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newTab");

	ERRDATA return(evbox);
}

void setFilePrefs(pageStruct *page)
{
	ERRDATA
	PangoFontDescription	*font_desc;
	GtkTextAttributes		*attr;
	gchar					*forecol;
	gchar					*backcol;

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

	forecol=gdk_rgba_to_string(&fcol);
	backcol=gdk_rgba_to_string(&bcol);
	g_object_set((gpointer)page->highlightTag,"background",backcol,"foreground",forecol,NULL);
	g_object_set((gpointer)page->vertTag.verticalTag,"background",backcol,"foreground",forecol,NULL);
#else
	forecol=gdk_color_to_string((const GdkColor*)&attr->appearance.bg_color);
	backcol=gdk_color_to_string((const GdkColor*)&attr->appearance.fg_color);
	g_object_set((gpointer)page->highlightTag,"background",backcol,"foreground",forecol,NULL);
	g_object_set((gpointer)page->vertTag.verticalTag,"background",backcol,"foreground",forecol,NULL);
#endif
	g_free(forecol);
	g_free(backcol);

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
			page=getPageStructByIDFromPage(loop);
			if(page!=NULL)
				{
					gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);
					setFilePrefs(page);
				}
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
	GString			*str;
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

enum {SAVEFROMCURRENTPAGE=0,SAVEASPAGE=-1};

VISIBLE bool saveFile(GtkWidget *widget,gpointer data)
{
	ERRDATA
	pageStruct	*page=getPageStructByIDFromPage(-1);
	GtkTextIter	start,end;
	gchar		*text;
	FILE		*fd=NULL;
	GtkWidget	*dialog;
	long		dowhat=0;

	dowhat=(long)data;
	if(dowhat==0)
		page=getPageStructByIDFromPage(-1);

	if(dowhat==-1)
		page=getPageStructByIDFromPage(-1);

	if(dowhat>0)
		page=getPageStructByIDFromPage(dowhat-1);

	if(page==NULL)
		return(true);

	page->itsMe=true;
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
	text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,FALSE);


	if(page->filePath!=NULL && (dowhat>=0))
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
		
			saveFilePath=page->filePath;
			saveFileName=page->fileName;
			if(page->dirName!=NULL)
				{
					ERRDATA debugFree(&page->dirName);
				}
			page->dirName=g_path_get_dirname(page->filePath);

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
	GString			*str=g_string_new(NULL);
	char			*command;
	GtkTextIter		iter;
	pageStruct		*page;
	char			*convstr=NULL;

	ERRDATA

#ifdef _USEGTK3_
	dialog=gtk_file_chooser_dialog_new(OPEN_TT_LABEL,NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN_LABEL, GTK_RESPONSE_ACCEPT,NULL);
#else
	dialog=gtk_file_chooser_dialog_new(OPEN_TT_LABEL,NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
#endif
	gtk_window_set_transient_for((GtkWindow*)dialog,(GtkWindow*)mainWindow);
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
		{
			filepath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			filename=g_path_get_basename(filepath);
			newFile(NULL,NULL);
			page=getPageStructByIDFromPage(-1);
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
			gtk_widget_show_all((GtkWidget*)page->tabVbox);
			setChangedSensitive((GtkTextBuffer*)page->buffer,page);
		}

	rebuildTabsMenu();
	gtk_widget_destroy(dialog);
	ERRDATA
}

VISIBLE void reloadFile(GtkWidget *widget,gpointer data)
{
	ERRDATA
	pageStruct	*page=getPageStructByIDFromPage(-1);
	gchar		*buffer;
	long		filelen;
	GtkTextIter	start;
	GtkTextIter	end;

	ERRDATA
	if(page->filePath!=NULL)
		{
			moveBMsForPage(page,UPDATEBMLINE);
			g_file_get_contents(page->filePath,&buffer,(gsize*)&filelen,NULL);
			gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
			gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
			gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
			gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,buffer,filelen);
			ERRDATA debugFree(&buffer);
			moveBMsForPage(page,MOVEBM);
		}
	ERRDATA
}

VISIBLE void saveSession(const char *filename,const char *path)
{
	ERRDATA
	pageStruct		*page;
	FILE			*fd=NULL;
	char			*filepath;
	GtkTextMark		*mark;
	GtkTextIter		iter;
	int				linenumber;
	GtkTextIter		markiter;
	GList			*ptr;
	const char		*name;
	GtkAllocation	alloc;
	int				winx;
	int				winy;

	ERRDATA

	if(path==NULL)
		{
			sinkReturn=asprintf(&filepath,"%s/%s",getenv("HOME"),APPFOLDENAME);
			g_mkdir_with_parents(filepath,493);
			ERRDATA debugFree(&filepath);
			sinkReturn=asprintf(&filepath,"%s/%s/session",getenv("HOME"),APPFOLDENAME);
			name="session";
		}
	else
		{
			sinkReturn=asprintf(&filepath,"%s",path);
			asprintf(&thisSessionName,"%s",filename);
			name=filename;
		}

	fd=fopen(filepath,"w");
	if(fd!=NULL)
		{
			fprintf(fd,"%s\n",name);
			gtk_widget_get_allocation(mainWindow,&alloc);
			gtk_window_get_position((GtkWindow*)mainWindow,&winx,&winy);
			if( (alloc.width>10) && (alloc.height>10) )
				fprintf(fd,"%i %i %i %i\n",alloc.width,alloc.height,winx,winy);
//reseve space for expansion
			fprintf(fd,"#RESERVED\n#RESERVED\n");
			for(int loop=0; loop<gtk_notebook_get_n_pages(mainNotebook); loop++)
				{
					page=getPageStructByIDFromPage(loop);
					if(page!=NULL)
						{
							mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
							gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
							linenumber=gtk_text_iter_get_line(&iter);
							fprintf(fd,"%i %i %s\n",linenumber,(int)page->hidden,page->filePath);
//reseve space for expansion
							//fprintf(fd,"#RESERVED\n#RESERVED\n");
							if(page->isEditable==true)
								fprintf(fd,"unlocked\n");
							else
								fprintf(fd,"locked\n");
							fprintf(fd,"#RESERVED\n");
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
				}

			ERRDATA
			fclose(fd);
			freeAndNull(&thisSessionName);
			thisSessionName=strdup(name);
			freeAndNull(&thisSessionPath);
			thisSessionPath=strdup(filepath);
			ERRDATA debugFree(&filepath);
		}
	ERRDATA
}

VISIBLE void restoreSession(GtkWidget *widget,gpointer data)
{
	ERRDATA
	FILE		*fd=NULL;
	char		*filename;
	char		buffer[2048];
	int			intarg;
	char		strarg[2048];
	GtkTextIter	markiter;
	int			currentline;
	TextBuffer	*buf=new TextBuffer;
	int			hidden;
	int			winx=0,winy=0;
	int			width=800,hite=600;
	bool		goodfile=false;
	GtkTextMark	*mark;
	bool		lockit=false;

	_ENTER_
	ERRDATA

	loadingSession=true;
	fromGOpen=false;
	waitForFinish=false;
	showBarberPole(DIALOG_POLE_RESTORING);
	g_signal_handler_block(mainNotebook,switchPageHandler);

	usleep(500000);
	if(data==NULL)
		{
			sinkReturn=asprintf(&filename,"%s/%s/session",getenv("HOME"),APPFOLDENAME);
		}
	else
		{
			sinkReturn=asprintf(&filename,"%s",(char*)data);
		}

	freeAndNull(&thisSessionName);
	freeAndNull(&thisSessionPath);
	thisSessionPath=strdup(filename);

	fd=fopen(filename,"r");
	if(fd!=NULL)
		{
			closeAllTabs(NULL,NULL);
			sinkReturnStr=fgets(buffer,2048,fd);
			buffer[strlen(buffer)-1]=0;
			thisSessionName=strdup(buffer);
			sinkReturn=fscanf(fd,"%i %i %i %i\n",&width,&hite,&winx,&winy);
			gtk_window_resize((GtkWindow*)mainWindow,width,hite);
			if(winx!=-1 && winy!=-1)
				gtk_window_move((GtkWindow *)mainWindow,winx,winy);
//skip reserved
			sinkReturnStr=fgets(buffer,2048,fd);
			sinkReturnStr=fgets(buffer,2048,fd);

			while(fgets(buffer,2048,fd)!=NULL)
				{
//skip reserved
					if(strcmp(buffer,"#RESERVED\n")==0)
						continue;
					strarg[0]=0;
					sscanf(buffer,"%i %i %[^\n]s",(int*)&currentline,(int*)&hidden,(char*)&strarg);
					setBarberPoleMessage(strarg);
					if(strlen(strarg)>0)
						{
							goodfile=openFile(strarg,currentline,false);
							if(goodfile==true)
								{
									sinkReturnStr=fgets(buffer,2048,fd);
									sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
									intarg=999;
									lockit=false;
									while(intarg!=-1)
										{
											if(strcmp(buffer,"locked\n")==0)
												{
													lockit=true;
													sinkReturnStr=fgets(buffer,2048,fd);
													sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
													continue;
												}
											if((strcmp(buffer,"unlocked\n")==0) || (strcmp(buffer,"#RESERVED\n")==0))
												{
													sinkReturnStr=fgets(buffer,2048,fd);
													sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
													continue;
												}
											gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)currentPageStruct->buffer,&markiter,intarg);
											gtk_text_buffer_place_cursor((GtkTextBuffer*)currentPageStruct->buffer,&markiter);
											toggleBookmark(NULL,&markiter);

											sinkReturnStr=fgets(buffer,2048,fd);
											sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
										}
									gotoLine(NULL,(gpointer)(long)currentline);
									if(hidden==true)
										hideTab(NULL,(void*)currentPageStruct);
									else
										currentPageStruct->hidden=false;
									if(lockit==true)
										{
											currentPageStruct->isEditable=false;
											gtk_widget_set_sensitive(GTK_WIDGET(currentPageStruct->view),currentPageStruct->isEditable);
										}
								}
							else
								{
									intarg=999;
									while(intarg!=-1)
										{
											sinkReturnStr=fgets(buffer,2048,fd);
											sscanf(buffer,"%i",(int*)&intarg);
										}
								}
						}
				}
			fclose(fd);
			ERRDATA debugFree(&filename);
		}
	setBarberPoleMessage("Finishing ...");
	currentTabNumber=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)-1;
	for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
		{
			pageStruct *page=getPageStructByIDFromPage(j);
			if(page!=NULL)
				{
					buf->textBuffer=(GtkTextBuffer*)page->buffer;
					buf->scroll2CentreScreen((GtkTextView*)page->view,true);
				}
		}
	delete buf;
	
	gtk_statusbar_remove_all((GtkStatusbar*)statusWidget,statusID);
	while(gtk_events_pending())
		gtk_main_iteration();
	loadingSession=false;
	rebuildTabsMenu();
	g_signal_handler_unblock(mainNotebook,switchPageHandler);
	updateStatusBar(NULL,NULL,NULL,NULL);

	killBarberPole();
	_LEAVE_
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
	const char	*umarks[]={MARK_TYPE_1,MARK_TYPE_2,MARK_TYPE_3,MARK_TYPE_4,MARK_TYPE_5};
	const char	*markpaths[]={BOOKMARKPNG,USERMARK1PNG,USERMARK2PNG,USERMARK3PNG,USERMARK4PNG};

	for(int j=0;j<5;j++)
		{
			image=(GtkImage*)gtk_image_new_from_file(markpaths[j]);
			pbuf=gtk_image_get_pixbuf(image);

#ifdef _USEGTK3_
			GdkRGBA		color;
			gdk_rgba_parse (&color,highlightColour);

			GtkSourceMarkAttributes *attr=gtk_source_mark_attributes_new();

			gtk_source_mark_attributes_set_pixbuf(attr,pbuf);
			if(j==0)
				gtk_source_mark_attributes_set_background(attr,&color);
			gtk_source_view_set_mark_attributes(view,umarks[j],attr,1);
#else
			GdkColor	color;
			gdk_color_parse(highlightColour,&color);
			if(j==0)
				gtk_source_view_set_mark_category_background(view,umarks[j],&color);
			gtk_source_view_set_mark_category_icon_from_pixbuf(view,umarks[j],pbuf);
			gtk_source_view_set_mark_category_priority(view,umarks[j],j);
#endif
		}
	ERRDATA
}

gboolean resetRegexMark (GtkWidget *widget,GdkEvent *event,pageStruct *page)
{
	if(page==NULL)
		return(false);

	page->regexList=NULL;
	page->regexMatchNumber=-1;
	loadingSession=false;
	page->vertTag.gotStart=false;

	return(false);
}

bool highLightText(GtkWidget *widget,GdkEvent *event,gpointer data)
{
	TextBuffer	*buf;
	pageStruct	*page;
	page=(pageStruct*)data;
	if(page!=NULL)
		{
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->getSelectionBounds();
			if(buf->selectionStart==-1)
				{
					page->startChar=buf->charPos;
					page->endChar=buf->charPos;
				}
			else
				{
					page->startChar=buf->selectionStart;
					page->endChar=buf->selectionEnd;
				}
			delete buf;
		}
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

//vert select
	gtk_widget_set_events((GtkWidget*)page->view,GDK_POINTER_MOTION_MASK);
	g_signal_connect(G_OBJECT(page->view),"motion-notify-event",G_CALLBACK(verticalSelect),(void*)page);
	g_signal_connect(G_OBJECT(page->view),"button-release-event",G_CALLBACK(highLightText),(void*)page);

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
//highlite
	page->highlightTag=gtk_text_buffer_create_tag((GtkTextBuffer*)page->buffer,"highlighttag",NULL,NULL);
//vert select
	page->vertTag.verticalTag=gtk_text_buffer_create_tag((GtkTextBuffer*)page->buffer,"verticaltag",NULL,NULL);

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
	page->regexList=NULL;
	page->regexMatchNumber=-1;
	page->canUndo=false;
	page->canRedo=false;
	page->isDirty=false;
	page->searchString=NULL;
	page->replaceString=NULL;
	page->hidden=false;
	page->startChar=-1;
	page->endChar=-1;
	page->isEditable=true;
	page->vertTag.gotStart=false;
	page->vertTag.xStartLoc=0;
	page->vertTag.yStartLoc=0;
	page->vertTag.xEndLoc=0;
	page->vertTag.yEndLoc=0;
	page->vertTag.gotSelect=false;;

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

//	setPageSensitive();
	pages=g_list_prepend(pages,(gpointer)page);
	ERRDATA return(page);
}

void convertContentsFilePath(char *filepathcopy,char *data,int datalen)
{
	char		*command=NULL;
	char		*charsetfrom=NULL;
	iconv_t		cd;
    size_t		len_src;
    size_t		len_dst;
	char		*startptr;

	dataLen=-1;
	asprintf(&command,"file -i \"%s\"|awk -F'=' '{print $2}'",filepathcopy);
	charsetfrom=oneLiner(command);
	free(command);
	if(charsetfrom!=NULL)
		{
			len_src=datalen;
			len_dst=len_src * 8;
			cd=iconv_open("UTF-8",charsetfrom);
			convertedData=(char*)malloc(datalen*16);
			startptr=convertedData;

			iconv(cd,&data,&len_src,&startptr,&len_dst);
			iconv_close(cd);

			dataLen=(long)startptr-(long)convertedData;
			free(charsetfrom);
		}
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

//	_ENTER_
	if(readLinkFirst==true)
		filepathcopy=realpath(filepath,NULL);
	else
		filepathcopy=strdup(filepath);

	filename=g_path_get_basename(filepathcopy);

	for(int j=0; j<gtk_notebook_get_n_pages(mainNotebook); j++)
		{
			page=getPageStructByIDFromPage(j);
			if(page!=NULL)
				{
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

	convertContentsFilePath(filepathcopy,contents,length);
	ERRDATA debugFree(&contents);

	if(dataLen>0)
		{
			gtk_source_buffer_begin_not_undoable_action(page->buffer);
				whattodo=GTK_RESPONSE_YES;
				if(g_utf8_validate((const gchar*)convertedData,dataLen,&end)==false)
					whattodo=yesNo(DIALOG_YESNO_NON_TEXT_LABEL,(char*)filepath);
				if(whattodo==GTK_RESPONSE_YES)
					{
						dataLen=(long)end-(long)convertedData;
						gtk_text_buffer_set_text((GtkTextBuffer*)page->buffer,convertedData,dataLen);
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
				gtk_text_buffer_set_text((GtkTextBuffer*)page->buffer,"",-1);
			gtk_source_buffer_end_not_undoable_action(page->buffer);
		}
	ERRDATA debugFree(&convertedData);

	page->gFile=g_file_new_for_path(page->filePath);
	page->monitor=g_file_monitor_file(page->gFile,(GFileMonitorFlags)G_FILE_MONITOR_NONE,NULL,NULL);
	g_signal_connect(G_OBJECT(page->monitor),"changed",G_CALLBACK(fileChangedOnDisk),(void*)page);

	str=g_file_get_path(page->gFile);
	recenturi=g_filename_to_uri(str,NULL,NULL);
	gtk_recent_manager_add_item(recentMan,recenturi);
	ERRDATA debugFree(&filename);
	ERRDATA debugFree(&recenturi);
	ERRDATA debugFree(&str);
	ERRDATA debugFree(&filepathcopy);
//connect to mainNotebook
	gtk_container_add(GTK_CONTAINER(page->tabVbox),GTK_WIDGET(page->pageWindow));
	g_object_set_data(G_OBJECT(page->tabVbox),"pageid",(gpointer)(long)page->pageID);

	if(openInThisTab==-1)
		currentPage=gtk_notebook_append_page(mainNotebook,page->tabVbox,label);
	else
		currentPage=gtk_notebook_insert_page(mainNotebook,page->tabVbox,label,openInThisTab+1);

	gtk_notebook_set_tab_reorderable(mainNotebook,page->tabVbox,true);

	gtk_widget_show(page->tabVbox);
	gtk_notebook_set_current_page(mainNotebook,currentPage);

	if(openInThisTab==-1)
		currentPage++;

	gtk_widget_grab_focus((GtkWidget*)page->view);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(page->buffer),false);
	gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)page->buffer,styleScheme);

	gtk_widget_show((GtkWidget*)page->pageWindow);
	gtk_widget_show((GtkWidget*)page->view);
	setFilePrefs(page);

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"openFile");

	/* move cursor to the linenumber */
	if(fromGOpen==true)
		gtk_widget_show_all((GtkWidget*)(GtkTextView*)page->view);

	page->canUndo=false;
	page->canRedo=false;
	page->isDirty=false;
	page->isEditable=true;

	if(loadingSession==false)
		{
			TextBuffer	*buf;
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			gotoLine(NULL,(void*)(long)(linenum+1));
			buf->scroll2CentreScreen((GtkTextView*)page->view,true);
//yet another gtk3 hack;
			buf->scroll2CentreScreen((GtkTextView*)page->view,false);
			delete buf;
			rebuildTabsMenu();
		}
	setPageSensitive();
	currentPageStruct=page;
//	_LEAVE_
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
	g_object_set_data(G_OBJECT(page->tabVbox),"pageid",(gpointer)(long)page->pageID);

	gtk_notebook_append_page(mainNotebook,page->tabVbox,label);
	gtk_notebook_set_tab_reorderable(mainNotebook,page->tabVbox,true);

	gtk_notebook_set_current_page(mainNotebook,gtk_notebook_get_n_pages(mainNotebook)-1);
	currentPage++;
	gtk_widget_show_all((GtkWidget*)page->tabVbox);
	setFilePrefs(page);
	rebuildTabsMenu();

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"newFile");

	setPageSensitive();
}

VISIBLE void doOpenFile(GtkWidget *widget,gpointer data)
{
	ERRDATA
	GtkWidget	*dialog;
	char		*filename;
	GSList		*filenames;
	GSList		*thisnext;

#ifdef _USEGTK3_
	dialog=gtk_file_chooser_dialog_new(OPEN_TT_LABEL,NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN_LABEL, GTK_RESPONSE_ACCEPT,NULL);
#else
	dialog=gtk_file_chooser_dialog_new(OPEN_TT_LABEL,NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
#endif
	gtk_window_set_transient_for((GtkWindow*)dialog,(GtkWindow*)mainWindow);
	gtk_file_chooser_set_select_multiple((GtkFileChooser*)dialog,true);
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
		{
			filenames=gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
			thisnext=filenames;
			while(thisnext!=NULL)
				{
					filename=(char*)thisnext->data;
					openFile(filename,0,true);
					g_free(filename);
					thisnext=thisnext->next;
				}
			g_slist_free(filenames);
		}
	gtk_widget_destroy(dialog);
}

VISIBLE void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	ERRDATA
	gchar**	array=gtk_selection_data_get_uris(selection_data);
	int		cnt=g_strv_length(array);
	char	*filename;

	for(int j=0; j<cnt; j++)
		{
			filename=g_filename_from_uri(array[j],NULL,NULL);
			openFile(filename,0,true);
		}
	g_strfreev(array);
}

void recentFileMenu(GtkRecentChooser *chooser,gpointer *data)
{
	gchar	*uri=NULL;
	char	*filename;

	if(data!=NULL)
		{
			openFile((char*)data,0,true);
			return;
		}

	uri=gtk_recent_chooser_get_current_uri((GtkRecentChooser*)chooser);
	if(uri!=NULL)
		{
			filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
			openFile(filename,0,true);
			g_free(uri);
			g_free(filename);
		}
}

void openFromTab(GtkMenuItem *widget,pageStruct *page)
{
	ERRDATA
	char		*filepath=NULL;

	openInThisTab=gtk_notebook_get_current_page(mainNotebook);
	sinkReturn=asprintf(&filepath,"%s/%s",page->dirName,gtk_menu_item_get_label(widget));
	openFile(filepath,0,true);
	openInThisTab=-1;
	ERRDATA debugFree(&filepath);
}

