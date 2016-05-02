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
