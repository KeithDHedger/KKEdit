/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
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

#ifdef BUILDDOCVIEWER
#include <webkit/webkit.h>
#endif

#ifndef _GLOBALS_
#define _GLOBALS_

#define REPLACE			100
#define FINDNEXT		200
#define FINDPREV		300

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com"
#define VERSION "0.0.12"

#define TOOL_ASYNC 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2

#define VALIDCHARS "[]a-zA-Z0-9:/@ ()_-,.*#;[\"]"
#define VALIDFILENAMECHARS "[A-Za-z0-9_-./]"
#define VALIDFUNCTIONCHARS "[A-Za-z0-9_*]"

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
	GFile*				gFile;
	GFileMonitor*		monitor;
	bool				itsMe;
};

struct toolStruct
{
	char*				menuName;
	char*				filePath;
	char*				command;
	char*				currentDir;
	int					flags;
	bool				inTerminal;					
};

struct functionData
{
	char*				name;
	char*				type;
	int					line;
	char*				file;
	char*				define;
	int					intab;
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
extern GtkWidget*		menubookmarksub;
extern GtkWidget*		menuhelp;
extern GtkWidget*		redoMenu;
extern GtkWidget*		undoMenu;
extern GtkWidget*		saveMenu;
extern GtkWidget*		saveAsMenu;

extern int				currentPage;

extern GtkWidget*		prefswin;
extern bool				indent;
extern bool				lineNumbers;
extern bool				lineWrap;
extern bool				highLight;
extern bool				singleUse;
extern int				tabWidth;
extern char*			fontAndSize;
extern char*			terminalCommand;
extern int				depth;

extern GtkWidget*		fontBox;
extern GtkWidget*		terminalBox;
extern bool				tmpIndent;
extern bool				tmpLineNumbers;
extern bool				tmpLineWrap;
extern bool				tmpHighLight;
extern bool				tmpSingleUse;
extern int				tmpTabWidth;
extern int				tmpDepth;

extern GtkWidget*		toolNameWidget;
extern GtkWidget*		commandLineWidget;
extern GtkWidget*		inTermWidget;
extern GtkWidget*		syncWidget;
extern GtkWidget*		ignoreWidget;
extern GtkWidget*		pasteWidget;
extern GtkWidget*		replaceWidget;

extern bool				inTerm;
extern bool				runSync;
extern bool				ignoreOut;
extern bool				pasteOut;
extern bool				replaceOut;
extern bool				editTool;

extern int				windowWidth;
extern int				windowHeight;
extern int				windowX;
extern int				windowY;
extern bool				wrapSearch;
extern bool				insensitiveSearch;
extern bool				replaceAll;

extern int				currentTabNumber;
extern int 				untitledNumber;

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

#ifdef BUILDDOCVIEWER
extern GtkWidget*		docView;
extern WebKitWebView*	webView;
#endif

pageStruct* getPageStructPtr(int pagenum);
void getMimeType(char* filepath,void* ptr);
void setLanguage(pageStruct* page);
void runCommand(char* commandtorun,void* ptr,bool interm,int flags);
functionData* getFunctionByName(char* name);
void destroyData(functionData* fdata);
void getRecursiveTagList(char* filepath,void* ptr);

#endif




