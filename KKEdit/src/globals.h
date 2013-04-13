/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 * globals.h
 *
*/
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

#ifndef _GLOBALS_
#define _GLOBALS_

#define REPLACE			100
#define FINDNEXT		200
#define FINDPREV		300

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com"
#define VERSION "0.0.7"

#define TOOL_SYNC 8
//#define TOOL_ASYNC 
//#define TOOL_INSERT_OP 4
//#define TOOL_PASTE_OP 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2

struct pageStruct
{
	GtkScrolledWindow*	pageWindow;
	GtkSourceBuffer*	buffer;
	GtkSourceView*		view;
	char*				filePath;
	GtkMenuItem*		navSubMenu;
	bool				rebuildMenu;
	GtkWidget*			tabName;
	GtkTextIter			iter;
	GtkTextIter			match_start;
	GtkTextIter			match_end;
	bool				isFirst;
	char*				fileName;
};

struct toolStruct
{
	char*				menuName;
	char*				filePath;
	char*				currentDir;
	int					flags;
	bool				inTerminal;					
};

extern GtkWidget*		window;
extern GtkNotebook*		notebook;

extern GtkWidget*		menubar;
extern GtkWidget*		menufile;
extern GtkWidget*		menuedit;
extern GtkWidget*		menufunc;
extern GtkWidget*		menunav;
extern GtkWidget*		menutools;
extern GtkWidget*		menubookmark;
extern GtkWidget*		menuhelp;
extern GtkWidget*		redoMenu;
extern GtkWidget*		undoMenu;
extern GtkWidget*		saveMenu;

extern int				currentPage;

extern bool				indent;
extern bool				lineNumbers;
extern bool				lineWrap;
extern bool				highLight;
extern int				tabWidth;
extern char*			fontAndSize;
extern char*			terminalCommand;

extern int				windowWidth;
extern int				windowHeight;
extern int				windowX;
extern int				windowY;
extern bool				wrapSearch;
extern bool				insensitiveSearch;
extern bool				replaceAll;

extern int				currentTabNumber;

extern GtkToolItem*		newButton;
extern GtkToolItem*		openButton;
extern GtkToolItem*		saveButton;
extern GtkToolItem*		saveasButton;
extern GtkToolItem*		closeButton;
extern GtkToolItem*		redoButton;
extern GtkToolItem*		undoButton;

extern GtkWidget*		findReplaceDialog;
extern GtkWidget*		findBox;
extern GtkWidget*		replaceBox;

pageStruct* getPageStructPtr(int pagenum);
void getMimeType(char* filepath,void* ptr);
void setLanguage(pageStruct* page);
int getTabFromPath(char* filepath);
void runCommand(char* commandtorun,void* ptr,bool interm);

#endif




