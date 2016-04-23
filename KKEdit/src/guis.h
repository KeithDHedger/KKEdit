/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:50:17 GMT 2015 kdhedger68713@gmail.com

 * This file(guis.h) is part of KKEdit.

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

#ifndef _GUIS_
#define _GUIS_

enum	{AUTOINDENT=0,SHOWNUMS,WRAP,HIGHLIGHT,NOSYNTAX,USESINGLE,AUTOSAVE,AUTOBM,NODUPLICATE,NOWARN,READLINK,AUTOSHOW,UPDATECHECK,BEKIND,GLOBALPLUGMENU,AUTOSEARCH,MAXPREFSWIDGETS};
enum {MAXTABCHARS=0,MAXHISTORY,MAXFUNCDEPTH,COMPLETIONSIZE,TABWIDTH,MENUWIDTH,MAXBMWIDTH,NOTETABSSIZE,MAXJUMPHISTORY,MAXPREFSINTWIDGETS};

extern	GtkWidget	*prefsWidgets[MAXPREFSWIDGETS];
extern	GObject	*prefsIntWidgets[MAXPREFSINTWIDGETS];

enum {STOCKMENU=0,IMAGEMENU,PIXMAPMENU,NORMALMENU,CHECKMENU};

enum {ITEMNEW=0,ITEMOPEN,ITEMSAVE,ITEMCUT,ITEMCOPY,ITEMPASTE,ITEMUNDO,ITEMREDO,ITEMFIND,ITEMGOTO,ITEMLINE,ITEMAPI,ITEMDEFINE,ITEMLIVE,ITEMSEPERATOR,ITEMEXPANDER,ITEMQTAPI,ITEMBACK,ITEMFORWARD,MAXITEMS};

void buildMainGui(void);
void buildTools(void);
void buildFindReplace(void);
void buildWordCheck(int documentCheck);
int showFunctionEntry(void);
#ifdef _BUILDDOCVIEWER_
void buildGtkDocViewer(void);
#endif
void setUpToolBar(void);
char *makeToolBarList(void);
#endif