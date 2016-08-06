/*
 *
 * ©K. D. Hedger. Fri  6 May 10:49:55 BST 2016 kdhedger68713@gmail.com

 * This file (kkedit-plugins.h) is part of KKEdit.

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

#ifndef _PLUGINS_
#define _PLUGINS_

#include <gtksourceview/gtksourceview.h>

#define GLOBALPLUGS			1
#define LOCALPLUGS			0

#define TOOL_ASYNC			8
#define TOOL_IGNORE_OP		0
#define TOOL_PASTE_OP		1
#define TOOL_REPLACE_OP		2
#define TOOL_SHOW_DOC		4
#define	TOOL_INSERT_MASK	0b10111
#define	TOOL_VIEW_OP 		16
#define USEFILE				-2
#define USEURI				-1

#define NEWMENUNAME "newmenu"
#define OPENMENUNAME "openmenu"
#define HEXDUMPMENUNAME "hexdumpmenu"
#define NEWADMINMENUNAME "newadminmenu"
#define NEWEDITORMENUNAME "neweditormenu"
#define MANEDITORMENUNAME "maneditormenu"
#define DOXYBUILDMENUNAME "doxybuildmenu"
#define RECENTMENUNAME "recentmenu"
#define SAVEMENUNAME "savemenu"
#define SAVEASMENUNAME "saveasmenu"
#define SAVEALLMENUNAME "saveallmenu"
#define SAVESESSIONMENUNAME "savesessionmenu"
#define RESTORESESSIONMENUNAME "restoresessionmenu"
#define RESTORESESSIONBMMENUNAME "restoresessionbmmenu"
#define PRINTMENUNAME "printmenu"
#define CLOSEMENUNAME "closemenu"
#define CLOSEALLMENUNAME "closeallmenu"
#define SORTTABSMENUNAME "sorttabsmenu"
#define SHOWTABSMENUNAME "showalltabsmenu"
#define REVERTMENUNAME "revertmenu"
#define QUITMENUNAME "quitmenu"
#define UNDOMENUNAME "undomenu"
#define REDOMENUNAME "redomenu"
#define UNDOALLMENUNAME "undoallmenu"
#define REDOALLMENUNAME "redoallmenu"
#define CUTMENUNAME "cutmenu"
#define COPYMENUNAME "copymenu"
#define PASTEMENUNAME "pastemenu"
#define FINDMENUNAME "findmenu"
#define FINDNEXTMENUNAME "findnextmenu"
#define PREFSMENUNAME "prefsmenu"
#define PLUGPREFSMENUNAME "plugprefsmenu"
#define SHOWDOCSMENUNAME "showdocsmenu"
#define SHOWBMBARMENUNAME "showbmbarmenu"
#define SHOWTOOLBARMENUNAME "showtoolbarmenu"
#define SHOWTOOLOUTMENUNAME "showtoolout"
#define SHOWSTATUSMENUNAME "showstatusmenu"
#define SHOWDOCVIEWERMENUNAME "showdocviewer"
#define GOTODEFMENUNAME "gotodefinemenu"
#define OPENINCLUDEMENUNAME "openincludemenu"
#define GOTOLINEMENUNAME "gotolinemenu"
#define SEARCHFORDEFMENUNAME "searchfordefmenu"
#define SEARCHGTKMENUNAME "searchgtkmenu"
#define SEARCHQT5MENUNAME "searchqt5menu"
#define SEARCHQT5ASSISTMENUNAME "searchqt5assistmenu"
#define SEARCHDOXYMENUNAME "searchdoxymenu"
#define GOBACKMENUNAME "gobackmenu"
#define GOFORWARDMENUNAME "goforwardmenu"
#define ABOUTMENUNAME "aboutmenu"
#define HELPMENUNAME "helpmenu"
#define GETPLUGSMENUNAME "getplugsmenu"
#define SELECTTABMENUNAME "selecttabmenu"

#define VIEWSHOWLINENUMERS "viewshowlinenumbers"
#define VIEWAUTOINDENT "viewautoindent"
#define VIEWWRAPLINES "viewwraplines"
#define VIEWNOSYNTAX "viewnosyntax"
#define VIEWWHITESPACE "viewwhitespace"
#define VIEWAUTOCOMPLETE "viewautocomplete"
#define VIEWHIGHLIGHT "viewhighlight"


#ifndef _PAGESTRUCT_
#define _PAGESTRUCT_

enum {NOTYPE=0,FROMFILEMENU,FROMEDITMENU,FROMNAVMENU,FROMFUNCMENU,FROMBOOKMARKMENU,FROMTOOLSMENU,FROMHELPMENU,FROMSELECTION,FROMGOTO,FROMBASICFIND};

struct	args
{
	const char	*name;
	int			type;
	void		*data;
};

enum {TYPEINT=1,TYPESTRING,TYPEBOOL,TYPELIST};

struct pageStruct
{
	GtkScrolledWindow	*pageWindow;
	GtkSourceBuffer		*buffer;
	GtkSourceView		*view;
	char				*fileName;
	char				*filePath;
	char				*realFilePath;
	char				*dirName;
	GtkMenuItem			*navSubMenu;
	bool				rebuildMenu;
	GtkWidget			*tabName;
	GtkTextIter			iter;
	GtkTextIter			match_start;
	GtkTextIter			match_end;
	bool				isFirst;
	GFile				*gFile; 
	GFileMonitor		*monitor;
	bool				itsMe;
	GList				*markList;
	const char			*lang;
	GtkWidget			*tabVbox;
	bool				showingChanged;
	GtkTextMark			*backMark;
	GtkTextTag			*highlightTag;
	GList				*userDataList;
	GtkSourceCompletion	*completion;
	GSList				*regexList;
	int					regexMatchNumber;
	bool				canUndo;
	bool				canRedo;
	bool				isDirty;
	unsigned			pageID;
	char				*searchString;
	char				*replaceString;;
	GtkWidget			*tabButton;
	bool				hidden;
};
#endif


#ifndef _PLUGINDATA_
#define _PLUGINDATA_
struct moduleData
{
	char				*name;
	bool				enabled;
	GModule				*module;
	bool				loaded;
	char				*path;
	bool				unload;
};
#endif

//plugins
struct	plugMenuList
{
	GtkWidget			*menuBar;
	GtkWidget			*menuFile;
	GtkWidget			*menuEdit;
	GtkWidget			*menuFunc;
	GtkWidget			*menuNav;
	GtkWidget			*menuTools;
	GtkWidget			*menuHelp;
	GtkWidget			*menuBookMark;
	GtkWidget			*menuView;
};

struct	plugUserData
{
	plugUserData():type(NOTYPE),userStr(NULL),userStr2(NULL),userInt(0),userData(NULL),userStart(-1),userEnd(-1) {}
	int			type;
	const char	*userStr;
	const char	*userStr2;
	int			userInt;
	void		*userData;
	int			userStart;
	int			userEnd;
};

struct	plugData
{
//menus
	plugMenuList		mlist;
	moduleData			*modData;
//	GList			*plugins;
//install location of KKEdit data
	const char			*dataDir;
//global plugin folder defaults to DATADIR/plugins
	char				*gPlugFolder;
//local plugin folder defaults to ~/.KKEdit/plugins
	char				*lPlugFolder;
//kkedit version string
	const char			*kkeditVersion;
//unused
	char				*htmlFile;
//location of variable that holds the uri to be disp[layed by showDoc
	char**				thePage;
//main mainNotebook
	GtkNotebook			*notebook;
//current page MAYBE NULL!!
	pageStruct			*page;
//cuurent tab
	int					currentTab;
//KKEdits temp folder
	char				*tmpFolder;
//kkedits main window user box's
//top
	GtkWidget			*topUserBox;
	GtkWidget			*mainWindowVPane;
//left
	GtkWidget			*leftUserBox;
	GtkWidget			*mainWindowHPane;
//right
	GtkWidget			*rightUserBox;
	GtkWidget			*secondWindowHPane;
//bottom
	GtkWidget			*bottomUserBox;
	GtkWidget			*secondWindowVPane;
//kkedit main window
	GtkWidget			*mainWindow;
//tool output window buffer;
	GtkTextBuffer		*toolOutBuffer;
//tool output window
	GtkWidget			*toolOutWindow;
//tab popup menu
	GtkWidget			*tabPopUpMenu;
//right click popup menu
	GtkWidget			*contextPopUpMenu;

//leftright user box visiblity ref
	int					leftShow;
	int					rightShow;
//topbottom user box visiblity ref
	int					topShow;
	int					bottomShow;
//locale dir for gettext
	const char			*locale;

//user data passed to plugins.
	plugUserData		userData;
};

void		showDocView(int howtodisplay,char *text,const char *title);
pageStruct	*getPageStructByIDFromPage(int pagenum);
void		showToolOutput(bool immediate);
void		hideToolOutput(bool immediate);
void		showSide(bool left);
void		hideSide(bool left);
void		showTop(bool top);
void		hideTop(bool top);
void		runCommand(char *commandtorun,void *ptr,bool interm,int flags,int useroot,char *title);
void		debugFree(char** ptr);
bool		openFile(const gchar *filepath,int linenumber,bool warn);
bool		saveFile(GtkWidget *widget,gpointer data);
void		newFile(GtkWidget *widget,gpointer data);
int			loadVarsFromFile(char *filepath,args *dataptr);
void		saveVarsToFile(char *filepath,args *dataptr);

#endif