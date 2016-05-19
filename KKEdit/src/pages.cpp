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

GList	*pages=NULL;

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
