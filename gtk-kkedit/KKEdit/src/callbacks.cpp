/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:48:49 GMT 2015 kdhedger68713@gmail.com

 * This file (callbacks.cpp) is part of KKEdit.

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

GtkWidget*			tabMenu;
char				defineText[1024];
GtkPrintSettings*	settings=NULL;
bool				closingAll=false;

void releasePlugs(gpointer data,gpointer user_data)
{
	ERRDATA
	if(((moduleData*)data)->module!=NULL)
		g_module_close((GModule*)((moduleData*)data)->module);
}

void setToobarSensitive(void)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(currentTabNumber);

	for(int j=0; j<(int)strlen(toolBarLayout); j++)
		{
			switch(toolBarLayout[j])
				{
				case 'N':
//newnewButton
					if(newButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)newButton,true);
					break;
				case 'O':
//open+recent
					if(openButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)openButton,true);
					break;
				case 'S':
//save
					if(saveButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)saveButton,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
							else
								gtk_widget_set_sensitive((GtkWidget*)saveButton,false);
						}
					break;
				case 'X':
//cut
					if(cutButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)cutButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)cutButton,false);
						}
					break;
				case 'C':
//copy
					if(copyButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)copyButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)copyButton,false);
						}
					break;
				case 'P':
//paste
					if(pasteButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)pasteButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)pasteButton,false);
						}
					break;
				case 'U':
//undo
					if(undoButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)undoButton,gtk_source_buffer_can_undo(page->buffer));
							else
								gtk_widget_set_sensitive((GtkWidget*)undoButton,false);
						}
					break;
				case 'R':
//redo
					if(redoButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)redoButton,gtk_source_buffer_can_redo(page->buffer));
							else
								gtk_widget_set_sensitive((GtkWidget*)redoButton,false);
						}
					break;
				case 'F':
//find
					if(findButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)findButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)findButton,false);
						}
					break;
				case 'G':
//navigation
					if(gotoDefButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)gotoDefButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)gotoDefButton,false);
						}
					break;

				case 'B':
//go back
					if(backButton!=NULL)
						{
							gtk_widget_set_sensitive((GtkWidget*)backButton,false);
							if(page!=NULL)
								{
									if(history->canGoBack()==true)
										gtk_widget_set_sensitive((GtkWidget*)backButton,true);
								}
						}
					break;


				case '9':
//find in gtkdoc
					if(gotoLineButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)gotoLineButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)gotoLineButton,false);
						}
					break;

				case 'A':
//find in gtkdoc
					if(findApiButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)findApiButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)findApiButton,false);
						}
					break;

				case 'D':
//find in function def
					if(findFuncDefButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)findFuncDefButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)findFuncDefButton,false);
						}
					break;
				case 'L':
//livesearch
					if(liveSearchButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)liveSearchButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)liveSearchButton,false);
						}
					break;
				}
		}
}

void destroyBMData(gpointer data)
{
	ERRDATA debugFree(&((bookMarksNew*)data)->markName);
}

VISIBLE void removeAllBookmarks(GtkWidget* widget,GtkTextIter* titer)
{
	ERRDATA
	pageStruct*	page=NULL;
	int			numpages;
	GtkTextIter	startiter;
	GtkTextIter	enditer;

	numpages=gtk_notebook_get_n_pages(mainNotebook);
	for(int j=0; j<numpages; j++)
		{
			page=getPageStructPtr(j);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
			gtk_source_buffer_remove_source_marks(page->buffer,&startiter,&enditer,NULL);
		}
	g_list_free_full(newBookMarksList,destroyBMData);
	newBookMarksList=NULL;
	rebuildBookMarkMenu();
	gtk_widget_show_all(bookMarkMenu);
}

VISIBLE void toggleBookmark(GtkWidget* widget,GtkTextIter* titer)
{
	ERRDATA
	pageStruct*		page=getPageStructPtr(-1);
	GtkWidget*		menuitem;
	GtkTextMark*	mark;
	GtkTextIter*	iter;
	GtkTextIter		siter;
	int				line;
	GtkTextIter		startprev,endprev;
	char*			previewtext;
	GSList*			mark_list=NULL;
	const gchar*	mark_type;
	GList*			ptr=NULL;
	bookMarksNew*	bookmarkdata;
	char*			correctedpreview;

	if(page==NULL)
		return;

	mark_type=MARK_TYPE_1;
	if(titer==NULL)
		{
			mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
			gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&siter,mark);
			iter=&siter;
		}
	else
		iter=titer;

	mark_list=gtk_source_buffer_get_source_marks_at_line(page->buffer,gtk_text_iter_get_line(iter),mark_type);
	if (mark_list!=NULL)
		{
			//remove entry from bookmark list
			ptr=newBookMarksList;
			while(ptr!=NULL)
				{
					if((gpointer)((bookMarksNew*)ptr->data)->mark==(gpointer)GTK_TEXT_MARK(mark_list->data))
						{
							ERRDATA debugFree(&((bookMarksNew*)ptr->data)->markName);
							newBookMarksList=g_list_remove(newBookMarksList,ptr->data);
							gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(page->buffer),GTK_TEXT_MARK(mark_list->data));
							break;
						}
					ptr=g_list_next(ptr);
				}

			/* just take the first and delete it */
//rebuild bookmark menu
			rebuildBookMarkMenu();

			ptr=newBookMarksList;
			while(ptr!=NULL)
				{
					menuitem=gtk_image_menu_item_new_with_label(((bookMarksNew*)ptr->data)->label);
					gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(jumpToMark),(void*)ptr->data);
					ptr=g_list_next(ptr);
				}
			gtk_widget_show_all(bookMarkMenu);
			g_slist_free(mark_list);
		}
	else
		{
			/* no mark found: create one */
			bookmarkdata=(bookMarksNew*)malloc(sizeof(bookMarksNew));
			newBookMarksList=g_list_append(newBookMarksList,(gpointer)bookmarkdata);
			bookmarkdata->page=page;
			asprintf(&bookmarkdata->markName,"%s-%i",gettext("Bookmark"),bmMarkNumber++);
			bookmarkdata->mark=gtk_source_buffer_create_source_mark(page->buffer,bookmarkdata->markName,mark_type,iter);
//preview text for menu
			line=gtk_text_iter_get_line(iter);
			gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&startprev,line);
			gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&endprev,line+1);
			previewtext=gtk_text_iter_get_text(&startprev,&endprev);

			previewtext[strlen(previewtext)-1]=0;
			g_strchug(previewtext);
			g_strchomp(previewtext);

			correctedpreview=truncateWithElipses(previewtext,maxBMChars);
			ERRDATA debugFree(&previewtext);
			previewtext=correctedpreview;

			bookmarkdata->label=previewtext;
			bookmarkdata->line=line;
			menuitem=gtk_menu_item_new_with_label(bookmarkdata->label);
			gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(jumpToMark),(void*)bookmarkdata);
			gtk_widget_show_all(bookMarkMenu);
		}
}

void refreshMainWindow(void)
{
	ERRDATA
	gtk_widget_show(mainWindow);
	gtk_widget_show_all((GtkWidget*)mainNotebook);
	gtk_widget_show(mainWindowVBox);
	gtk_widget_show(mainVPane);
	gtk_widget_show(mainWindowHPane);
	gtk_widget_show(secondWindowHPane);
	gtk_widget_show_all(menuBar);
	gtk_widget_show_all(toolBarBox);
	gtk_widget_show_all(mainWindowHBox);

	if(showToolOutWin)
		gtk_widget_show(toolOutVBox);
	else
		gtk_widget_hide(toolOutVBox);

	if(showStatus)
		gtk_widget_show(statusWidget);
	else
		gtk_widget_hide(statusWidget);

	if(showToolBar)
		gtk_widget_show((GtkWidget*)toolBarBox);
	else
		gtk_widget_hide((GtkWidget*)toolBarBox);

}

int yesNo(char* question,char* file)
{
	ERRDATA
	GtkWidget*	dialog;
	int			result;

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,"%s %s",question,file);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_YES,GTK_RESPONSE_YES,GTK_STOCK_NO,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),gettext("What Do You Want To Do?"));

	result=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return(result);
}

VISIBLE void doOpenFile(GtkWidget* widget,gpointer data)
{
	ERRDATA
	GtkWidget*	dialog;
	char*		filename;
	GSList*		filenames;
	GSList*		thisnext;

	dialog=gtk_file_chooser_dialog_new(gettext("Open File"),NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	gtk_file_chooser_set_select_multiple((GtkFileChooser*)dialog,true);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filenames=gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
			thisnext=filenames;
			while(thisnext!=NULL)
				{
					filename=(char*)thisnext->data;
					openFile(filename,0,true);
					g_free (filename);
					thisnext=thisnext->next;
				}
			g_slist_free(filenames);
		}
	gtk_widget_destroy (dialog);
	refreshMainWindow();
}

int show_question(char* filename)
{
	ERRDATA
	GtkWidget*	dialog;
	gint		result;
	char*		message;

	asprintf(&message,gettext("Save file %s before closing?"),filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_SAVE,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),gettext("Warning unsaved data!"));

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	ERRDATA debugFree(&message);
	return(result);
}

void updateStatusBar(GtkTextBuffer* textbuffer,GtkTextIter* location,GtkTextMark* mark,gpointer data)
{
	pageStruct* page=(pageStruct*)data;
	TextBuffer*	buf;
	char*		message=NULL;
	const char*	path;
	const char*	lang;

	if(doUpdateWidgets==false)
		return;

	if(sessionBusy==true)
		return;

	if((statusMessage!=NULL) || (busyFlag==true))
		return;

	if(busyFlag==true)
		return;

	pageStruct* pagecheck=getPageStructPtr(currentTabNumber);
	if(pagecheck==NULL)
		return;


	if((page==NULL) || (showStatus==false))
		{
			gtk_statusbar_pop((GtkStatusbar*)statusWidget,0);
			gtk_statusbar_push((GtkStatusbar*)statusWidget,0,"");
			return;
		}

	if(pagecheck!=page)
		return;

	ERRDATA

	page->regexList=NULL;
	page->regexMatchNumber=-1;

	path=page->filePath;
	lang=page->lang;

	if(path==NULL)
		path="";

	if(lang==NULL)
		lang="";

	buf=new TextBuffer(textbuffer);

	gtk_statusbar_pop((GtkStatusbar*)statusWidget,0);
	asprintf(&message,gettext("Line %i Column %i \t\tSyntax Highlighting %s\t\tFilePath %s"),buf->lineNum,buf->column,lang,path);
	gtk_statusbar_push((GtkStatusbar*)statusWidget,0,message);
	ERRDATA debugFree(&message);
	delete buf;
}

void setSensitive(void)
{
	ERRDATA
	pageStruct*		page=getPageStructPtr(-1);
	const gchar*	text;
	char*			newlabel;
	int				offset=0;
	GtkTextIter	start_find,end_find;

	if(page==NULL)
		return;

	if(sessionBusy==true)
		return;

	setToobarSensitive();

	if(page==NULL)
		{
//menu
			gtk_widget_set_sensitive((GtkWidget*)undoMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)redoMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)undoAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)redoAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)cutMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)copyMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)pasteMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)bookMarkMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)funcMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)navMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)printMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)closeMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)closeAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)revertMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)goBackMenu,false);
			gtk_statusbar_remove_all((GtkStatusbar*)statusWidget,0);
		}
	else
		{
			gtk_widget_set_sensitive((GtkWidget*)goBackMenu,history->canGoBack());
			text=gtk_label_get_text((GtkLabel*)page->tabName);
//menu
			gtk_widget_set_sensitive((GtkWidget*)undoMenu,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoMenu,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)undoAllMenu,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoAllMenu,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)saveMenu,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
			gtk_widget_set_sensitive((GtkWidget*)goBackMenu,history->canGoBack());
//tab
			if(text[0]=='*')
				offset=1;

			if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer))==true)
				asprintf(&newlabel,"*%s",&text[offset]);
			else
				newlabel=strdup(&text[offset]);

			gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)newlabel);
			ERRDATA debugFree(&newlabel);
			gtk_widget_set_sensitive((GtkWidget*)cutMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)copyMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)pasteMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)bookMarkMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)navMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)printMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)closeMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)closeAllMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)saveAllMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)revertMenu,true);
			gtk_widget_show_all(page->tabName);
			updateStatusBar((GtkTextBuffer*)page->buffer,NULL,NULL,page);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start_find);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end_find);
			gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&start_find,&end_find);
		}
//do plugin sensitive
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"setSensitive");
}

VISIBLE void closeTab(GtkWidget* widget,gpointer data)
{
	ERRDATA
	long		thispage;
	int			result;
	pageStruct*	page;
	GList*		ptr;
	bool		changed=false;
	GtkWidget*	menuitem;

	if(sessionBusy==true)
		return;

	busyFlag=true;	

	if(closingAll==true)
		thispage=0;
	else
		{
			if(data==NULL)
				thispage=gtk_notebook_get_current_page(mainNotebook);
			else
				thispage=gtk_notebook_page_num(mainNotebook,(GtkWidget *)data);
		}

	if((thispage<0) || (thispage>gtk_notebook_get_n_pages(mainNotebook)))
		return;

	closingAll=false;
	page=getPageStructPtr(thispage);
	if(page==NULL)
		return;

	if(gtk_text_buffer_get_modified((GtkTextBuffer*)page->buffer))
		{
			result=show_question(page->fileName);
			switch(result)
				{
				case GTK_RESPONSE_YES:
					if(saveFile(NULL,NULL)==false)
						return;
					break;
				case GTK_RESPONSE_NO:
					break;
				default:
					return;
					break;
				}
		}

	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"closeFile");

	rebuildBookMarkMenu();
	while(changed==false)
		{
			changed=true;
			ptr=newBookMarksList;
			while(ptr!=NULL)
				{
					if(((bookMarksNew*)ptr->data)->page==page)
						{
							ERRDATA debugFree(&((bookMarksNew*)ptr->data)->markName);
							ERRDATA debugFree(&((bookMarksNew*)ptr->data)->label);
							ERRDATA debugFree((char**)&ptr->data);
							changed=false;
							newBookMarksList=g_list_remove_link(newBookMarksList,ptr);
						}
					ptr=g_list_next(ptr);
				}
		}

	ptr=newBookMarksList;
	while(ptr!=NULL)
		{
			menuitem=gtk_menu_item_new_with_label(((bookMarksNew*)ptr->data)->label);
			gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(jumpToMark),(void*)ptr->data);
			ptr=g_list_next(ptr);
		}
	gtk_widget_show_all(bookMarkMenu);

	if(page->filePath!=NULL)
		ERRDATA debugFree(&(page->filePath));
	if(page->fileName!=NULL)
		ERRDATA debugFree(&(page->fileName));

	ERRDATA debugFree(&(page->dirName));
	ERRDATA debugFree(&(page->realFilePath));
	if(page->markList!=NULL)
		g_list_free_full(page->markList,free);
	page->markList=NULL;

	if(page->userDataList!=NULL)
		g_list_free_full(page->userDataList,free);
	page->userDataList=NULL;

	if(page->regexList!=NULL)
		g_slist_free_full(page->regexList,free);
	page->regexList=NULL;

	if(page->monitor!=NULL)
		g_clear_object(&(page->monitor));
	if(page->gFile!=NULL)
		g_clear_object(&(page->gFile));

	currentPage--;

	gtk_notebook_remove_page(mainNotebook,thispage);
//TODO//
//	ERRDATA debugFree((char**)&page);
	busyFlag=false;
	if(closingAll==false)
		{
			currentTabNumber=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook);
			resetWidgetSenisitive();
		}
}

VISIBLE void closeAllTabs(GtkWidget* widget,gpointer data)
{
	ERRDATA
	int	numtabs=gtk_notebook_get_n_pages(mainNotebook);

	for(int loop=0; loop<numtabs; loop++)
		{
			closingAll=true;
			closeTab(NULL,0);
		}

//rebuild bookmark menu
	closingAll=false;
	rebuildBookMarkMenu();
	gtk_widget_show_all(bookMarkMenu);
	setWidgets();
}

void sortTabs(GtkWidget* widget,gpointer data)
{
	ERRDATA
	bool		flag=true;
	pageStruct	*page1=NULL;
	pageStruct	*page2=NULL;

	doUpdateWidgets=false;

	while(flag==true)
		{
			flag=false;
			for (int j=0;j<gtk_notebook_get_n_pages(mainNotebook)-1;j++)
				{
					page1=getPageStructPtr(j);
					page2=getPageStructPtr(j+1);
					if(strcmp(page2->fileName,page1->fileName)<0)
						{
							flag=true;
							gtk_notebook_reorder_child((GtkNotebook*)mainNotebook,page2->tabVbox,j);
						}
				}
		}

	rebuildTabsMenu();
	doUpdateWidgets=true;
	resetWidgetSenisitive();
}

VISIBLE void switchPage(GtkNotebook *notebook,gpointer arg1,guint thispage,gpointer user_data)
{
	ERRDATA
	pageStruct*	page;
	char*		functions=NULL;
	GtkWidget*	menuitem;
	int			linenum=-1;
	char*		ts=NULL;

	char*		lineptr=NULL;
	bool		onefunc=false;
	int			numtypes=0;
	char*		typenames[50]= {NULL,};
	bool		flag;
	char*		newstr=NULL;
	GtkWidget*	whattypemenu;
	GtkWidget*	typesubmenus[50]= {NULL,};
	GtkWidget*	submenu;
	char*		correctedstr=NULL;

	if(doUpdateWidgets==false)
		return;

	if(arg1==NULL)
		return;

	page=(pageStruct*)g_object_get_data((GObject*)arg1,"pagedata");
	if(page==NULL)
		{
			setWidgets();
			return;
		}

	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)funcMenu);
	if (submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)funcMenu,NULL);

	currentTabNumber=thispage;

	page->rebuildMenu=false;

	getRecursiveTagList(page->filePath,&functions);
	lineptr=functions;

	page->isFirst=true;
	page->navSubMenu=(GtkMenuItem*)gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(funcMenu),(GtkWidget*)page->navSubMenu);

	while ((lineptr!=NULL) && (strlen(lineptr)>0))
		{
			linenum=-1;
			correctedstr=NULL;
			ts=NULL;
			sscanf (lineptr,"%*s %*s %i %a[^\n]s",&linenum,&ts);
			if(linenum==-1)
				{
					ERRDATA debugFree(&correctedstr);
					ERRDATA debugFree(&ts);
				}
			else
				{
					correctedstr=truncateWithElipses(ts,maxFuncDefs);
				}

			if(linenum>0)
				{
					if(listFunction==4)
						{
							newstr=NULL;
							newstr=globalSlice->sliceBetween(lineptr,(char*)" ",(char*)" ");
							if(newstr!=NULL)
								{
									flag=false;
									for(int j=0; j<numtypes; j++)
										{
											if (strcmp(newstr,typenames[j])==0)
												{
													whattypemenu=typesubmenus[j];
													flag=true;
													break;
												}
										}

									if(flag==false)
										{
											typenames[numtypes]=strdup(newstr);
											ERRDATA debugFree(&newstr);
											if(typenames[numtypes][strlen(typenames[numtypes])-1]=='s')
												asprintf(&newstr,"%s's",typenames[numtypes]);
											else
												asprintf(&newstr,"%ss",typenames[numtypes]);
											newstr[0]=toupper(newstr[0]);
											submenu=gtk_image_menu_item_new_with_label(newstr);
											typesubmenus[numtypes]=gtk_menu_new();
											gtk_menu_item_set_submenu(GTK_MENU_ITEM(submenu),typesubmenus[numtypes]);
											gtk_menu_shell_append(GTK_MENU_SHELL(page->navSubMenu),submenu);
											whattypemenu=typesubmenus[numtypes];
											numtypes++;
										}

									ERRDATA debugFree(&newstr);

									onefunc=true;
									menuitem=gtk_image_menu_item_new_with_label(correctedstr);
									gtk_menu_shell_append(GTK_MENU_SHELL(whattypemenu),menuitem);
									g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(gotoLine),(void*)(long)linenum);
								}
						}
					else
						{
							onefunc=true;
							menuitem=gtk_image_menu_item_new_with_label(correctedstr);
							gtk_menu_shell_append(GTK_MENU_SHELL(page->navSubMenu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(gotoLine),(void*)(long)linenum);
						}
				}
			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;
			ERRDATA debugFree(&correctedstr);
			ERRDATA debugFree(&ts);
		}

	gtk_window_set_title((GtkWindow*)mainWindow,page->fileName);
	refreshMainWindow();
	if(functions!=NULL)
		ERRDATA debugFree(&functions);

	gtk_widget_set_sensitive((GtkWidget*)funcMenu,onefunc);

	rebuildTabsMenu();

	setSensitive();

	createCompletion(page);
//plug switch tab
	globalPlugins->globalPlugData->page=page;
	globalPlugins->globalPlugData->currentTab=currentTabNumber;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"switchTab");
}

VISIBLE void copyToClip(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_copy_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
}

VISIBLE void cutToClip(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_cut_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),true);
	setSensitive();
}

VISIBLE void pasteFromClip(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*		page=getPageStructPtr(-1);
	char*			clipdata=NULL;
	GtkClipboard*	mainclipboard;
	GtkTextIter		start;
	GtkTextIter		end;

	mainclipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	clipdata=gtk_clipboard_wait_for_text(mainclipboard);
	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);
		if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
			gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&start,&end);
		gtk_text_buffer_insert_at_cursor((GtkTextBuffer*)page->buffer,(const gchar*)clipdata,-1);
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
	setSensitive();
}

VISIBLE void undo(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(-1);

	if(page!=NULL)
		{
			doBusy(true,page);
			if(gtk_source_buffer_can_undo(page->buffer));
				{
					gtk_source_buffer_undo(page->buffer);
					page->isFirst=true;
					setSensitive();
				}
			doBusy(false,page);
		}
}

VISIBLE void unRedoAll(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(-1);
	if(page!=NULL)
		{
		 	doBusy(true,page);
			if((long)data==0)
				{
					if(gtk_source_buffer_can_undo(page->buffer)==true)
						{
							while(gtk_source_buffer_can_undo(page->buffer)==true)
								{
									if(gtk_source_buffer_can_undo(page->buffer)==true)
										gtk_source_buffer_undo(page->buffer);
								}
						}
				}
			else
				{
					if(gtk_source_buffer_can_redo(page->buffer)==true)
						{
							while(gtk_source_buffer_can_redo(page->buffer)==true)
								{
									if(gtk_source_buffer_can_redo(page->buffer)==true)
										gtk_source_buffer_redo(page->buffer);
								}
						}
				}
			page->isFirst=true;
			setSensitive();
		 	doBusy(false,page);
		}
}

VISIBLE void redo(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	page=getPageStructPtr(-1);

	if(page!=NULL)
		{
		 	doBusy(true,page);
			if(gtk_source_buffer_can_redo(page->buffer));
				{
					gtk_source_buffer_redo(page->buffer);
					page->isFirst=true;
					setSensitive();
				}
		 	doBusy(false,page);
		}
}

VISIBLE void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	ERRDATA
	gchar**	array=gtk_selection_data_get_uris(selection_data);
	int		cnt=g_strv_length(array);
	char*	filename;

	for(int j=0; j<cnt; j++)
		{
			filename=g_filename_from_uri(array[j],NULL,NULL);
			openFile(filename,0,true);
		}
	setWidgets();
	g_strfreev(array);
}

void externalTool(GtkWidget* widget,gpointer data)
{
printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>in external\n");
	ERRDATA
	toolStruct*		tool=(toolStruct*)data;
	pageStruct*		page=getPageStructPtr(-1);
	char*			docdirname=NULL;
	char*			tooldirname=NULL;
	char*			text=NULL;
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	const char*		vars[]= {"%t","%f","%d","%i","%h","%l",NULL};
	char*			ptr;
	long			pos;
	int				loop=0;
	GString*		tempCommand=NULL;
	bool			continueflag=false;
	char*			barcontrol=NULL;
	StringSlice*	slice=new StringSlice;
	char*			barcommand=NULL;
	char			*strarray=NULL;
	unsigned int	buffersize=1000;
	char			*pagepath=NULL;

	if(page==NULL || tool==NULL)
		return;

	const char*		varData[]= {NULL,page->filePath,NULL,DATADIR,htmlFile,page->lang};

	tempCommand=g_string_new(tool->command);

	if(page->filePath!=NULL)
		docdirname=g_path_get_dirname(page->filePath);
	else
		docdirname=strdup(getenv("HOME"));

	tooldirname=g_path_get_dirname(tool->filePath);
	chdir(tooldirname);

	strarray=(char*)calloc(buffersize,1);
	for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
		{
			pagepath=(char*)(pageStruct*)(getPageStructPtr(j))->filePath;
			if(pagepath!=NULL)
				{
					if(buffersize<strlen(strarray)+strlen(pagepath))
						{
							buffersize+=1000;
							strarray=(char*)realloc(strarray,buffersize);
						}
					if(pagepath!=NULL)
						{
							strcat(strarray,pagepath);
							strcat(strarray,";");
						}
				}
		}

	if(pagepath!=NULL)
		{
			if(page->filePath!=NULL)
				setenv("KKEDIT_CURRENTFILE",page->filePath,1);
			if(htmlFile!=NULL)
				setenv("KKEDIT_HTMLFILE",htmlFile,1);
			if(docdirname!=NULL)
				setenv("KKEDIT_CURRENTDIR",docdirname,1);
			if(DATADIR!=NULL)
				setenv("KKEDIT_DATADIR",DATADIR,1);
			if(page->lang!=NULL)
				setenv("KKEDIT_SOURCE_LANG",page->lang,1);
			if(strarray!=NULL)
				setenv("KKEDIT_FILE_LIST",strarray,1);
			free(strarray);
			asprintf(&barcontrol,"%s/BarControl-%s",tmpFolderName,slice->randomName(6));

			if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
				{
					selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
					setenv("KKEDIT_SELECTION",selection,1);
				}
			else
				selection=strdup("");

			varData[0]=selection;
			varData[2]=docdirname;

			continueflag=false;
			while(continueflag==false)
				{
					continueflag=true;
					loop=0;
					while(vars[loop]!=NULL)
						{
							ptr=strstr(tempCommand->str,vars[loop]);
							if(ptr!=NULL)
								{
									pos=(long)ptr-(long)tempCommand->str;
									tempCommand=g_string_erase(tempCommand,pos,2);
									tempCommand=g_string_insert(tempCommand,pos,varData[loop]);
									continueflag=false;
								}
							loop++;
						}
				}

			if(tool->clearView==true)
				gtk_text_buffer_set_text(toolOutputBuffer,"",0);

			if(tool->useBar==true)
				{
					setenv("KKEDIT_BAR_CONTROL",barcontrol,1);
					asprintf(&barcommand,POLEPATH " \"%s\" \"%s\" &",tool->menuName,barcontrol);
					system(barcommand);
				}

			runCommand(tempCommand->str,&text,tool->inTerminal,tool->flags,tool->runAsRoot,tool->menuName);
			ERRDATA debugFree(&selection);

			if(text!=NULL)
				{
					if(tool->flags & TOOL_REPLACE_OP)
						{
							gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
							gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
							gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (page->buffer),&start);
							gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&start,text,strlen(text));
						}

					if(tool->flags & TOOL_PASTE_OP)
						{
							if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
								gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
							gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(page->buffer),text,strlen(text));
						}
				}

			unsetenv("KKEDIT_CURRENTFILE");
			unsetenv("KKEDIT_CURRENTDIR");
			unsetenv("KKEDIT_DATADIR");
			unsetenv("KKEDIT_SELECTION");
			unsetenv("KKEDIT_HTMLFILE");
			unsetenv("KKEDIT_BAR_CONTROL");
			unsetenv("KKEDIT_FILE_LIST");
		}

	ERRDATA debugFree(&text);
	ERRDATA debugFree(&docdirname);
	ERRDATA debugFree(&tooldirname);
	if(barcommand!=NULL)
		{
			ERRDATA debugFree(&barcommand);
			asprintf(&barcommand,"echo quit>%s",barcontrol);
			system(barcommand);
			ERRDATA debugFree(&barcommand);
		}
	ERRDATA debugFree(&barcontrol);
	delete slice;
}

VISIBLE void openHelp(GtkWidget* widget,gpointer data)
{
	ERRDATA
	const char* lang;

	if(strncmp(localeLang,"en",2)==0)
		lang="en";
	else
		lang="fr";

	asprintf(&thePage,"file://%s/help/help.%s.html",DATADIR,lang);
#ifdef _BUILDDOCVIEWER_
	showDocView(USEURI,(char*)"KKEdit",gettext("KKEdit Help"));
#else
	asprintf(&thePage,"%s %s/help/help.%s.html",browserCommand,DATADIR,lang);
	runCommand(thePage,NULL,false,8,0,(char*)gettext("KKEdit Help"));
	ERRDATA debugFree((char**)&thePage);
	thePage=NULL;
#endif
}

void copyToClipboard(GtkWidget* widget,gpointer data)
{
	ERRDATA
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(clipboard,(char*)data,-1);
}

VISIBLE void addtoCustomWordList(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*		page=(pageStruct*)data;
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	char*			command;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	asprintf(&command,"echo '%s'|cat - %s/%s|sort -u -o %s/%s.tmp;mv %s/%s.tmp %s/%s",selection,getenv("HOME"),CUSTOMWORDFILE,getenv("HOME"),CUSTOMWORDFILE,getenv("HOME"),CUSTOMWORDFILE,getenv("HOME"),CUSTOMWORDFILE);
	system(command);
	ERRDATA debugFree(&command);
	ERRDATA debugFree(&selection);
	createCompletion(page);
}

void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data)
{
	ERRDATA
	pageStruct*		page=getPageStructPtr(-1);
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	GtkWidget*		menuitem;
	GtkWidget*		image;
	GList*			ptr;
	functionData*	fdata;
	char*			temptext=NULL;

	if(sessionBusy==true)
		return;

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
					fdata=getFunctionByNameOpenFiles(selection,false,false);
					if(fdata!=NULL)
						{
							temptext=truncateWithElipses(fdata->define,maxFuncDefs);
							menuitem=gtk_menu_item_new_with_label(temptext);
							ERRDATA debugFree(&temptext);
							sprintf((char*)&defineText,"%s",fdata->define);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(copyToClipboard),(void*)defineText);
							destroyData(fdata);

							image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_MENU);
							menuitem=gtk_image_menu_item_new_with_label(gettext("Go To Definition"));
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(goToDefinition),NULL);
						}

					if(gotDoxygen==0)
						{
							menuitem=gtk_image_menu_item_new_with_label(gettext("Find In Documentation"));
							image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doxyDocs),NULL);
						}
					menuitem=gtk_image_menu_item_new_with_label(gettext("Search In Qt5 Docs"));
					image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(searchQT5Docs),NULL);

					menuitem=gtk_image_menu_item_new_with_label(gettext("Search In Gtk Docs"));
					image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(searchGtkDocs),NULL);

					menuitem=gtk_image_menu_item_new_with_label(gettext("Add To Custom Word List"));
					image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(addtoCustomWordList),(void*)page);

#ifdef _ASPELL_
//spell check
					if((spellChecker!=NULL) && (aspellConfig!=NULL))
						{
							menuitem=gtk_image_menu_item_new_with_label(gettext("Check Spellling"));
							image=gtk_image_new_from_stock(GTK_STOCK_SPELL_CHECK,GTK_ICON_SIZE_MENU);
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(checkWord),NULL);
						}
#endif
					menuitem=gtk_separator_menu_item_new();
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

					ptr=toolsList;
					while(ptr!=NULL)
						{
							if((((toolStruct*)ptr->data)->inPopUp==true) && (((toolStruct*)ptr->data)->alwaysPopup==false))
								{
									menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
									if(((toolStruct*)ptr->data)->comment!=NULL)
										gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);

									gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
									g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
								}
							ptr=g_list_next(ptr);
						}
				}
		}

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	ptr=toolsList;
	while(ptr!=NULL)
		{
			if((((toolStruct*)ptr->data)->alwaysPopup==true))
				{
					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
					if(((toolStruct*)ptr->data)->comment!=NULL)
						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
				}
			ptr=g_list_next(ptr);
		}

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_menu_item_new_with_label(gettext("Toggle Bookmark"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(toggleBookmark),NULL);

	globalPlugins->globalPlugData->contextPopUpMenu=(GtkWidget*)menu;

	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"addToContext");

	gtk_widget_show_all((GtkWidget*)menu);
}

void doTabMenu(GtkWidget *widget,gpointer user_data)
{
	ERRDATA
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	gtk_clipboard_set_text(clipboard,(char*)user_data,-1);
	gtk_widget_destroy(tabMenu);
}

gboolean whatPane(GtkWidget *widget,GdkEvent *event,gpointer data)
{
	ERRDATA
	pageStruct* page=(pageStruct*)getPageStructPtr(-1);

	if(sessionBusy==true)
		return(true);

	if((long)data==1)
		page->inTop=true;
	else
		page->inTop=false;

	autoSeleced=false;
	return(false);
}

void doSplitView(GtkWidget *widget,gpointer user_data)
{
	ERRDATA
	pageStruct* page=(pageStruct*)user_data;

	if(gtk_paned_get_child2((GtkPaned*)page->pane)==NULL)
		{
			page->pageWindow2=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

			page->view2 = (GtkSourceView*)gtk_source_view_new_with_buffer (page->buffer);
			g_signal_connect(G_OBJECT(page->view2),"populate-popup",G_CALLBACK(populatePopupMenu),NULL);
			setFilePrefs(page);

			gtk_paned_add2(GTK_PANED(page->pane),(GtkWidget*)page->pageWindow2);
			gtk_container_add(GTK_CONTAINER((GtkWidget*)page->pageWindow2),(GtkWidget*)page->view2);
			g_signal_connect(G_OBJECT(page->view2),"button-release-event",G_CALLBACK(whatPane),(void*)2);
			page->isSplit=true;
		}
	else
		{
			page->isSplit=false;
			gtk_widget_destroy(gtk_paned_get_child2((GtkPaned*)page->pane));
		}

	gtk_widget_show_all(page->pane);
}

void changeSourceStyle(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*					page=getPageStructPtr(-1);
	GtkSourceLanguageManager*	lm=gtk_source_language_manager_get_default();
	const gchar* const*			ids=gtk_source_language_manager_get_language_ids(lm);
	GtkSourceLanguage*			lang=gtk_source_language_manager_get_language(lm,ids[(long)data]);
	const char*					langname=gtk_source_language_get_name(lang);

	gtk_source_buffer_set_language(page->buffer,lang);
	page->lang=langname;
}

void openFromTab(GtkMenuItem* widget,pageStruct* page)
{
	ERRDATA
	char*		filepath=NULL;

	openInThisTab=gtk_notebook_get_current_page(mainNotebook);
	asprintf(&filepath,"%s/%s",page->dirName,gtk_menu_item_get_label(widget));
	openFile(filepath,0,true);
	openInThisTab=-1;
	ERRDATA debugFree(&filepath);
}

bool tabPopUp(GtkWidget *widget, GdkEventButton *event,gpointer user_data)
{
	ERRDATA
	pageStruct*					page;
	GtkWidget*					menuitem;
	GtkWidget*					image;
	GtkWidget*					submenu;
	GtkWidget*					menuids;
	GtkSourceLanguageManager*	lm;
	const gchar* const*			ids;
	int							cnt=0;
	char*						idsort[1000];
	int							idnum[1000];
	char*						holdstr=NULL;
	int							holdidnum;
	bool						flag=true;
	GtkSourceLanguage*			lang;
	const char*					langname;
	FILE*						fp=NULL;
	char*						command;
	char						line[2048];
	char*						name;

	if(sessionBusy==true)
		return(false);

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			tabMenu=gtk_menu_new();
			page=(pageStruct*)user_data;

//copy dirname
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label(gettext("Copy Folder Path"));
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->dirName);
//copy filepath
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label(gettext("Copy Filepath"));
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->filePath);
//copy filename
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label(gettext("Copy FileName"));
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->fileName);

#ifdef _ASPELL_
//check document
			if((spellChecker!=NULL) && (aspellConfig!=NULL) && (gtk_text_buffer_get_modified((GtkTextBuffer*)page->buffer)==false))
				{
					image=gtk_image_new_from_stock(GTK_STOCK_SPELL_CHECK,GTK_ICON_SIZE_MENU);
					menuitem=gtk_image_menu_item_new_with_label(gettext("Spell Check Document"));
					gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
					gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doSpellCheckDoc),(void*)page->filePath);
				}
#endif

//paned view
			image=gtk_image_new_from_stock(GTK_STOCK_NEW,GTK_ICON_SIZE_MENU);
			if(page->isSplit==true)
				menuitem=gtk_image_menu_item_new_with_label(gettext("Un-Split View"));
			else
				menuitem=gtk_image_menu_item_new_with_label(gettext("Split View"));

			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doSplitView),(void*)page);

//source highlighting
			lm=gtk_source_language_manager_get_default();
			ids=gtk_source_language_manager_get_language_ids(lm);

			image=gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label(gettext("Source Highlight"));
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);

			submenu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);

			cnt=0;
			while(ids[cnt]!=NULL)
				{
					idsort[cnt]=strdup(ids[cnt]);
					idnum[cnt]=cnt;
					cnt++;
				}
			idsort[cnt]=NULL;

			flag=true;
			while(flag==true)
				{
					flag=false;
					for(int j=0; j<cnt; j++)
						{
							if((idsort[j+1]!=NULL) && (strcmp(idsort[j],idsort[j+1])>0))
								{
									flag=true;
									holdstr=idsort[j];
									idsort[j]=idsort[j+1];
									idsort[j+1]=holdstr;
									holdidnum=idnum[j];
									idnum[j]=idnum[j+1];
									idnum[j+1]=holdidnum;
								}
						}
				}

			for(int j=0; j<cnt; j++)
				{
					lang=gtk_source_language_manager_get_language(lm,idsort[j]);
					if(gtk_source_language_get_hidden(lang)!=true)
						{
							langname=gtk_source_language_get_name(lang);

							if((page->lang!=NULL) && (strcmp(page->lang,langname)==0))
								{
									image=gtk_image_new_from_stock(GTK_STOCK_APPLY,GTK_ICON_SIZE_MENU);
									menuids=gtk_image_menu_item_new_with_label(langname);
									gtk_image_menu_item_set_image((GtkImageMenuItem *)menuids,image);
								}
							else
								{
									menuids=gtk_menu_item_new_with_label(langname);
								}

							g_signal_connect(G_OBJECT(menuids),"activate",G_CALLBACK(changeSourceStyle),(void*)(long)idnum[j]);
							gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuids);
						}
				}
//add files to tab
			menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);

			submenu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);

			asprintf(&command,"ls --color=never -Bp  \"%s\" | grep -v /|sort|sed -n '/^.*[^\\.o]$/p'",page->dirName);
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					while(fgets(line,2048,fp))
						{
							line[strlen(line)-1]=0;
							name=basename(line);
							menuids=gtk_image_menu_item_new_with_label(strdup(name));
							gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuids);
							g_signal_connect(G_OBJECT(menuids),"activate",G_CALLBACK(openFromTab),(void*)page);
						}
					fclose(fp);
				}
			ERRDATA debugFree(&command);

			gtk_widget_show_all(menuitem);

//do plugs
			globalPlugins->globalPlugData->tabPopUpMenu=tabMenu;
			globalPlugins->globalPlugData->page=page;
			g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"addToTab");

			gtk_menu_attach_to_widget(GTK_MENU(tabMenu),widget,NULL);
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);

			return(true);
		}
	else
		return(false);
}

void writeExitData(void)
{
	ERRDATA
	GtkAllocation	alloc;
	char*			filename;
	int				winx;
	int				winy;
	
	gtk_widget_get_allocation(mainWindow,&alloc);
	gtk_window_get_position((GtkWindow*)mainWindow,&winx,&winy);
	asprintf(&windowAllocData,"%i %i %i %i",alloc.width,alloc.height,winx,winy);

#ifdef _BUILDDOCVIEWER_
	if(gtk_widget_get_realized(docView)==true)
		{
			gtk_widget_get_allocation(docView,&alloc);
			gtk_window_get_position((GtkWindow*)docView,&winx,&winy);
		}
	else
		{
			alloc.width=docWindowWidth;
			alloc.height=docWindowHeight;
			winx=docWindowX;
			winy=docWindowY;
		}
#endif
	asprintf(&docWindowAllocData,"%i %i %i %i",alloc.width,alloc.height,winx,winy);

	toolOutHeight=gtk_paned_get_position((GtkPaned*)mainVPane);
	bottomVPaneHite=gtk_paned_get_position((GtkPaned*)mainWindowVPane);
	topVPaneHite=gtk_paned_get_position((GtkPaned*)secondWindowVPane);

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	ERRDATA debugFree(&filename);
	asprintf(&filename,"%s/.KKEdit/kkedit.window.rc",getenv("HOME"));

	saveVarsToFile(filename,kkedit_window_rc);

	ERRDATA debugFree(&filename);
	ERRDATA debugFree(&windowAllocData);
}

void writeConfig(void)
{
	ERRDATA
	char*			filename;

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	ERRDATA debugFree(&filename);

	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	saveVarsToFile(filename,kkedit_rc);
	ERRDATA debugFree(&filename);
}

VISIBLE bool doSaveAll(GtkWidget* widget,gpointer data)
{
	ERRDATA
	int			numpages=gtk_notebook_get_n_pages(mainNotebook);
	int			result;
	pageStruct*	page;

	for(int loop=0; loop<numpages; loop++)
		{
			page=getPageStructPtr(loop);
			if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)))
				{
					if((bool)data==true)
						{
							result=show_question(g_path_get_basename(page->fileName));
							switch(result)
								{
								case GTK_RESPONSE_YES:
									gtk_notebook_set_current_page(mainNotebook,loop);
									saveFile(NULL,NULL);
									break;
								case GTK_RESPONSE_NO:
									break;
								case GTK_RESPONSE_CANCEL:
									return(false);
									break;

								default:
									break;
								}
						}
					else
						{
							gtk_notebook_set_current_page(mainNotebook,loop);
							saveFile(NULL,NULL);
						}
				}
		}
	return(true);
}

bool doShutdown(GtkWidget* widget,GdkEvent *event,gpointer data)
{
	ERRDATA
	char*	command;

	if(doSaveAll(widget,(void*)true)==false)
		{
			gtk_widget_show(mainWindow);
			return(true);
		}

	if(onExitSaveSession)
		saveSession(NULL,NULL);
	writeExitData();

#ifdef _ASPELL_
	delete_aspell_config(aspellConfig);
	delete_aspell_speller(spellChecker);
#endif

	g_list_foreach(globalPlugins->plugins,releasePlugs,NULL);

	asprintf(&command,"rm -rf %s",tmpFolderName);
	system(command);
	ERRDATA debugFree(&command);
	system("rmdir /tmp/icedteaplugin-* 2>/dev/null");

	g_application_release(mainApp);
	return(false);
}

void setPrefs(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*	tpage=getPageStructPtr(-1);
	bool*		bools[MAXPREFSWIDGETS]={&indent,&lineNumbers,&lineWrap,&highLight,&noSyntax,&singleUse,&onExitSaveSession,&restoreBookmarks,&noDuplicates,&noWarnings,&readLinkFirst,&autoShowComps,&autoCheck,&nagScreen,&useGlobalPlugMenu};
	unsigned int*		ints[MAXPREFSINTWIDGETS]={&maxTabChars,&maxFRHistory,&depth,&autoShowMinChars,&tabWidth,&maxFuncDefs,&maxBMChars};

	if(strcmp(gtk_widget_get_name(widget),"style")==0)
		{
			if(tmpStyleName!=NULL)
				ERRDATA debugFree(&tmpStyleName);
			tmpStyleName=gtk_combo_box_text_get_active_text((GtkComboBoxText*)data);
			if(tpage!=NULL)
				gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)tpage->buffer,gtk_source_style_scheme_manager_get_scheme(schemeManager,tmpStyleName));
		}

	if(strcmp(gtk_widget_get_name(widget),"save")==0)
		{
			gtk_widget_set_sensitive(prefsWidgets[AUTOBM],gtk_toggle_button_get_active((GtkToggleButton*)prefsWidgets[AUTOSAVE]));
			return;
		}

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		{
			if(tpage!=NULL)
				gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)tpage->buffer,styleScheme);
			gtk_widget_destroy(prefswin);
		}

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			for(int j=0;j<MAXPREFSWIDGETS;j++)
				*bools[j]=gtk_toggle_button_get_active((GtkToggleButton*)prefsWidgets[j]);

			for(int j=0;j<MAXPREFSINTWIDGETS;j++)
				*ints[j]=gtk_adjustment_get_value((GtkAdjustment*)prefsIntWidgets[j]);

			if(styleName!=NULL)
				{
					ERRDATA debugFree(&styleName);
					styleName=strdup(tmpStyleName);
				}

			if(highlightColour!=NULL)
				{
					GdkColor	colour;
					ERRDATA debugFree(&highlightColour);
					gtk_color_button_get_color((GtkColorButton*)bmHighlightBox,&colour);
					highlightColour=gdk_color_to_string(&colour);
				}

			if(terminalCommand!=NULL)
				{
					ERRDATA debugFree(&terminalCommand);
					terminalCommand=strdup(gtk_entry_get_text((GtkEntry*)terminalBox));
				}

			if(rootCommand!=NULL)
				{
					ERRDATA debugFree(&rootCommand);
					rootCommand=strdup(gtk_entry_get_text((GtkEntry*)rootCommandBox));
				}

			if(fontAndSize!=NULL)
				{
					ERRDATA debugFree(&fontAndSize);
					fontAndSize=strdup(gtk_font_button_get_font_name((GtkFontButton*)fontButton));
				}

			if(browserCommand!=NULL)
				{
					ERRDATA debugFree(&browserCommand);
					browserCommand=strdup(gtk_entry_get_text((GtkEntry*)defaultBrowserBox));
				}

			if(toolBarLayout!=NULL)
				{
					ERRDATA debugFree(&toolBarLayout);
					toolBarLayout=makeToolBarList();
					gtk_widget_destroy((GtkWidget*)toolBar);
					toolBar=(GtkToolbar*)gtk_toolbar_new();
					gtk_box_pack_start(GTK_BOX(toolBarBox),(GtkWidget*)toolBar,true,true,0);
					setUpToolBar();
				}

			listFunction=gtk_combo_box_get_active((GtkComboBox*)funcListDrop);

			if(tpage!=NULL)
				switchPage(mainNotebook,tpage->tabVbox,currentTabNumber,NULL);

			gtk_widget_destroy(prefswin);
			resetAllFilePrefs();
			writeConfig();
			setSensitive();
			refreshMainWindow();
		}
}

void setToolOptions(GtkWidget* widget,gpointer data)
{
	ERRDATA
	char*		dirname;
	char*		text;
	char*		toolpath;
	int			flags;

	asprintf(&dirname,"%s/.KKEdit/tools",getenv("HOME"));
	text=strdup(gtk_entry_get_text((GtkEntry*)toolNameWidget));
	text=g_strdelimit(text," ",'-');
	asprintf(&toolpath,"%s/.KKEdit/tools/%s",getenv("HOME"),text);

	if((gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false) || (gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget)==true))
		{
			gtk_widget_set_sensitive(ignoreWidget,false);
			gtk_widget_set_sensitive(pasteWidget,false);
			gtk_widget_set_sensitive(replaceWidget,false);
			gtk_widget_set_sensitive(outputWidget,false);
		}
	else
		{
			gtk_widget_set_sensitive(ignoreWidget,true);
			gtk_widget_set_sensitive(pasteWidget,true);
			gtk_widget_set_sensitive(replaceWidget,true);
			gtk_widget_set_sensitive(outputWidget,true);
		}

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			if(gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false)
				flags=TOOL_ASYNC;
			else
				{
					if(gtk_toggle_button_get_active((GtkToggleButton*)ignoreWidget)==true)
						flags=TOOL_IGNORE_OP;
					if(gtk_toggle_button_get_active((GtkToggleButton*)pasteWidget)==true)
						flags=TOOL_PASTE_OP;
					if(gtk_toggle_button_get_active((GtkToggleButton*)replaceWidget)==true)
						flags=TOOL_REPLACE_OP;
					if(gtk_toggle_button_get_active((GtkToggleButton*)outputWidget)==true)
						flags=TOOL_VIEW_OP;
				}
			if(gtk_toggle_button_get_active((GtkToggleButton*)showDocWidget)==true)
				flags=flags+TOOL_SHOW_DOC;


			intermarg=(int)gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget);
			flagsarg=flags;
			inpopup=(int)gtk_toggle_button_get_active((GtkToggleButton*)inPopupWidget);
			alwayspopup=(int)gtk_toggle_button_get_active((GtkToggleButton*)alwaysPopupWidget);
			clearview=(int)gtk_toggle_button_get_active((GtkToggleButton*)clearViewWidget);
			commandarg=(char*)gtk_entry_get_text((GtkEntry*)commandLineWidget);
			commentarg=(char*)gtk_entry_get_text((GtkEntry*)commentWidget);
			menuname=(char*)gtk_entry_get_text((GtkEntry*)toolNameWidget);
			rootarg=(int)gtk_toggle_button_get_active((GtkToggleButton*)runAsRootWidget);
			keycode=(int)gdk_keyval_from_name(gtk_entry_get_text((GtkEntry*)keyWidget));
			usebar=(int)gtk_toggle_button_get_active((GtkToggleButton*)useBarWidget);

			saveVarsToFile(toolpath,tool_vars);

			gtk_widget_hide((GtkWidget*)data);
			gtk_widget_destroy((GtkWidget*)data);
			buildTools();
			gtk_widget_show_all(toolsMenu);
		}

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		{
			gtk_widget_hide((GtkWidget*)data);
			gtk_widget_destroy((GtkWidget*)data);
		}

	if(strcmp(gtk_widget_get_name(widget),"delete")==0)
		{
			if((gtk_entry_get_text((GtkEntry*)toolNameWidget)!=NULL) && (yesNo((char*)gettext("Are you sure you want to delete"),(char*)gtk_entry_get_text((GtkEntry*)toolNameWidget))==GTK_RESPONSE_YES))
				{
					asprintf(&dirname,"rm \"%s\"",toolpath);
					system(dirname);
					gtk_widget_hide((GtkWidget*)data);
					gtk_widget_destroy((GtkWidget*)data);
					buildTools();
					gtk_widget_show_all(toolsMenu);
				}
		}

	ERRDATA debugFree(&toolpath);
	ERRDATA debugFree(&text);
	ERRDATA debugFree(&dirname);
}

VISIBLE void doAbout(GtkWidget* widget,gpointer data)
{
	ERRDATA
	const char*	authors[]= {"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,"\nBig thanks to Sadi Yumuşak for all his help.",gettext("\nMore by the same author\n"),"Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};
	const char	copyright[] ="Copyright \xc2\xa9 2013 K.D.Hedger \n" MYEMAIL;
	const char*	aboutboxstring=gettext("KKEdit Code Text Editor");
	char*		licence;
	char*		translators;

	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));
	g_file_get_contents(DATADIR"/docs/gpl-3.0.txt",&licence,NULL,NULL);

	gtk_show_about_dialog(NULL,"authors",authors,"translator-credits",translators,"comments",aboutboxstring,"copyright",copyright,"version",VERSION,"website",MYWEBSITE,"website-label","KKEdit Homepage","program-name","KKEdit","logo-icon-name","KKEdit","license",licence,NULL);

	ERRDATA debugFree(&licence);
	ERRDATA debugFree(&translators);
}

GtkSourceBuffer*	printBuffer;
GtkSourceView*		printView;

void doCombineBuffers(void)
{
	ERRDATA
	pageStruct*	page;
	GtkTextIter	iter;
	GtkTextIter	fromstart;
	GtkTextIter	fromend;

	printBuffer=gtk_source_buffer_new(NULL);
	printView=(GtkSourceView*)gtk_source_view_new_with_buffer(printBuffer);

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)printBuffer,&iter);

	for(int j=0; j<gtk_notebook_get_n_pages(mainNotebook); j++)
		{
			page=getPageStructPtr(j);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&fromstart);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&fromend);
			gtk_text_buffer_insert((GtkTextBuffer*)printBuffer,&iter,gtk_text_buffer_get_text((GtkTextBuffer *)page->buffer,&fromstart,&fromend,true),-1);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)printBuffer,&iter);
		}
}


void drawPage(GtkPrintOperation *operation,GtkPrintContext *context,gint page_nr,gpointer user_data)
{
	ERRDATA
	GtkSourcePrintCompositor *compositor;

	compositor=GTK_SOURCE_PRINT_COMPOSITOR(user_data);
	gtk_source_print_compositor_draw_page(compositor,context,page_nr);
}

void beginPrint(GtkPrintOperation *operation,GtkPrintContext *context,gpointer user_data)
{
	ERRDATA
	GtkSourcePrintCompositor*	compositor;
	gint						n_pages;

	compositor=GTK_SOURCE_PRINT_COMPOSITOR (user_data);

	while(!gtk_source_print_compositor_paginate(compositor,context));

	n_pages=gtk_source_print_compositor_get_n_pages(compositor);
	gtk_print_operation_set_n_pages(operation,n_pages);
}

VISIBLE void printFile(GtkWidget* widget,gpointer data)
{
	ERRDATA
	doCombineBuffers();
	GtkSourcePrintCompositor*	printview=gtk_source_print_compositor_new_from_view(printView);
	GtkPrintOperation*			print;
	GtkPrintOperationResult		result;

	print=gtk_print_operation_new ();
	if (settings != NULL)
		gtk_print_operation_set_print_settings(print,settings);
	g_signal_connect(print,"begin-print",G_CALLBACK(beginPrint),(void*)printview);
	g_signal_connect(print,"draw-page",G_CALLBACK (drawPage),(void*)printview);
	result=gtk_print_operation_run(print,GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,GTK_WINDOW(mainWindow),NULL);
	if (result==GTK_PRINT_OPERATION_RESULT_APPLY)
		{
			if (settings != NULL)
				g_object_unref(settings);
			settings=(GtkPrintSettings*)g_object_ref((gpointer)gtk_print_operation_get_print_settings(print));
		}
	g_object_unref(print);
	g_object_unref(printview);
}

void recentFileMenu(GtkRecentChooser* chooser,gpointer* data)
{
	ERRDATA
	gchar*	uri=NULL;
	char*	filename;

	if(data!=NULL)
		{
			openFile((char*)data,0,true);
			return;
		}

	uri=gtk_recent_chooser_get_current_uri(chooser);
	if (uri!=NULL)
		{
			filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
			openFile(filename,0,true);
			g_free (uri);
			ERRDATA debugFree(&filename);
		}
}

VISIBLE void newEditor(GtkWidget* widget,gpointer data)
{
	ERRDATA
	char*	command=NULL;

	switch((long)data)
		{
		case 1:
			if(strcmp(rootCommand,"")!=0)
				asprintf(&command,"%s kkedit -m 2>&1 >/dev/null &",rootCommand);
			else
				asprintf(&command,"%s sudo kkedit -m 2>&1 >/dev/null &",terminalCommand);
			system(command);
			ERRDATA debugFree(&command);
			break;
		case 2:
			system("kkedit -m 2>&1 >/dev/null &");
			break;
		case 3:
			if(gotManEditor==0)
				system("manpageeditor 2>&1 >/dev/null &");
			break;
		}
}

void line_mark_activated(GtkSourceGutter* gutter,GtkTextIter* iter,GdkEventButton* ev,pageStruct* page)
{
	ERRDATA
	if(sessionBusy==true)
		return;

	if(ev->button!=1)
		return;

	toggleBookmark(NULL,iter);
}

VISIBLE void showToolOutput(bool immediate)
{
	ERRDATA
	showToolOutWin=true;
	gtk_widget_show(toolOutVBox);
	gtk_menu_item_set_label((GtkMenuItem*)toolOutMenu,gettext("Hide Tool Output"));
	if(immediate==true)
		{
			while(gtk_events_pending())
				gtk_main_iteration();
		}
}

VISIBLE void hideToolOutput(bool immediate)
{
	ERRDATA
	showToolOutWin=false;;
	gtk_widget_show(toolOutVBox);
	gtk_menu_item_set_label((GtkMenuItem*)toolOutMenu,gettext("Show Tool Output"));
	if(immediate==true)
		{
			while(gtk_events_pending())
				gtk_main_iteration();
		}
}

VISIBLE void toggleToolOutput(GtkWidget* widget,gpointer data)
{
	ERRDATA
	showToolOutWin=!showToolOutWin;
	if(showToolOutWin)
		{
			gtk_widget_show(toolOutVBox);
			gtk_menu_item_set_label((GtkMenuItem*)widget,gettext("Hide Tool Output"));
		}
	else
		{
			gtk_widget_hide(toolOutVBox);
			gtk_menu_item_set_label((GtkMenuItem*)widget,gettext("Show Tool Output"));
		}
}

VISIBLE void toggleBookMarkBar(GtkWidget* widget,gpointer data)
{
	ERRDATA
	showBMBar=!showBMBar;
	if(showBMBar)
		gtk_menu_item_set_label((GtkMenuItem*)widget,gettext("Hide Bookmarks Bar"));
	else
		gtk_menu_item_set_label((GtkMenuItem*)widget,gettext("Show Bookmarks Bar"));
	resetAllFilePrefs();
}

VISIBLE void toggleToolBar(GtkWidget* widget,gpointer data)
{
	ERRDATA
	showToolBar=!showToolBar;
	if(showToolBar)
		gtk_menu_item_set_label((GtkMenuItem*)widget,gettext("Hide Tool Bar"));
	else
		gtk_menu_item_set_label((GtkMenuItem*)widget,gettext("Show Tool Bar"));
	refreshMainWindow();
}

//toggleStatusBar
VISIBLE void toggleStatusBar(GtkWidget* widget,gpointer data)
{
	ERRDATA
	showStatus=!showStatus;
	if(showStatus)
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Hide Status Bar");
	else
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Show Status Bar");
	refreshMainWindow();
}

#ifdef _BUILDDOCVIEWER_

VISIBLE void toggleDocviewer(GtkWidget* widget,gpointer data)
{
	ERRDATA
	showHideDocviewer=!showHideDocviewer;
	if(showHideDocviewer)
		{
			gtk_menu_item_set_label((GtkMenuItem*)showDocViewWidget,gettext("Hide Docviewer"));
			gtk_widget_show_all(docView);
			gtk_window_present((GtkWindow*)docView);
		}
	else
		{
			gtk_menu_item_set_label((GtkMenuItem*)showDocViewWidget,gettext("Show Docviewer"));
			gtk_widget_hide(docView);
		}
}
#endif

//toggle line numbers from view
VISIBLE void toggleLineNumbers(GtkWidget* widget,gpointer data)
{
	lineNumbers=!lineNumbers;
	resetAllFilePrefs();
}

//toggle wrap lines
VISIBLE void toggleWrapLines(GtkWidget* widget,gpointer data)
{
	lineWrap=!lineWrap;
	resetAllFilePrefs();
}

//togle no syntax
VISIBLE void toggleSyntax(GtkWidget* widget,gpointer data)
{
	noSyntax=!noSyntax;
	resetAllFilePrefs();
}

//togle no syntax
VISIBLE void toggleAutoComplete(GtkWidget* widget,gpointer data)
{
	autoShowComps=!autoShowComps;
	resetAllFilePrefs();
}

//togle hilite current line
VISIBLE void toggleHighlightCurrent(GtkWidget* widget,gpointer data)
{
	highLight=!highLight;
	resetAllFilePrefs();
}

void doKeyShortCut(int what)
{
	ERRDATA
	TextBuffer*		buf;
	char*			text;
	pageStruct*		page=getPageStructPtr(-1);
	GtkTextMark*	mark;

	if(page==NULL)
		return;

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	switch(what)
		{
//delete line ^Y
		case 0:
			gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
			break;
//delete from cursor to end of line ^k
		case 1:
			buf->getToVisibleLineEnd();
			buf->deleteFromCursor(&buf->visibleLineEnd);
			break;
//delete from cursor to beginning of line ^?
		case 2:
			buf->getToLineStart();
			buf->deleteToCursor(&buf->lineStart);
			break;
//Select Word Under Cursor
		case 3:
			if(buf->selectWord())
				buf->selectRange(&buf->lineStart,&buf->cursorPos);
			break;
//delete word under cursor ^h
		case 4:
			if(buf->selectWord())
				gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->cursorPos);
			break;
//duplictae line ^D
		case 5:
			buf->getCursorIter();
			text=buf->getSelectedText();
			gtk_text_iter_backward_lines(&buf->cursorPos,-1);
			gtk_text_buffer_begin_user_action(buf->textBuffer);
				gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
				ERRDATA debugFree(&text);
			gtk_text_buffer_end_user_action(buf->textBuffer);
			break;
//select line ^l
		case 6:
			buf->selectVisibleLine();
			break;
//Move Current Line Up ^m
		case 7:
			text=buf->getSelectedText();
			gtk_text_buffer_begin_user_action(buf->textBuffer);
				gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
				buf->getCursorIter();
				gtk_text_iter_backward_visible_line(&buf->cursorPos);
				gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
				gtk_text_iter_backward_visible_line(&buf->cursorPos);
				gtk_text_buffer_place_cursor(buf->textBuffer,&buf->cursorPos);
			gtk_text_buffer_end_user_action(buf->textBuffer);
			break;
//Move Current Line Down
		case 8:
			text=buf->getSelectedText();
			gtk_text_buffer_begin_user_action(buf->textBuffer);
				gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
				buf->getCursorIter();
				gtk_text_iter_forward_visible_line(&buf->cursorPos);
				gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
				buf->getCursorIter();
				gtk_text_iter_forward_visible_line(&buf->cursorPos);
				gtk_text_buffer_place_cursor(buf->textBuffer,&buf->cursorPos);
			gtk_text_buffer_end_user_action(buf->textBuffer);
			break;
//Select From Cursor To End Of Line
		case 9:
			buf->selectToLineEnd();
			break;
//Select From Beginning Of Line To Cursor
		case 10:
			buf->selectToLineStart();
			break;
//Move Selection Up
		case 11:
			if(gtk_text_buffer_get_selection_bounds(buf->textBuffer,&buf->lineStart,&buf->lineEnd))
				{
					gtk_text_buffer_begin_user_action(buf->textBuffer);
						gtk_text_iter_set_line_offset(&buf->lineStart,0);
						if(!gtk_text_iter_starts_line(&buf->lineEnd))
							gtk_text_iter_forward_visible_line(&buf->lineEnd);
						buf->selectRange(&buf->lineStart,&buf->lineEnd);
						text=gtk_text_buffer_get_text(buf->textBuffer,&buf->lineStart,&buf->lineEnd,true);
						gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
						buf->getCursorIter();
						gtk_text_iter_backward_visible_line(&buf->cursorPos);
						mark=gtk_text_buffer_create_mark(buf->textBuffer,"moveup",&buf->cursorPos,true);
						gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
						gtk_text_buffer_get_iter_at_mark(buf->textBuffer,&buf->cursorPos,mark);
						buf->lineEnd=buf->cursorPos;
						gtk_text_buffer_place_cursor(buf->textBuffer,&buf->cursorPos);
						gtk_text_iter_forward_chars(&buf->lineEnd,strlen(text));
						buf->selectRange(&buf->cursorPos,&buf->lineEnd);
						gtk_text_buffer_delete_mark(buf->textBuffer,mark);
					gtk_text_buffer_end_user_action(buf->textBuffer);
				}
			break;
//Move Selection Down
		case 12:
			if(gtk_text_buffer_get_selection_bounds(buf->textBuffer,&buf->lineStart,&buf->lineEnd))
				{
					gtk_text_buffer_begin_user_action(buf->textBuffer);
						gtk_text_iter_set_line_offset(&buf->lineStart,0);
						if(!gtk_text_iter_starts_line(&buf->lineEnd))
							gtk_text_iter_forward_visible_line(&buf->lineEnd);
						buf->selectRange(&buf->lineStart,&buf->lineEnd);
						text=gtk_text_buffer_get_text(buf->textBuffer,&buf->lineStart,&buf->lineEnd,true);
						gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
						buf->getCursorIter();
						gtk_text_iter_forward_visible_line(&buf->cursorPos);
						mark=gtk_text_buffer_create_mark(buf->textBuffer,"movedown",&buf->cursorPos,false);
						gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
						gtk_text_buffer_get_iter_at_mark(buf->textBuffer,&buf->lineEnd,mark);
						buf->cursorPos=buf->lineEnd;
						gtk_text_iter_backward_chars(&buf->cursorPos,strlen(text));
						buf->selectRange(&buf->cursorPos,&buf->lineEnd);
						gtk_text_buffer_delete_mark(buf->textBuffer,mark);
					gtk_text_buffer_end_user_action(buf->textBuffer);
				}
			break;
//completion
		case 13:
			forcePopup=true;
			doCompletionPopUp(page);
			forcePopup=false;
			break;
		}
	delete buf;
}

void loadKeybindings(void)
{
	ERRDATA
	char*	filename;

	asprintf(&filename,"%s/.KKEdit/keybindings.rc",getenv("HOME"));

	loadVarsFromFile(filename,keybindings_rc);
	for(int j=0;j<NUMSHORTCUTS;j++)
		{
			if(shortCutStrings[j]!=NULL)
				sscanf(shortCutStrings[j],"%i %i",(int*)&shortCuts[j][0],(int*)&shortCuts[j][1]);
		}
	ERRDATA debugFree(&filename);
}

VISIBLE gboolean keyShortCut(GtkWidget* window,GdkEventKey* event,gpointer data)
{
	ERRDATA
	int		loop;
	bool	gotKey=false;


	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
		{
			for(loop=0; loop<NUMSHORTCUTS; loop++)
				{
					if(event->keyval==shortCuts[loop][0])
						{
							gotKey=true;
							break;
						}
				}

			if(gotKey==true)
				{
					doKeyShortCut(loop);
					return(true);
				}

			return(gtk_window_propagate_key_event((GtkWindow*)mainWindow, event));
		}

	return(false);
}

VISIBLE void getPlugins(GtkWidget* widget,gpointer data)
{
	ERRDATA
	char*	command;

	asprintf(&command,"%s \"%s\" &",browserCommand,PLUGINSLINK);
	system(command);
	ERRDATA debugFree(&command);
}

