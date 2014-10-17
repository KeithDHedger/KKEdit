/*
 *
 * K.D.Hedger 2014 <kdhedger68713@gmail.com>
 *
 * plugins.h
 *
*/

#ifndef _PLUGINS_
#define _PLUGINS_
#if 1
#include "config.h"

#ifndef _USEQT5_
typedef GtkWidget Widget;
#include <gtksourceview/gtksourceview.h>
#else
typedef QWidget Widget;
#endif
#include <gmodule.h>

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
#define SEARCHDOXYMENUNAME "searchdoxymenu"
#define GOBACKMENUNAME "gobackmenu"
#define ABOUTMENUNAME "aboutmenu"
#define HELPMENUNAME "helpmenu"
#define GETPLUGSMENUNAME "getplugsmenu"

#ifndef _PAGESTRUCT_
#define _PAGESTRUCT_

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

struct pageStruct
{
	Widget*			pane;
	Widget*			pageWindow;
	Widget*			pageWindow2;
	Widget*			buffer;
	Widget*			view;
	Widget*			view2;
	char*				filePath;
	char*				realFilePath;
	char*				dirName;
	Widget*		navSubMenu;
	bool				rebuildMenu;
	Widget*			tabName;
//	GtkTextIter			iter;
//	GtkTextIter			match_start;
//	GtkTextIter			match_end;
	bool				isFirst;
	char*				fileName;
//	GFile*				gFile; 
//	GFileMonitor*		monitor;
	bool				itsMe;
	GList*				markList;
	bool				inTop;
	bool				isSplit;
	const char*			lang;
	Widget*			tabVbox;
	bool				showingChanged;
//	GtkTextMark*		backMark;
//	GtkTextTag*			highlightTag;
	GList*				userDataList;
//	GtkSourceCompletion* completion;
	gpointer			reserved2;
	gpointer			reserved3;
	gpointer			reserved4;
};
#endif

#ifndef _PLUGINDATA_
#define _PLUGINDATA_
struct moduleData
{
	char*		name;
	bool		enabled;
	GModule*	module;
	bool		loaded;
	char*		path;
	bool		unload;
};
#endif

//plugins
struct plugMenuList
{
	Widget*		menuBar;
	Widget*		menuFile;
	Widget*		menuEdit;
	Widget*		menuFunc;
	Widget*		menuNav;
	Widget*		menuTools;
	Widget*		menuHelp;
	Widget*		menuBookMark;
	Widget*		menuView;
};

struct plugData
{
//menus
	plugMenuList	mlist;
	moduleData*		modData;
//	GList*			plugins;
//install location of KKEdit data
	const char*		dataDir;
//global plugin folder defaults to DATADIR/plugins
	char*			gPlugFolder;
//local plugin folder defaults to ~/.KKEdit/plugins
	char*			lPlugFolder;
//kkedit version string
	const char*		kkeditVersion;
//unused
	char*			htmlFile;
//location of variable that holds the uri to be disp[layed by showDoc
	char**			thePage;
//main notebook
	Widget*	notebook;
//current page MAYBE NULL!!
	pageStruct*		page;
//cuurent tab
	int				currentTab;
//KKEdits temp folder
	char*			tmpFolder;
//kkedits main window user box's
//top
	Widget*		topUserBox;
	Widget*		mainWindowVPane;
//left
	Widget*		leftUserBox;
	Widget*		mainWindowHPane;
//right
	Widget*		rightUserBox;
	Widget*		secondWindowHPane;
//bottom
	Widget*		bottomUserBox;
	Widget*		secondWindowVPane;
//kkedit main window
	Widget*		mainWindow;
//tool output window buffer;
//	GtkTextBuffer*	toolOutBuffer;
//tool output window
	Widget*		toolOutWindow;
//tab popup menu
	Widget*		tabPopUpMenu;
//right click popup menu
	Widget*		contextPopUpMenu;

//leftright user box visiblity ref
	int				leftShow;
	int				rightShow;
//topbottom user box visiblity ref
	int				topShow;
	int				bottomShow;
//locale dir for gettext
	const char*		locale;
};

void		showDocView(int howtodisplay,char* text,const char* title);
pageStruct*	getPageStructPtr(int pagenum);
void		showToolOutput(bool immediate);
void		hideToolOutput(bool immediate);
void		showSide(bool left);
void		hideSide(bool left);
void		showTop(bool top);
void		hideTop(bool top);
void		runCommand(char* commandtorun,void* ptr,bool interm,int flags,int useroot,char* title);
void		debugFree(gpointer ptr,const char* message);
bool		openFile(const gchar *filepath,int linenumber,bool warn);
bool		saveFile(Widget* widget,gpointer data);
void		newFile(Widget* widget,gpointer data);
void		loadVarsFromFile(char* filepath,args* dataptr);
void		saveVarsToFile(char* filepath,args* dataptr);
#endif
#endif
