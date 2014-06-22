/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceiter.h>
#include <unique/unique.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#include <ctype.h>
#include <gdk/gdkkeysyms.h>
#include <libgen.h>

#include "config.h"
#ifdef BUILDDOCVIEWER
#include <webkit/webkit.h>
#endif

#include "globals.h"
#include "files.h"
#include "guis.h"
#include "navcallbacks.h"
#include "searchcallbacks.h"
#include "spellcheck.h"
#include "textbuffer.h"
#include "config.h"

GtkWidget*			tabMenu;
char				defineText[1024];
GtkPrintSettings*	settings=NULL;

void setToobarSensitive(void)
{
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
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)backButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)backButton,false);
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
	free(((bookMarksNew*)data)->markName);
}

void removeAllBookmarks(GtkWidget* widget,GtkTextIter* titer)
{
	pageStruct*	page=NULL;
	int			numpages;
	GtkTextIter	startiter;
	GtkTextIter	enditer;

	numpages=gtk_notebook_get_n_pages(notebook);
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
	gtk_widget_show_all(menuBookMark);
}

void toggleBookmark(GtkWidget* widget,GtkTextIter* titer)
{
	pageStruct*		page=getPageStructPtr(-1);
	GtkWidget*		menuitem;
	GtkTextMark*	mark;
	GtkTextIter*	iter;
	GtkTextIter		siter;
	int				line;
	GtkTextIter		startprev,endprev;
	char*			previewtext;
	char*			starttext;
	char*			endtext;
	GSList*			mark_list=NULL;
	const gchar*	mark_type;
	GList*			ptr=NULL;
	bookMarksNew*	bookmarkdata;

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
							free(((bookMarksNew*)ptr->data)->markName);
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
					gtk_menu_shell_append(GTK_MENU_SHELL(menuBookMarkSubMenu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(jumpToMark),(void*)ptr->data);
					ptr=g_list_next(ptr);
				}
			gtk_widget_show_all(menuBookMark);
			g_slist_free(mark_list);
		}
	else
		{
			/* no mark found: create one */
			bookmarkdata=(bookMarksNew*)malloc(sizeof(bookMarksNew));
			newBookMarksList=g_list_append(newBookMarksList,(gpointer)bookmarkdata);
			bookmarkdata->page=page;
			asprintf(&bookmarkdata->markName,"Bookmark-%i",bmMarkNumber++);
			bookmarkdata->mark=gtk_source_buffer_create_source_mark(page->buffer,bookmarkdata->markName,mark_type,iter);
//preview text for menu
			line=gtk_text_iter_get_line(iter);
			gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&startprev,line);
			gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&endprev,line+1);
			previewtext=gtk_text_iter_get_text(&startprev,&endprev);

			previewtext[strlen(previewtext)-1]=0;
			g_strchug(previewtext);
			g_strchomp(previewtext);

			if(strlen(previewtext)>BOOKMAXMARKMENULEN)
				{
					starttext=sliceLen(previewtext,-1,BOOKMAXMARKMENULEN/2);
					endtext=sliceLen(previewtext,strlen(previewtext)-BOOKMAXMARKMENULEN/2,-1);
					g_free(previewtext);
					asprintf(&previewtext,"%s...%s",starttext,endtext);
				}

			bookmarkdata->label=previewtext;
			menuitem=gtk_menu_item_new_with_label(bookmarkdata->label);
			gtk_menu_shell_append(GTK_MENU_SHELL(menuBookMarkSubMenu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(jumpToMark),(void*)bookmarkdata);
			gtk_widget_show_all(menuBookMark);
		}
}

void showHideWidget(GtkWidget* widget,bool show)
{
	if(show==false)
		gtk_widget_hide(widget);
	else
		gtk_widget_show(widget);
}

void refreshMainWindow(void)
{
	gtk_widget_show_all(window);

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
	GtkWidget*	dialog;
	int			result;

	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,"%s %s",question,file);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_YES,GTK_RESPONSE_YES,GTK_STOCK_NO,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"What Do You Want To Do?");

	result=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return(result);
}

void doOpenFile(GtkWidget* widget,gpointer data)
{
	GtkWidget*	dialog;
	char*		filename;
	GSList*		filenames;
	GSList*		thisnext;

	dialog=gtk_file_chooser_dialog_new("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	gtk_file_chooser_set_select_multiple((GtkFileChooser*)dialog,true);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filenames=gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
			thisnext=filenames;
			while(thisnext!=NULL)
				{
					filename=(char*)thisnext->data;
					openFile(filename,0);
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
	GtkWidget*	dialog;
	gint		result;
	char*		message;

	asprintf(&message,"Save file %s before closing?",filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_SAVE,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Warning unsaved data!");

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	g_free(message);
	return(result);
}

void updateStatuBar(GtkTextBuffer* textbuffer,GtkTextIter* location,GtkTextMark* mark,gpointer data)
{
	pageStruct* page=(pageStruct*)data;
	TextBuffer*	buf;
	char*		message=NULL;
	const char*	path;
	const char*	lang;

	pageStruct* pagecheck=getPageStructPtr(currentTabNumber);

	if((page==NULL) || (showStatus==false))
		{
			gtk_statusbar_pop((GtkStatusbar*)statusWidget,0);
			gtk_statusbar_push((GtkStatusbar*)statusWidget,0,"");
			return;
		}

	if(pagecheck!=page)
		return;

	path=page->filePath;
	lang=page->lang;

	if(path==NULL)
		path="";

	if(lang==NULL)
		lang="";

	buf=new TextBuffer(textbuffer);

	gtk_statusbar_pop((GtkStatusbar*)statusWidget,0);
	asprintf(&message,"Line %i Column %i \t\tSyntax Highlighting %s\t\tFilePath %s",buf->lineNum,buf->column,lang,path);
	gtk_statusbar_push((GtkStatusbar*)statusWidget,0,message);
	free(message);
	delete buf;
}

void setSensitive(void)
{
	pageStruct*		page=getPageStructPtr(currentTabNumber);
	const gchar*	text;
	char*			newlabel;
	int				offset=0;
	GtkTextIter	start_find,end_find;

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
			gtk_widget_set_sensitive((GtkWidget*)menuBookMark,false);
			gtk_widget_set_sensitive((GtkWidget*)menufunc,false);
			gtk_widget_set_sensitive((GtkWidget*)menunav,false);
			gtk_widget_set_sensitive((GtkWidget*)menuprint,false);
			gtk_widget_set_sensitive((GtkWidget*)menuclose,false);
			gtk_widget_set_sensitive((GtkWidget*)menucloseall,false);
			gtk_widget_set_sensitive((GtkWidget*)menusaveall,false);
			gtk_widget_set_sensitive((GtkWidget*)menurevert,false);
			gtk_statusbar_remove_all((GtkStatusbar*)statusWidget,0);
		}
	else
		{
			text=gtk_label_get_text((GtkLabel*)page->tabName);
//menu
			gtk_widget_set_sensitive((GtkWidget*)undoMenu,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoMenu,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)undoAllMenu,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoAllMenu,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)saveMenu,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
//tab
			if(text[0]=='*')
				offset=1;

			if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer))==true)
				asprintf(&newlabel,"*%s",&text[offset]);
			else
				newlabel=strdup(&text[offset]);

			gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)newlabel);
			g_free(newlabel);
			gtk_widget_set_sensitive((GtkWidget*)cutMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)copyMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)pasteMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)menuBookMark,true);
			gtk_widget_set_sensitive((GtkWidget*)menunav,true);
			gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)menuprint,true);
			gtk_widget_set_sensitive((GtkWidget*)menuclose,true);
			gtk_widget_set_sensitive((GtkWidget*)menucloseall,true);
			gtk_widget_set_sensitive((GtkWidget*)menusaveall,true);
			gtk_widget_set_sensitive((GtkWidget*)menurevert,true);
			gtk_widget_show_all(page->tabName);
			updateStatuBar((GtkTextBuffer*)page->buffer,NULL,NULL,page);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start_find);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end_find);
			gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&start_find,&end_find);  
		}
}

bool closingAll=false;

void closeTab(GtkWidget* widget,gpointer data)
{
	long		thispage;
	int			result;
	pageStruct*	page;

	if(closingAll==true)
		thispage=0;
	else
		{
			if(data==NULL)
				thispage=gtk_notebook_get_current_page(notebook);
			else
				thispage=gtk_notebook_page_num(notebook,(GtkWidget *)data);
		}

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

	if(page->filePath!=NULL)
		g_free(page->filePath);
	if(page->fileName!=NULL)
		g_free(page->fileName);
	if(page->gFile!=NULL)
		g_object_unref(page->gFile);
	if(page->monitor!=NULL)
		g_object_unref(page->monitor);
	if(page!=NULL)
		g_free(page);
	currentPage--;
	gtk_notebook_remove_page(notebook,thispage);
	setSensitive();
}

void closeAllTabs(GtkWidget* widget,gpointer data)
{
	int	numtabs=gtk_notebook_get_n_pages(notebook);

	for(int loop=0; loop<numtabs; loop++)
		{
			closingAll=true;
			closeTab(NULL,0);
		}

//rebuild bookmark menu
	rebuildBookMarkMenu();
	gtk_widget_show_all(menuBookMark);
}

void switchPage(GtkNotebook *notebook,gpointer arg1,guint thispage,gpointer user_data)
{
	pageStruct*	page;
	char*		functions=NULL;
	GtkWidget*	menuitem;
	int			linenum;
	char		tmpstr[1024];
	char*		lineptr;
	bool		onefunc=false;
	int			numtypes=0;
	char*		typenames[50]= {NULL,};
	bool		flag;
	char*		newstr=NULL;
	GtkWidget*	whattypemenu;
	GtkWidget*	typesubmenus[50]= {NULL,};
	GtkWidget*	submenu;

	if(arg1==NULL)
		return;

	page=(pageStruct*)g_object_get_data((GObject*)arg1,"pagedata");
	if(page==NULL)
		return;

	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)menufunc);
	if (submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)menufunc,NULL);

	currentTabNumber=thispage;

	page->rebuildMenu=false;

	getRecursiveTagList(page->filePath,&functions);
	lineptr=functions;

	page->isFirst=true;
	page->navSubMenu=(GtkMenuItem*)gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufunc),(GtkWidget*)page->navSubMenu);

	while (lineptr!=NULL)
		{
			tmpstr[0]=0;
			sscanf (lineptr,"%*s %*s %i %"VALIDCHARS"s",&linenum,tmpstr);

			if(strlen(tmpstr)>0)
				{
					if(listFunction==4)
						{
							newstr=NULL;
							newstr=sliceBetween(lineptr,(char*)" ",(char*)" ");
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
											free(newstr);
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

									free(newstr);

									onefunc=true;
									menuitem=gtk_image_menu_item_new_with_label(tmpstr);
									gtk_menu_shell_append(GTK_MENU_SHELL(whattypemenu),menuitem);
									gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(gotoLine),(void*)(long)linenum);
								}
						}
					else
						{
							onefunc=true;
							menuitem=gtk_image_menu_item_new_with_label(tmpstr);
							gtk_menu_shell_append(GTK_MENU_SHELL(page->navSubMenu),menuitem);
							gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(gotoLine),(void*)(long)linenum);
						}
				}
			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;
		}

	gtk_window_set_title((GtkWindow*)window,page->fileName);
	refreshMainWindow();
	if(functions!=NULL)
		g_free(functions);

	gtk_widget_set_sensitive((GtkWidget*)menufunc,onefunc);
	setSensitive();
}

void copyToClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_copy_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
}

void cutToClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_cut_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),true);
	setSensitive();
}

void pasteFromClip(GtkWidget* widget,gpointer data)
{
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
	gtk_text_buffer_end_user_action   ((GtkTextBuffer*)page->buffer);
	setSensitive();
}

void undo(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);

	if(gtk_source_buffer_can_undo(page->buffer));
	{
		gtk_source_buffer_undo(page->buffer);
		page->isFirst=true;
		setSensitive();
	}
}

void unRedoAll(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);

	if(page!=NULL)
		{
			if((long)data==0)
				{
					while(gtk_source_buffer_can_undo(page->buffer))
						gtk_source_buffer_undo(page->buffer);
				}
			else
				{
					while(gtk_source_buffer_can_redo(page->buffer))
						gtk_source_buffer_redo(page->buffer);
				}
			page->isFirst=true;
			setSensitive();
		}
}


void redo(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);

	if(gtk_source_buffer_can_redo(page->buffer));
	{
		gtk_source_buffer_redo(page->buffer);
		page->isFirst=true;
		setSensitive();
	}
}

void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	gchar**	array=gtk_selection_data_get_uris(selection_data);
	int		cnt=g_strv_length(array);
	char*	filename;

	for(int j=0; j<cnt; j++)
		{
			filename=g_filename_from_uri(array[j],NULL,NULL);
			openFile(filename,0);
		}

	g_strfreev(array);
}

void externalTool(GtkWidget* widget,gpointer data)
{
	toolStruct*	tool=(toolStruct*)data;
	pageStruct*	page=getPageStructPtr(-1);
	char*		docdirname=NULL;
	char*		tooldirname=NULL;
	char*		text=NULL;
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	const char*	vars[]= {"%t","%f","%d","%i","%h","%l",NULL};
	char*		ptr;
	long		pos;
	int			loop=0;
	GString*	tempCommand;
	bool		continueflag;

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

	setenv("KKEDIT_CURRENTFILE",page->filePath,1);
	setenv("KKEDIT_HTMLFILE",htmlFile,1);
	setenv("KKEDIT_CURRENTDIR",docdirname,1);
	setenv("KKEDIT_DATADIR",DATADIR,1);
	setenv("KKEDIT_SOURCE_LANG",page->lang,1);

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

#ifdef BUILDDOCVIEWER
	gtk_window_set_title((GtkWindow*)docView,tool->menuName);
#endif

	if(tool->clearView==true)
		{
			gtk_text_buffer_set_text(toolOutputBuffer,"",0);
		}

	runCommand(tempCommand->str,&text,tool->inTerminal,tool->flags,tool->runAsRoot);
	g_free(selection);

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
	g_free(text);
	g_free(docdirname);
	g_free(tooldirname);
}

void openHelp(GtkWidget* widget,gpointer data)
{
	asprintf(&thePage,"file://%s/help/help.html",DATADIR);
#ifdef BUILDDOCVIEWER
	gtk_window_set_title((GtkWindow*)docView,"KKEdit Help");
	showDocView(USEURI,(char*)"KKEdit");

#else
	asprintf(&thePage,"xdg-open %s/help/help.html",DATADIR);
	runCommand(thePage,NULL,false,8);
	g_free(thePage);
	thePage=NULL;
#endif
}

void copyToClipboard(GtkWidget* widget,gpointer data)
{
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(clipboard,(char*)data,-1);
}

void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data)
{
	pageStruct*		page=getPageStructPtr(-1);
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	GtkWidget*		menuitem;
	GtkWidget*		image;
	GList*			ptr;

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
					functionData* fdata=getFunctionByName(selection,false);
					if(fdata!=NULL)
						{
							sprintf((char*)&defineText,"%s",fdata->define);
							menuitem=gtk_menu_item_new_with_label(defineText);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(copyToClipboard),(void*)defineText);
							destroyData(fdata);

							image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_MENU);
							menuitem=gtk_image_menu_item_new_with_label("Go To Definition");
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(goToDefinition),NULL);
						}

					menuitem=gtk_image_menu_item_new_with_label("Search In Qt5 Docs");
					image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(searchQT5Docs),NULL);

					menuitem=gtk_image_menu_item_new_with_label("Search In Gtk Docs");
					image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(seachGtkDocs),NULL);
#ifdef _ASPELL_
//spell check
					if((spellChecker!=NULL) && (aspellConfig!=NULL))
						{
							menuitem=gtk_image_menu_item_new_with_label("Check Spellling");
							image=gtk_image_new_from_stock(GTK_STOCK_SPELL_CHECK,GTK_ICON_SIZE_MENU);
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(checkWord),NULL);
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
									gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
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
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
				}
			ptr=g_list_next(ptr);
		}

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_menu_item_new_with_label("Toggle Bookmark");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(toggleBookmark),NULL);

	gtk_widget_show_all((GtkWidget*)menu);
}

void doTabMenu(GtkWidget *widget,gpointer user_data)
{
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	gtk_clipboard_set_text(clipboard,(char*)user_data,-1);
	gtk_widget_destroy(tabMenu);
}

gboolean whatPane(GtkWidget *widget,GdkEvent *event,gpointer data)
{
	pageStruct* page=(pageStruct*)getPageStructPtr(-1);

	if((long)data==1)
		page->inTop=true;
	else
		page->inTop=false;

	return(false);
}

void doSplitView(GtkWidget *widget,gpointer user_data)
{
	pageStruct* page=(pageStruct*)user_data;

	if(gtk_paned_get_child2((GtkPaned*)page->pane)==NULL)
		{
			page->pageWindow2=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

			page->view2 = (GtkSourceView*)gtk_source_view_new_with_buffer (page->buffer);
			g_signal_connect(G_OBJECT(page->view2),"populate-popup",G_CALLBACK(populatePopupMenu),NULL);
			setFilePrefs(page);
//	g_signal_connect(G_OBJECT(page->view2),"paste-clipboard",G_CALLBACK(testcallback),NULL);


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
	char*		filepath=NULL;

	asprintf(&filepath,"%s/%s",page->dirName,gtk_menu_item_get_label(widget));
	openFile(filepath,0);
	free(filepath);
}

bool tabPopUp(GtkWidget *widget, GdkEventButton *event,gpointer user_data)
{
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

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			tabMenu=gtk_menu_new();
			page=(pageStruct*)user_data;

//copy dirname
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy Folder Path");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->dirName);
//copy filepath
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy Filepath");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->filePath);
//copy filename
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy FileName");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->fileName);

#ifdef _ASPELL_
//check document
			if((spellChecker!=NULL) && (aspellConfig!=NULL))
				{
					image=gtk_image_new_from_stock(GTK_STOCK_SPELL_CHECK,GTK_ICON_SIZE_MENU);
					menuitem=gtk_image_menu_item_new_with_label("Spell Check Document");
					gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
					gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doSpellCheckDoc),(void*)page->filePath);
				}
#endif

//paned view
			image=gtk_image_new_from_stock(GTK_STOCK_NEW,GTK_ICON_SIZE_MENU);
			if(page->isSplit==true)
				menuitem=gtk_image_menu_item_new_with_label("Un-Split View");
			else
				menuitem=gtk_image_menu_item_new_with_label("Split View");

			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doSplitView),(void*)page);

//source highlighting
			lm=gtk_source_language_manager_get_default();
			ids=gtk_source_language_manager_get_language_ids(lm);

			image=gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Source Highlight");
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

							gtk_signal_connect(GTK_OBJECT(menuids),"activate",G_CALLBACK(changeSourceStyle),(void*)(long)idnum[j]);
							gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuids);
						}
				}
//add files to tab
			//image=gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);

			submenu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);

			asprintf(&command,"find \"%s\" -maxdepth 1 -xtype f -iname \"[^.]*[^$.o]\"|sort",page->dirName);
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					while(fgets(line,2048,fp))
						{
							line[strlen(line)-1]=0;
							name=basename(line);
							menuids=gtk_image_menu_item_new_with_label(strdup(name));
							gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuids);
							gtk_signal_connect(GTK_OBJECT(menuids),"activate",G_CALLBACK(openFromTab),(void*)page);
						}
					fclose(fp);
				}
			g_free(command);

			gtk_widget_show_all(menuitem);

			gtk_menu_attach_to_widget(GTK_MENU(tabMenu),widget,NULL);
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);

			return(true);
		}
	else
		return(false);
}

void messageOpen(UniqueMessageData *message)
{
	int argc;

	gchar** uris=unique_message_data_get_uris(message);
	argc=g_strv_length(uris);

	for (int loop=1; loop<argc; loop++)
		openFile(uris[loop],1);
}

UniqueResponse messageReceived(UniqueApp *app,UniqueCommand command,UniqueMessageData *message,guint time,gpointer user_data)
{
	UniqueResponse	res;

	switch(command)
		{
		case UNIQUE_ACTIVATE:
			gtk_window_set_screen(GTK_WINDOW(window),unique_message_data_get_screen(message));
			gtk_window_present_with_time(GTK_WINDOW(window),time);
			res=UNIQUE_RESPONSE_OK;
			break;

		case UNIQUE_OPEN:
			messageOpen(message);
			gtk_window_set_screen(GTK_WINDOW(window),unique_message_data_get_screen(message));
			gtk_window_present_with_time(GTK_WINDOW(window),time);
			res=UNIQUE_RESPONSE_OK;
			break;

		default:
			res=UNIQUE_RESPONSE_OK;
			break;
		}
	return(res);
}

void writeExitData(void)
{
	GtkAllocation	alloc;
	FILE*			fd=NULL;
	char*			filename;
	int				winx;
	int				winy;

	gtk_widget_get_allocation(window,&alloc);
	gtk_window_get_position((GtkWindow*)window,&winx,&winy);

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);
	asprintf(&filename,"%s/.KKEdit/kkedit.window.rc",getenv("HOME"));
	fd=fopen(filename,"w");
	if(fd!=NULL)
		{
			fprintf(fd,"windowsize	%i %i %i %i\n",alloc.width,alloc.height,winx,winy);
			fprintf(fd,"insenssearch	%i\n",(int)insensitiveSearch);
			fprintf(fd,"useregex	%i\n",(int)useRegex);
			fprintf(fd,"wrapsearch	%i\n",(int)wrapSearch);
			fprintf(fd,"replaceall	%i\n",(int)replaceAll);
			fprintf(fd,"allfiles	%i\n",(int)findInAllFiles);
			fprintf(fd,"showbmbar	%i\n",(int)showBMBar);
			fprintf(fd,"showtoolbar	%i\n",(int)showToolBar);
			fprintf(fd,"showstatusbar	%i\n",(int)showStatus);
			fprintf(fd,"highlightall	%i\n",(int)hightlightAll);
			fclose(fd);
		}
	g_free(filename);
}

void writeConfig(void)
{
	GtkAllocation	alloc;
	FILE*			fd=NULL;
	char*			filename;
	int				winx;
	int				winy;

	gtk_widget_get_allocation(window,&alloc);
	gtk_window_get_position((GtkWindow*)window,&winx,&winy);

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);
	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	fd=fopen(filename,"w");
	if(fd!=NULL)
		{
			fprintf(fd,"indentcode	%i\n",(int)indent);
			fprintf(fd,"showlinenumbers	%i\n",(int)lineNumbers);
			fprintf(fd,"wrapline	%i\n",(int)lineWrap);
			fprintf(fd,"highlightcurrentline	%i\n",(int)highLight);
			fprintf(fd,"stylename	%s\n",styleName);
			fprintf(fd,"singleuse	%i\n",(int)singleUse);
			fprintf(fd,"insenssearch	%i\n",(int)insensitiveSearch);
			fprintf(fd,"wrapsearch	%i\n",(int)wrapSearch);
			fprintf(fd,"savesessiononexit	%i\n",(int)onExitSaveSession);
			fprintf(fd,"restorebookmarks	%i\n",(int)restoreBookmarks);
			fprintf(fd,"nagscreen	%i\n",nagScreen);
			fprintf(fd,"noduplicates	%i\n",noDuplicates);
			fprintf(fd,"warning	%i\n",noWarnings);
			fprintf(fd,"readlink	%i\n",readLinkFirst);

			fprintf(fd,"showbmbar	%i\n",(int)showBMBar);
			fprintf(fd,"higlightcolour	%s\n",highlightColour);

			fprintf(fd,"tabwidth	%i\n",tabWidth);
			fprintf(fd,"depth	%i\n",depth);
			fprintf(fd,"font	%s\n",fontAndSize);
			fprintf(fd,"terminalcommand	%s\n",terminalCommand);
			fprintf(fd,"rootcommand	%s\n",rootCommand);

			fprintf(fd,"toolbarlayout	%s\n",toolBarLayout);
			fprintf(fd,"funcsort	%i\n",listFunction);

			fclose(fd);
		}
	g_free(filename);
}

bool doSaveAll(GtkWidget* widget,gpointer data)
{
	int			numpages=gtk_notebook_get_n_pages(notebook);
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
									gtk_notebook_set_current_page(notebook,loop);
									saveFile(NULL,NULL);

									break;
								case GTK_RESPONSE_NO:
									break;
								default:
									return(false);;
									break;
								}
						}
					else
						{
							gtk_notebook_set_current_page(notebook,loop);
							saveFile(NULL,NULL);
						}
				}
		}
	return(true);
}

void doShutdown(GtkWidget* widget,gpointer data)
{
	char*	command;

	asprintf(&command,"rm %s &>/dev/null",htmlFile);
	if(doSaveAll(widget,(void*)true)==true)
		{
			if(onExitSaveSession)
				saveSession(NULL,NULL);
			writeExitData();
			gtk_main_quit();
		}
	system(command);
	g_free(command);

#ifdef _ASPELL_
	delete_aspell_config(aspellConfig);
	delete_aspell_speller(spellChecker);
#endif
}

void setPrefs(GtkWidget* widget,gpointer data)
{
	pageStruct*			tpage=getPageStructPtr(-1);

	if(strcmp(gtk_widget_get_name(widget),"indent")==0)
		tmpIndent=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"show")==0)
		tmpLineNumbers=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"wrap")==0)
		tmpLineWrap=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"high")==0)
		tmpHighLight=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"single")==0)
		tmpSingleUse=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"save")==0)
		tmpSaveSessionOnExit=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"marks")==0)
		tmpRestoreBookmarks=gtk_toggle_button_get_active((GtkToggleButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"duplicates")==0)
		tmpNoDuplicates=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"warning")==0)
		tmpNoWarnings=gtk_toggle_button_get_active((GtkToggleButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"readlink")==0)
		tmpReadLinkFirst=gtk_toggle_button_get_active((GtkToggleButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"ihavedonated")==0)
		tmpNagScreen=gtk_toggle_button_get_active((GtkToggleButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"style")==0)
		{
			if(tmpStyleName!=NULL)
				g_free(tmpStyleName);
			tmpStyleName=gtk_combo_box_text_get_active_text((GtkComboBoxText*)data);
			if(tpage!=NULL)
				gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)tpage->buffer,gtk_source_style_scheme_manager_get_scheme(schemeManager,tmpStyleName));
		}

	gtk_widget_set_sensitive(restoreBMs,tmpSaveSessionOnExit);

	if(strcmp(gtk_widget_get_name(widget),"tabs")==0)
		tmpTabWidth=(int)gtk_spin_button_get_value((GtkSpinButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"depth")==0)
		tmpDepth=(int)gtk_spin_button_get_value((GtkSpinButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		{
			if(tpage!=NULL)
				gtk_source_buffer_set_style_scheme((GtkSourceBuffer*)tpage->buffer,styleScheme);
			gtk_widget_destroy(prefswin);
		}

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			indent=tmpIndent;
			lineNumbers=tmpLineNumbers;
			lineWrap=tmpLineWrap;
			highLight=tmpHighLight;
			noDuplicates=tmpNoDuplicates;
			noWarnings=tmpNoWarnings;
			readLinkFirst=tmpReadLinkFirst;

			if(styleName!=NULL)
				{
					g_free(styleName);
					styleName=strdup(tmpStyleName);
				}

			singleUse=tmpSingleUse;
			onExitSaveSession=tmpSaveSessionOnExit;
			restoreBookmarks=tmpRestoreBookmarks;

			nagScreen=tmpNagScreen;

			if(highlightColour!=NULL)
				{
					g_free(highlightColour);
					highlightColour=strdup(gtk_entry_get_text((GtkEntry*)bmHighlightBox));
				}

			if(terminalCommand!=NULL)
				{
					g_free(terminalCommand);
					terminalCommand=strdup(gtk_entry_get_text((GtkEntry*)terminalBox));
				}

			if(rootCommand!=NULL)
				{
					g_free(rootCommand);
					rootCommand=strdup(gtk_entry_get_text((GtkEntry*)rootCommandBox));
				}

			if(fontAndSize!=NULL)
				{
					g_free(fontAndSize);
					fontAndSize=strdup(gtk_font_button_get_font_name((GtkFontButton*)fontButton));
				}

			if(toolBarLayout!=NULL)
				{
					g_free(toolBarLayout);
					toolBarLayout=makeToolBarList();
					gtk_widget_destroy((GtkWidget*)toolBar);
					toolBar=(GtkToolbar*)gtk_toolbar_new();
					gtk_box_pack_start(GTK_BOX(toolBarBox),(GtkWidget*)toolBar,true,true,0);
					setUpToolBar();
				}

			tabWidth=tmpTabWidth;
			depth=tmpDepth;
			listFunction=gtk_combo_box_get_active((GtkComboBox*)funcListDrop);

			if(tpage!=NULL)
				switchPage(notebook,tpage->tabVbox,currentTabNumber,NULL);

			gtk_widget_destroy(prefswin);
			resetAllFilePrefs();
			writeConfig();
			setSensitive();
			refreshMainWindow();
		}
}

void setToolOptions(GtkWidget* widget,gpointer data)
{
	char*	dirname;
	char*	text;
	char*	toolpath;
	FILE*	fd=NULL;
	int		flags;

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
			fd=fopen(toolpath,"w");
			if(fd!=NULL)
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

					fprintf(fd,"name\t%s\n",gtk_entry_get_text((GtkEntry*)toolNameWidget));
					fprintf(fd,"command\t%s\n",gtk_entry_get_text((GtkEntry*)commandLineWidget));
					fprintf(fd,"comment\t%s\n",gtk_entry_get_text((GtkEntry*)commentWidget));
					fprintf(fd,"flags\t%i\n",flags);
					fprintf(fd,"interm\t%i\n",(int)gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget));
					fprintf(fd,"inpopup\t%i\n",(int)gtk_toggle_button_get_active((GtkToggleButton*)inPopupWidget));
					fprintf(fd,"alwayspopup\t%i\n",(int)gtk_toggle_button_get_active((GtkToggleButton*)alwaysPopupWidget));
					fprintf(fd,"clearview\t%i\n",(int)gtk_toggle_button_get_active((GtkToggleButton*)clearViewWidget));
					fprintf(fd,"runasroot\t%i\n",(int)gtk_toggle_button_get_active((GtkToggleButton*)runAsRootWidget));
					fprintf(fd,"shortcutkey\t%i\n",(int)gdk_keyval_from_name(gtk_entry_get_text((GtkEntry*)keyWidget)));
					fclose(fd);
				}

			gtk_widget_hide((GtkWidget*)data);
			gtk_widget_destroy((GtkWidget*)data);
			buildTools();
			gtk_widget_show_all(menutools);
		}

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		{
			gtk_widget_hide((GtkWidget*)data);
			gtk_widget_destroy((GtkWidget*)data);
		}

	if(strcmp(gtk_widget_get_name(widget),"delete")==0)
		{
			if((gtk_entry_get_text((GtkEntry*)toolNameWidget)!=NULL) && (yesNo((char*)"Are you sure you want to delete",(char*)gtk_entry_get_text((GtkEntry*)toolNameWidget))==GTK_RESPONSE_YES))
				{
					asprintf(&dirname,"rm \"%s\"",toolpath);
					system(dirname);
					gtk_widget_hide((GtkWidget*)data);
					gtk_widget_destroy((GtkWidget*)data);
					buildTools();
					gtk_widget_show_all(menutools);
				}
		}

	free(toolpath);
	free(text);
	free(dirname);
}

void doAbout(GtkWidget* widget,gpointer data)
{
	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};
	const char	copyright[] ="Copyright \xc2\xa9 2013 K.D.Hedger";
	const char*	aboutboxstring="KKEdit Code Text Editor";
	char*		licence;

	g_file_get_contents(DATADIR"/docs/gpl-3.0.txt",&licence,NULL,NULL);

	gtk_show_about_dialog(NULL,"authors",authors,"comments",aboutboxstring,"copyright",copyright,"version",VERSION,"website",MYWEBSITE,"program-name","KKEdit","logo-icon-name","KKEdit","license",licence,NULL);

	g_free(licence);
}

GtkSourceBuffer*	printBuffer;
GtkSourceView*		printView;

void doCombineBuffers(void)
{
	pageStruct*	page;
	GtkTextIter	iter;
	GtkTextIter	fromstart;
	GtkTextIter	fromend;

	printBuffer=gtk_source_buffer_new(NULL);
	printView=(GtkSourceView*)gtk_source_view_new_with_buffer(printBuffer);

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)printBuffer,&iter);

	for(int j=0; j<gtk_notebook_get_n_pages(notebook); j++)
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
	GtkSourcePrintCompositor *compositor;

	compositor=GTK_SOURCE_PRINT_COMPOSITOR(user_data);
	gtk_source_print_compositor_draw_page(compositor,context,page_nr);
}

void beginPrint(GtkPrintOperation *operation,GtkPrintContext *context,gpointer user_data)
{
	GtkSourcePrintCompositor*	compositor;
	gint						n_pages;

	compositor=GTK_SOURCE_PRINT_COMPOSITOR (user_data);

	while(!gtk_source_print_compositor_paginate(compositor,context));

	n_pages=gtk_source_print_compositor_get_n_pages(compositor);
	gtk_print_operation_set_n_pages(operation,n_pages);
}

void printFile(GtkWidget* widget,gpointer data)
{
	doCombineBuffers();
	GtkSourcePrintCompositor*	printview=gtk_source_print_compositor_new_from_view(printView);
	GtkPrintOperation*			print;
	GtkPrintOperationResult		result;

	print=gtk_print_operation_new ();
	if (settings != NULL)
		gtk_print_operation_set_print_settings(print,settings);
	g_signal_connect(print,"begin-print",G_CALLBACK(beginPrint),(void*)printview);
	g_signal_connect(print,"draw-page",G_CALLBACK (drawPage),(void*)printview);
	result=gtk_print_operation_run(print,GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,GTK_WINDOW(window),NULL);
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
	gchar*	uri=NULL;
	char*	filename;

	if(data!=NULL)
		{
			openFile((char*)data,0);
			return;
		}

	uri=gtk_recent_chooser_get_current_uri(chooser);
	if (uri!=NULL)
		{
			filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
			openFile(filename,0);
			g_free (uri);
			g_free(filename);
		}
}

void newEditor(GtkWidget* widget,gpointer data)
{
	char*	command=NULL;

	switch((long)data)
		{
		case 1:
			if(strcmp(rootCommand,"")!=0)
				asprintf(&command,"%s kkedit -m &>/dev/null &",rootCommand);
			else
				asprintf(&command,"%s sudo kkedit -m &>/dev/null &",terminalCommand);
			system(command);
			free(command);
			break;
		case 2:
			system("kkedit -m &>/dev/null &");
			break;
		case 3:
			if(gotManEditor==0)
				system("manpageeditor &>/dev/null &");
			break;
		}
}

void line_mark_activated(GtkSourceGutter* gutter,GtkTextIter* iter,GdkEventButton* ev,pageStruct* page)
{

	if(ev->button!=1)
		return;

	toggleBookmark(NULL,iter);
}

void toggleToolOutput(GtkWidget* widget,gpointer data)
{
	showToolOutWin=!showToolOutWin;
	if(showToolOutWin)
		{
			gtk_widget_show(toolOutVBox);
			gtk_menu_item_set_label((GtkMenuItem*)widget,"Hide Tool Output");
		}
	else
		{
			gtk_widget_hide(toolOutVBox);
			gtk_menu_item_set_label((GtkMenuItem*)widget,"Show Tool Output");
		}
}

void toggleBookMarkBar(GtkWidget* widget,gpointer data)
{
	showBMBar=!showBMBar;
	if(showBMBar)
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Hide Bookmarks Bar");
	else
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Show Bookmarks Bar");
	resetAllFilePrefs();
}

void toggleToolBar(GtkWidget* widget,gpointer data)
{
	showToolBar=!showToolBar;
	if(showToolBar)
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Hide Tool Bar");
	else
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Show Tool Bar");
	refreshMainWindow();
}

//toggleStatusBar
void toggleStatusBar(GtkWidget* widget,gpointer data)
{
	showStatus=!showStatus;
	if(showStatus)
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Hide Status Bar");
	else
		gtk_menu_item_set_label((GtkMenuItem*)widget,"Show Status Bar");
	refreshMainWindow();
}

#ifdef BUILDDOCVIEWER
void toggleDocviewer(GtkWidget* widget,gpointer data)
{
	showDocviewer=!showDocviewer;
	if(showDocviewer)
		{
			gtk_menu_item_set_label((GtkMenuItem*)widget,"Hide Docviewer");
			gtk_widget_show_all(docView);
			gtk_window_present((GtkWindow*)docView);
		}
	else
		{
			gtk_menu_item_set_label((GtkMenuItem*)widget,"Show Docviewer");
			gtk_widget_hide(docView);
		}
}
#endif

void doKeyShortCut(int what)
{
	TextBuffer*		buf;
	char*			text;
	pageStruct*		page=getPageStructPtr(-1);
	GtkTextMark*	mark;

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
			free(text);
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
		}
	delete buf;
}

void loadKeybindings(void)
{
	FILE*	fd=NULL;
	char*	filename;
	char	buffer[1024];
	char	key[256];
	char	func[256];
	int		keycnt=0;

	asprintf(&filename,"%s/.KKEdit/keybindings",getenv("HOME"));
	fd=fopen(filename,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					key[0]=0;
					func[0]=0;
					fgets(buffer,1024,fd);
					sscanf(buffer,"%s %s",(char*)&key,(char*)&func);
					if(strlen(buffer)>3)
						{
							shortCuts[keycnt][0]=(int)atoi(key);
							shortCuts[keycnt][1]=(int)atoi(func);
							keycnt++;
						}
				}
			fclose(fd);
		}
}

gboolean keyShortCut(GtkWidget* window,GdkEventKey* event,gpointer data)
{
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
				doKeyShortCut(loop);
		}

	return FALSE;
}







