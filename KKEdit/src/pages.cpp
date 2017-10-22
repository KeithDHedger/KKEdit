/*
 *
 * ©K. D. Hedger. Tue 17 May 11:57:05 BST 2016 kdhedger68713@gmail.com

 * This file (pages.cpp) is part of KKEdit.

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

GList		*pages=NULL;
pageStruct	*searchPageStruct=NULL;
pageStruct	*currentPageStruct=NULL;

VISIBLE pageStruct *getPageStructByIDFromList(unsigned pageid)
{
	pageStruct	*page=NULL;
	GList		*list;

	list=pages;
	while(list!=NULL)
		{
			page=(pageStruct*)list->data;
			if((page!=NULL) && (page->pageID==pageid))
				return(page);
			list=list->next;
		}			
	return(NULL);
}

VISIBLE pageStruct *getPageStructByIDFromPage(int pagenum)
{
	pageStruct	*page=NULL;
	GList		*list;
	int			thispage=-1;
	GtkWidget	*pageBox=NULL;
	unsigned	pageid=0;

	if(pagenum==-1)
		thispage=gtk_notebook_get_current_page(mainNotebook);
	else
		thispage=pagenum;

	pageBox=gtk_notebook_get_nth_page(mainNotebook,thispage);
	if(pageBox==NULL)
		return(NULL);
	else
		pageid=(unsigned)(long)g_object_get_data((GObject*)pageBox,"pageid");

	if(pageid==0)
		return(NULL);
	
	list=pages;
	while(list!=NULL)
		{
			page=(pageStruct*)list->data;
			if((page!=NULL) && (page->pageID==pageid))
				return(page);
			list=list->next;
		}			
	return(NULL);
}

pageListData* getCurrentPageListData(void)
{
	pageStruct		*page=NULL;
	GList			*list;
	int				thispage;
	GtkWidget		*pageBox=NULL;
	pageListData	*data=NULL;
	unsigned		pageid=0;
	unsigned		cnt=0;

	thispage=gtk_notebook_get_current_page(mainNotebook);
	pageBox=gtk_notebook_get_nth_page(mainNotebook,thispage);
	if(pageBox==NULL)
		return(NULL);
	else
		pageid=(unsigned)(long)g_object_get_data((GObject*)pageBox,"pageid");

	list=pages;
	while(list!=NULL)
		{
			page=(pageStruct*)list->data;
			if((page!=NULL) && (page->pageID==pageid))
				{
					data=(pageListData*)calloc(1,sizeof(pageListData));
					data->page=page;
					data->list=list;
					data->listNum=cnt;
					return(data);
				}
			cnt++;
			list=list->next;
		}			
	return(NULL);
}

unsigned getIDFromPage(int pagenum)
{
	int			thispage;
	GtkWidget	*pageBox=NULL;

	if(pagenum!=-1)
		thispage=pagenum;
	else
		thispage=gtk_notebook_get_current_page(mainNotebook);

	pageBox=gtk_notebook_get_nth_page(mainNotebook,thispage);

	if(pageBox==NULL)
		return(0);
	else
		return((unsigned)(long)g_object_get_data((GObject*)pageBox,"pageid"));
}

gboolean verticalSelect(GtkWidget *widget,GdkEvent *event,gpointer ud)
{
	GdkEventMotion	*evmotion=(GdkEventMotion*)event;;
	GdkModifierType	mask;
	int				buffer_x;
	int				buffer_y;
	GtkTextIter		iter;
	GtkTextIter		startiter;
	GtkTextIter		enditer;
	int				xstartcoord;
	int				ystartcoord;
	int				xendcoord;
	int				yendcoord;

	pageStruct		*page=(pageStruct*)ud;

	if(page==NULL)
		return(false);

	if (event->type==GDK_MOTION_NOTIFY)
		{
#ifdef _USEGTK3_
#if GTK_MINOR_VERSION >=22
			GdkSeat		*seat=gdk_display_get_default_seat(gdk_display_get_default());
			GdkDevice	*device=gdk_seat_get_pointer(seat);
			GdkWindow	*window=gdk_device_get_window_at_position (device,NULL,NULL);
if(window==NULL)
	return(false);
			window=gdk_window_get_device_position(window,device,NULL,NULL,&mask);
#else
			gdk_window_get_pointer(NULL,NULL,NULL,&mask);
#endif
#else
			gdk_window_get_pointer(NULL,NULL,NULL,&mask);
#endif

			if((mask & GDK_CONTROL_MASK) && (mask & GDK_BUTTON1_MASK))
				{
					gtk_text_view_window_to_buffer_coords((GtkTextView*)page->view,GTK_TEXT_WINDOW_TEXT,evmotion->x,evmotion->y,&buffer_x,&buffer_y);
					if(page->vertTag.gotStart==false)
						{
							page->vertTag.gotStart=true;
							gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&page->vertTag.startIter,buffer_x,buffer_y);
							gtk_text_view_get_line_yrange((GtkTextView*)page->view,&page->vertTag.startIter,&page->vertTag.yStartLoc,&page->vertTag.lineHite);
							page->vertTag.xStartLoc=buffer_x;
							page->vertTag.yStartLoc=buffer_y;
						}
					else
						{
							page->vertTag.gotSelect=true;
							page->vertTag.xEndLoc=buffer_x;
							page->vertTag.yEndLoc=buffer_y;
						}

					gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
					gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
					gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"verticaltag",&startiter,&enditer);

					xstartcoord=page->vertTag.xStartLoc;
					ystartcoord=page->vertTag.yStartLoc;
					xendcoord=page->vertTag.xEndLoc;
					yendcoord=page->vertTag.yEndLoc;
					
					while(ystartcoord<yendcoord)
						{
							gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&startiter,xstartcoord,ystartcoord);
							gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&enditer,xendcoord,ystartcoord);
							gtk_text_buffer_apply_tag((GtkTextBuffer*)page->buffer,page->vertTag.verticalTag,&startiter,&enditer);
							ystartcoord+=page->vertTag.lineHite;
						}
					return(true);
	       		}
    	}
	return(false);
}

void copyVerticalSelection(GtkWidget *widget,gpointer data)
{
	GtkTextIter		startiter;
	GtkTextIter		enditer;
	gchar			*textslice=NULL;
	GString			*str=g_string_new("");
	pageStruct		*page=(pageStruct*)data;
	GtkClipboard	*clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	const char		*addnewline="";
	int				xstartcoord=page->vertTag.xStartLoc;
	int				ystartcoord=page->vertTag.yStartLoc;
	int				xendcoord=page->vertTag.xEndLoc;
	int				yendcoord=page->vertTag.yEndLoc-page->vertTag.lineHite;

	if(page==NULL)
		return;

	while(ystartcoord<yendcoord)
		{
			gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&startiter,xstartcoord,ystartcoord);
			gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&enditer,xendcoord,ystartcoord);
			ystartcoord+=page->vertTag.lineHite;
			textslice=gtk_text_iter_get_visible_text(&startiter,&enditer);
			if(textslice[strlen(textslice)-1]!='\n')
				addnewline="\n";
			else
				addnewline="";
			g_string_append_printf(str,"%s%s",textslice,addnewline);
			g_free(textslice);
		}

	gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&startiter,xstartcoord,ystartcoord);
	gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&enditer,xendcoord,ystartcoord);
	textslice=gtk_text_iter_get_visible_text(&startiter,&enditer);
	g_string_append_printf(str,"%s",textslice);
	g_free(textslice);
	gtk_clipboard_set_text(clipboard,(char*)str->str,-1);

	g_string_free(str,true);
}

void cutVerticalSelection(GtkWidget *widget,gpointer data)
{
	GtkTextIter		startiter;
	GtkTextIter		enditer;
	gchar			*textslice=NULL;
	pageStruct		*page=(pageStruct*)data;
	GtkClipboard	*clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	const char		*addnewline="";
	int				xstartcoord=page->vertTag.xStartLoc;
	int				ystartcoord=page->vertTag.yStartLoc;
	int				xendcoord=page->vertTag.xEndLoc;
	int				yendcoord=page->vertTag.yEndLoc-page->vertTag.lineHite;

	if(page==NULL)
		return;

	copyVerticalSelection(widget,data);

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);
	while(ystartcoord<yendcoord+page->vertTag.lineHite)
		{
			gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&startiter,xstartcoord,ystartcoord);
			gtk_text_view_get_iter_at_location((GtkTextView*)page->view,&enditer,xendcoord,ystartcoord);
			ystartcoord+=page->vertTag.lineHite;
			gtk_text_buffer_delete ((GtkTextBuffer*)page->buffer,&startiter,&enditer);
		}
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}

void clearVerticalSelection(GtkWidget *widget,gpointer data)
{
	GtkTextIter	startiter;
	GtkTextIter	enditer;
	pageStruct	*page=(pageStruct*)data;

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
	gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"verticaltag",&startiter,&enditer);

	page->vertTag.gotSelect=false;
	page->vertTag.gotStart=false;
}

