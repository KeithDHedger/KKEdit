/*
 *
 * K.D.Hedger 2014 <kdhedger68713@gmail.com>
 *
 * plugins.h
 *
*/

#ifndef _PLUGINS_
#define _PLUGINS_

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
	plugMenuList	mlist;
	const char*		dataDir;
	char*			plugFolder;
	void*           showDoc;
	char*			htmlFile;
	char**			thePage;
};

#define USEFILE			-2
#define USEURI			-1

#endif