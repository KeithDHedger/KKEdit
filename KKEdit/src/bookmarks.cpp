/*
 *
 * ©K. D. Hedger. Mon  2 May 20:30:22 BST 2016 kdhedger68713@gmail.com

 * This file (bookmarks.cpp) is part of KKEdit.

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

void moveBMsForPage(pageStruct *page,unsigned dowhat)
{
	unsigned		thisid;
	unsigned		len;
	bookMarksNew	*bookmarkdata;
	GtkTextIter		newiter;

	if(page!=NULL)
		{
			thisid=page->pageID;
			if(newBookMarksList!=NULL)
				{
					len=g_list_length(newBookMarksList);
					for(unsigned j=0;j<len;j++)
						{
							bookmarkdata=(bookMarksNew*)g_list_nth_data (newBookMarksList,j);
							if(bookmarkdata->pageID==thisid)
								{
									switch(dowhat)
										{
											case 0:
												gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&newiter,bookmarkdata->line,0);
												gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)page->buffer,bookmarkdata->markName,&newiter);
												break;
											case 1:
												gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&newiter,(GtkTextMark*)bookmarkdata->mark);
												bookmarkdata->line=gtk_text_iter_get_line(&newiter);
												break;
										}
								}
						}
				}
		}
}

void destroyBMData(gpointer data)
{
	ERRDATA debugFree(&((bookMarksNew*)data)->markName);
}

VISIBLE void removeAllBookmarks(GtkWidget *widget,GtkTextIter *titer)
{
	ERRDATA
	pageStruct	*page=NULL;
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

VISIBLE void toggleBookmark(GtkWidget *widget,GtkTextIter *titer)
{
	ERRDATA
	pageStruct		*page=getPageStructPtr(-1);
	GtkWidget		*menuitem;
	GtkTextMark		*mark;
	GtkTextIter		*iter;
	GtkTextIter		siter;
	int				line;
	GtkTextIter		startprev,endprev;
	char			*previewtext;
	GSList			*mark_list=NULL;
	const gchar		*mark_type;
	GList			*ptr=NULL;
	bookMarksNew	*bookmarkdata;
	char			*correctedpreview;

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
	if(mark_list!=NULL)
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
					menuitem=createNewImageMenuItem(NULL,((bookMarksNew*)ptr->data)->label,false);
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
			sinkReturn=asprintf(&bookmarkdata->markName,"%s-%i",BOOKMARK_LABEL,bmMarkNumber++);
			bookmarkdata->mark=gtk_source_buffer_create_source_mark(page->buffer,bookmarkdata->markName,mark_type,iter);
			bookmarkdata->pageID=page->pageID;
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

VISIBLE void toggleBookMarkBar(GtkWidget *widget,gpointer data)
{
	ERRDATA
	showBMBar=!showBMBar;
	if(showBMBar)
		gtk_menu_item_set_label((GtkMenuItem*)widget,MENU_HIDE_BM_BAR_LABEL);
	else
		gtk_menu_item_set_label((GtkMenuItem*)widget,MENU_SHOW_BM_BAR_LABEL);
	resetAllFilePrefs();
}

