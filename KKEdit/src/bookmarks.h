/*
 *
 * ©K. D. Hedger. Mon  2 May 20:30:31 BST 2016 kdhedger68713@gmail.com

 * This file (bookmarks.h) is part of KKEdit.

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


#ifndef _BOOKMARKS_
#define _BOOKMARKS_

struct bookMarksNew
{
	pageStruct			*page;
	char				*label;
	GtkSourceMark		*mark;
	char				*markName;
	int					line;
	unsigned			pageID;
};

enum {UPDATEBMLINE=0,MOVEBM};

void moveBMsForPage(pageStruct *page,unsigned dowhat);
void destroyBMData(gpointer data);
void toggleBookmark(GtkWidget*,GtkTextIter *titer);
void removeAllBookmarks(GtkWidget *widget,GtkTextIter *titer);
void toggleBookMarkBar(GtkWidget *widget,gpointer data);
void setUserMark(void);
void removeUserMark(void);


#endif
