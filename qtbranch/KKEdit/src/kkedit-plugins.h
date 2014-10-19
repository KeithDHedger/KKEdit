/*
 *
 * K.D.Hedger 2014 <kdhedger68713@gmail.com>
 *
 * plugins.h
 *
*/

#ifndef _PLUGINS_
#define _PLUGINS_

#ifndef _USEQT5_
#include <gtksourceview/gtksourceview.h>
#include <gmodule.h>
#endif

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
#ifndef _USEQT5_
	GtkWidget*			pane;
	GtkScrolledWindow*	pageWindow;
	GtkScrolledWindow*	pageWindow2;
	GtkSourceBuffer*	buffer;
	GtkSourceView*		view;
	GtkSourceView*		view2;
	GtkMenuItem*		navSubMenu;
	GtkWidget*			tabName;
	GtkTextIter			iter;
	GtkTextIter			match_start;
	GtkTextIter			match_end;
	GFile*				gFile; 
	GFileMonitor*		monitor;
	GtkWidget*			tabVbox;
	GtkTextMark*		backMark;
	GtkTextTag*			highlightTag;
	GtkSourceCompletion* completion;
#else
	QVBoxLayout*		tabVbox;
#endif
	char*				filePath;
	char*				realFilePath;
	char*				dirName;
	bool				rebuildMenu;
	bool				isFirst;
	char*				fileName;
	bool				itsMe;
	GList*				markList;
	bool				inTop;
	bool				isSplit;
	const char*			lang;
	bool				showingChanged;
	GList*				userDataList;
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
#ifndef _USEQT5_
	GtkWidget*		menuBar;
	GtkWidget*		menuFile;
	GtkWidget*		menuEdit;
	GtkWidget*		menuFunc;
	GtkWidget*		menuNav;
	GtkWidget*		menuTools;
	GtkWidget*		menuHelp;
	GtkWidget*		menuBookMark;
	GtkWidget*		menuView;
#else
#endif
};

struct plugData
{
#ifndef _USEQT5_
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
	GtkNotebook*	notebook;
//current page MAYBE NULL!!
	pageStruct*		page;
//cuurent tab
	int				currentTab;
//KKEdits temp folder
	char*			tmpFolder;
//kkedits main window user box's
//top
	GtkWidget*		topUserBox;
	GtkWidget*		mainWindowVPane;
//left
	GtkWidget*		leftUserBox;
	GtkWidget*		mainWindowHPane;
//right
	GtkWidget*		rightUserBox;
	GtkWidget*		secondWindowHPane;
//bottom
	GtkWidget*		bottomUserBox;
	GtkWidget*		secondWindowVPane;
//kkedit main window
	GtkWidget*		mainWindow;
//tool output window buffer;
	GtkTextBuffer*	toolOutBuffer;
//tool output window
	GtkWidget*		toolOutWindow;
//tab popup menu
	GtkWidget*		tabPopUpMenu;
//right click popup menu
	GtkWidget*		contextPopUpMenu;

//leftright user box visiblity ref
	int				leftShow;
	int				rightShow;
//topbottom user box visiblity ref
	int				topShow;
	int				bottomShow;
//locale dir for gettext
	const char*		locale;
#else
#endif
};

#ifndef _USEQT5_

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
#ifndef _USEQT5_
bool		saveFile(GtkWidget* widget,gpointer data);
#else
//TODO//
bool		saveFile(void);
#endif
#ifndef _USEQT5_
void		newFile(GtkWidget* widget,gpointer data);
#else
//TODO//
void		newFile(void);
#endif
void		loadVarsFromFile(char* filepath,args* dataptr);
void		saveVarsToFile(char* filepath,args* dataptr);

#else
#endif

#endif
