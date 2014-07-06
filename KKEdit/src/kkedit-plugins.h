/*
 *
 * K.D.Hedger 2014 <kdhedger68713@gmail.com>
 *
 * plugins.h
 *
*/

#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

#ifndef _PLUGINS_
#define _PLUGINS_

#ifndef _PAGESTRUCT_
#define _PAGESTRUCT_
struct pageStruct
{
	GtkWidget*			pane;
	GtkScrolledWindow*	pageWindow;
	GtkScrolledWindow*	pageWindow2;
	GtkSourceBuffer*	buffer;
	GtkSourceView*		view;
	GtkSourceView*		view2;
	char*				filePath;
	char*				realFilePath;
	char*				dirName;
	GtkMenuItem*		navSubMenu;
	bool				rebuildMenu;
	GtkWidget*			tabName;
	GtkTextIter			iter;
	GtkTextIter			match_start;
	GtkTextIter			match_end;
	bool				isFirst;
	char*				fileName;
	GFile*				gFile; 
	GFileMonitor*		monitor;
	bool				itsMe;
	GList*				markList;
	bool				inTop;
	bool				isSplit;
	const char*			lang;
	GtkWidget*			tabVbox;
	bool				showingChanged;
	GtkTextMark*		backMark;
	GtkTextTag*			highlightTag;
};
#endif

//plugins
struct plugMenuList
{
	GtkWidget*		menuBar;
	GtkWidget*		menuFile;
	GtkWidget*		menuEdit;
	GtkWidget*		menuFunc;
	GtkWidget*		menuNav;
	GtkWidget*		menuTools;
	GtkWidget*		menuHelp;
	GtkWidget*		menuBookMark;
	GtkWidget*		menuView;
};

struct plugData
{
//menus
	plugMenuList	mlist;
//install location of KKEdit data
	const char*		dataDir;
//plugin folder defaults to dataDir/plugs
	char*			plugFolder;
//function pointer to showDocView
	void*           showDoc;
//unused
	char*			htmlFile;
//location of variable that holds the uri to be disp[layed by showDoc
	char**			thePage;
//get the page struct for the current page gives acces to text buffer text view etc
	void*			getPageStruct;
//current page MAYBE NULL!!
	pageStruct*		page;
};

#define USEFILE			-2
#define USEURI			-1

#endif