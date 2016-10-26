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

//MENU_NEW_LABEL gettext("_New")
//#define GTK_STOCK_NEW "document-new"
//hot key
//hot modifier
//hotkey strings
//callbacks
enum {MENUNEW=0,MENUOPEN,MENUOPENHEX,MENUNEWADMIN,MENUNEWED,MENUMANED,MENUDOXY,MENURECENT,MENUSAVE,MENUSAVEAS,MENUSAVEALL,MENUSAVESESSION,MENURESTORESESSION,MENUPRINT,MENUCLOSE,MENUCLOSEALL,MENUREVERT,MENUQUIT,MENUUNDO,MENUREDO,MENUUNDOALL,MENUREDOALL,MENUCUT,MENUCOPY,MENUPASTE,MENUFIND,MENUNEXT,MENUSORTTABS,MENUSHOWTABS,MENUSELECTTAB,MENUPREFS,MENUPLUGPREFS,MENUSHOWDOCS,MENUSHOWBMBAR,MENUHIDEBMBAR,MENUHIDETOOLBAR,MENUSHOWTOOLBAR,MENUHIDETOOLOUT,MENUSHOWTOOLOUT,MENUHIDESTATUS,MENUSHOWSTATUS,MENUSHOWVIEWER, \
MENUTOGGLENUMBERS,MENUWRAPLINES,MENUHILITELINE,MENUSYNTAXHILITE,MENUSHOWSPACE,MENUSHOWCOMPS \
,MENUGOTODEF,MENUOPENINC,MENUGOTOLINE,MENUSEARCHDEF,MENUSEARCHGTK,MENUSEARCHQT,MENUSEARCHDOXY,MENUBACK,MENUFORWARD,MENUCONTINUE,MENUTOOLS,MENUABOUT,MENULOCALHELP,MENUONLINE,MENUGETPLUGS, \
POPADDCUSTOM,MENUBLANK,MENUSPELLCHECK, \
TABFOLDERPATH,TABFILEPATH,TABFILENAME,TABSPELLCHECK,TABSRCHILITE,TABHIDE \
};

struct	menuDataStruct
{
	const char		*menuLabel;
	const char		*stockID;
	char			key;
	long			mod;
	void			*cb;
	const char		*widgetName;
	gpointer		userData;
	
};

extern menuDataStruct	menuData[];

enum	{AUTOINDENT=0,SHOWNUMS,WRAP,HIGHLIGHT,NOSYNTAX,USESINGLE,AUTOSAVE,AUTOBM,NODUPLICATE,NOWARN,READLINK,AUTOSHOW,UPDATECHECK,BEKIND,GLOBALPLUGMENU,AUTOSEARCH,SHOWWHITESPACE,DOCINEDITOR,SHOWMENUICONS,MAXPREFSWIDGETS};
enum {MAXTABCHARS=0,MAXHISTORY,MAXFUNCDEPTH,COMPLETIONSIZE,TABWIDTH,MENUWIDTH,MAXBMWIDTH,NOTETABSSIZE,MAXJUMPHISTORY,MAXPREFSINTWIDGETS};

extern	GtkWidget	*prefsWidgets[MAXPREFSWIDGETS];
extern	GObject		*prefsIntWidgets[MAXPREFSINTWIDGETS];

enum {STOCKMENU=0,IMAGEMENU,PIXMAPMENU,NORMALMENU,CHECKMENU};

enum {ITEMNEW=0,ITEMOPEN,ITEMSAVE,ITEMCUT,ITEMCOPY,ITEMPASTE,ITEMUNDO,ITEMREDO,ITEMFIND,ITEMGOTO,ITEMLINE,ITEMAPI,ITEMDEFINE,ITEMLIVE,ITEMSEPERATOR,ITEMEXPANDER,ITEMQTAPI,ITEMBACK,ITEMFORWARD,MAXITEMS};

extern GtkWidget	*recentToolBar;

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
void doPrefs(GtkWidget *widget,gpointer data);
void doMakeTool(void);
GtkWidget* newImageMenuItem(unsigned menunumber,GtkWidget *parent);
GtkWidget* newMenuItem(unsigned menunumber,GtkWidget *parent);
void buildMenus(void);

#endif