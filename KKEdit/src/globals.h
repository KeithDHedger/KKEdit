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
#include <gtksourceview/gtksourcestyleschememanager.h>

#include "config.h"
#include "sliceclass.h"
#include "kkedit-plugins.h"

#ifdef _BUILDDOCVIEWER_
#include <webkit/webkit.h>
#endif

#ifdef _ASPELL_
#include <aspell.h>
#endif

#ifndef _GLOBALS_
#define _GLOBALS_


#define REPLACE			100
#define FINDNEXT		200
#define FINDPREV		300

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "https://sites.google.com/site/keithhedgersyard/home"

#define TOOL_ASYNC 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2
#define TOOL_SHOW_DOC 4
#define	TOOL_INSERT_MASK 0b10111
#define	TOOL_VIEW_OP 16

#define VALIDCHARS "[]a-zA-Z0-9:/@ ()_-,.*#;[\"]"
#define VALIDFILENAMECHARS "[A-Za-z0-9_-./ <>]"
#define VALIDFUNCTIONCHARS "[A-Za-z0-9_*:/@ ()-,.#;[\"]"

#define BOOKMAXMARKMENULEN 40
#define MAXRECENT 10

#define PLACEHOLDERINFO "PLACEHOLDERS:\n%t - Currently selected text. Passed to command as $KKEDIT_SELECTION\n%f - Filepath of the current document. Passed to command as $KKEDIT_CURRENTFILE\n%d - Directory of the current document or $HOME. Passed to command as $KKEDIT_CURRENTDIR\n%i - The location of the globally installed tools. Passed to command as $KKEDIT_DATADIR\n%h - Tempory file for displaying html in doc viewer. Passed to command as $KKEDIT_HTMLFILE\n%l - Highlihting language. Passed to command as $KKEDIT_SOURCE_LANG"

#define KEYCUTSINFO "To set a custom shortcut:\nClick in the appropriate box and press CONTROL ( and optionally SHIFT ) plus your custom key.\nJust press 'Delete' to remove the shortcut\nClick 'Apply' to keep changes or 'Cancel' to discard any changes."

#define MARK_TYPE_1		"one"
#define MARK_TYPE_2		"two"

#define USEURI			-1
#define USEFILE			-2

#define NUMSHORTCUTS	13

enum {PIXBUF_COLUMN,TEXT_COLUMN,BUTTON_NUM};

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
//	GtkWidget*			toolOutVBox;
//	GtkWidget*			toolOutputBuffer;
//	GtkWidget*			toolOutputView;
};

struct toolStruct
{
	char*				menuName;
	char*				filePath;
	char*				command;
	int					flags;
	bool				inTerminal;					
	bool				inPopUp;
	bool				alwaysPopup;
	bool				clearView;
	char*				comment;
	bool				global;
	bool				runAsRoot;
	int					keyCode;
	bool				useBar;
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

struct bookMarksNew
{
	pageStruct*			page;
	char*				label;
	GtkSourceMark*		mark;
	char*				markName;
	int					line;
};

#include "backclass.h"

extern HistoryClass*	history;

extern GtkWidget*		window;
extern GtkAccelGroup*	accgroup;
extern GtkNotebook*		notebook;

extern GList*			newBookMarksList;
extern GtkWidget*		menuBookMark;
extern GtkWidget*		menuBookMarkSubMenu;
extern char*			highlightColour;
extern char*			tmpHighlightColour;
extern bool				showBMBar;
extern GtkWidget*		bmHighlightBox;
extern int				bmMarkNumber;

extern char*			toolBarLayout;
extern GtkToolbar*		toolBar;
extern GtkWidget*		toolBarBox;

extern GtkWidget*		menubar;
extern GtkWidget*		menufile;
extern GtkWidget*		menuedit;
extern GtkWidget*		menufunc;
extern GtkWidget*		menunav;
extern GtkWidget*		menutools;
extern GtkWidget*		menuView;

extern GtkWidget*		menuhelp;
extern GtkWidget*		menuprint;
extern GtkWidget*		menuclose;
extern GtkWidget*		menucloseall;
extern GtkWidget*		menusaveall;
extern GtkWidget*		menurevert;

extern GtkWidget*		menuItemOpen;
extern GtkWidget*		menuToolOut;
extern GtkWidget*		menuStatusBar;

extern GtkWidget*		redoMenu;
extern GtkWidget*		undoMenu;
extern GtkWidget*		redoAllMenu;
extern GtkWidget*		undoAllMenu;
extern GtkWidget*		cutMenu;
extern GtkWidget*		copyMenu;
extern GtkWidget*		pasteMenu;

extern GtkWidget*		saveMenu;
extern GtkWidget*		saveAsMenu;

extern GtkWidget*		lineNumberWidget;
extern GtkWidget*		findApiWidget;
extern GtkWidget*		findQtApiWidget;
extern GtkWidget*		findDefWidget;
extern GtkWidget*		liveSearchWidget;

extern int				currentPage;
extern bool				nagScreen;
extern bool				tmpNagScreen;

extern GtkWidget*		prefswin;
extern bool				indent;
extern bool				lineNumbers;
extern bool				lineWrap;
extern bool				highLight;
extern bool				singleUse;
extern int				tabWidth;
extern char*			fontAndSize;
extern char*			terminalCommand;
extern char*			rootCommand;
extern int				depth;
extern bool				onExitSaveSession;
extern bool				restoreBookmarks;
extern char*			styleName;
extern bool				noDuplicates;
extern bool				tmpNoDuplicates;
extern bool				noWarnings;
extern bool				tmpNoWarnings;
extern bool				readLinkFirst;
extern bool				tmpReadLinkFirst;

extern GtkWidget*		fontButton;
extern GtkWidget*		terminalBox;
extern GtkWidget*		rootCommandBox;

//key short cuts
extern GtkWidget*		keysWindow;

extern bool				tmpIndent;
extern bool				tmpLineNumbers;
extern bool				tmpLineWrap;
extern bool				tmpHighLight;
extern bool				tmpSingleUse;
extern int				tmpTabWidth;
extern int				tmpDepth;
extern bool				tmpSaveSessionOnExit;
extern bool				tmpRestoreBookmarks;
extern char*			tmpStyleName;

extern GtkWidget*		toolNameWidget;
extern GtkWidget*		commandLineWidget;
extern GtkWidget*		keyWidget;
extern GtkWidget*		commentWidget;
extern GtkWidget*		inTermWidget;
extern GtkWidget*		inPopupWidget;
extern GtkWidget*		alwaysPopupWidget;

extern GtkWidget*		syncWidget;
extern GtkWidget*		ignoreWidget;
extern GtkWidget*		pasteWidget;
extern GtkWidget*		replaceWidget;
extern GtkWidget*		outputWidget;
extern GtkWidget*		showDocWidget;
extern GtkWidget*		toolSelect;
extern GtkWidget*		clearViewWidget;
extern GtkWidget*		runAsRootWidget;
extern GtkWidget*		useBarWidget;

extern toolStruct*		selectedToolFromDrop;

//view tool output
extern GtkWidget*		mainVPane;
extern bool				showToolOutWin;
extern GtkWidget*		toolOutputView;
extern GtkTextBuffer*	toolOutputBuffer;
extern GtkWidget*		toolOutVBox;
extern bool				showStatus;
extern GtkWidget*		statusWidget;

extern GList*			toolsList;

extern GtkWidget*		restoreBMs;

extern int				windowWidth;
extern int				windowHeight;
extern int				windowX;
extern int				windowY;
extern bool				wrapSearch;
extern bool				insensitiveSearch;
extern bool				useRegex;
extern bool				replaceAll;
extern bool				findInAllFiles;
extern bool				hightlightAll;

extern int				currentTabNumber;
extern int 				untitledNumber;

extern GtkToolItem*		newButton;
extern GtkToolItem*		openButton;
extern GtkToolItem*		saveButton;
extern GtkToolItem*		redoButton;
extern GtkToolItem*		undoButton;
extern GtkToolItem*		cutButton;
extern GtkToolItem*		copyButton;
extern GtkToolItem*		pasteButton;
extern GtkToolItem*		findButton;
extern GtkToolItem*		gotoDefButton;
extern GtkToolItem*		backButton;
extern GtkToolItem*		gotoLineButton;
extern GtkToolItem*		findApiButton;
extern GtkToolItem*		findQtApiButton;
extern GtkToolItem*		findFuncDefButton;
extern GtkToolItem*		liveSearchButton;

//find replace
extern GtkWidget*		findReplaceDialog;
extern GtkWidget*		findBox;
extern GtkWidget*		replaceBox;
extern GtkWidget*		findDropBox;
extern GtkWidget*		replaceDropBox;
extern GSList*			findList;
extern GSList*			replaceList;

//custom toolbar
extern GtkWidget*		fromHBox;
extern GtkWidget*		iconViewBox;
extern bool				showToolBar;

extern int				listFunction;
extern GtkWidget*		funcListDrop;

extern char*			functionSearchText;
extern char*			thePage;
extern char*			htmlFile;
extern char*			htmlURI;
extern char*			tmpFolderName;

//runtime deps
extern int				gotManEditor;
extern int				gotDoxygen;

extern GtkSourceStyleSchemeManager*	schemeManager;
extern GtkSourceStyleScheme*		styleScheme;

#ifdef _BUILDDOCVIEWER_
extern GtkWidget*		docView;
extern WebKitWebView*	webView;
extern bool				showHideDocviewer;
extern GtkWidget*		showDocViewWidget;
#endif

extern GtkWidget*		spellCheckWord;
extern GtkWidget*		wordListDropbox;
extern char*			badWord;
extern char*			goodWord;
#ifdef _ASPELL_
extern AspellConfig*	aspellConfig;
extern AspellSpeller*	spellChecker;
#endif
extern StringSlice*		globalSlice;
extern unsigned int		shortCuts[100][2];
extern GtkWidget*		progressWindow;
extern GtkWidget*		progressBar;

//plugins
#define PLUGPATH "/media/LinuxData/Development/Projects/KKEdit/KKEdit/resources/plugs"
extern GList*			pluginList;
extern char*			pluginFolder;
extern plugData*		globalPlugData;

pageStruct* getPageStructPtr(int pagenum);
void getMimeType(char* filepath,void* ptr);
void setLanguage(pageStruct* page);
void runCommand(char* commandtorun,void* ptr,bool interm,int flags,int useroot);
functionData* getFunctionByName(char* name,bool recurse);
void destroyData(functionData* fdata);
void getRecursiveTagList(char* filepath,void* ptr);
void buildToolsList(void);
char* deleteSlice(char* srcstring,char* delstr);
void getRecursiveTagListFileName(char* filepath,void* ptr);
void scrollToIterInPane(pageStruct* page,GtkTextIter* iter);
void goBack(GtkWidget* widget,gpointer data);
void showBarberPole(const char* title);
void killBarberPole(void);

#endif




