/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * globals.h
 *
*/

#include "kkedit-includes.h"
#include "config.h"

#ifndef _GLOBALS_
#define _GLOBALS_
#if 1
#undef _DEBUG_FREE_

#ifndef _USEQT5_
	#include <gtk/gtk.h>
//	typedef GtkWidget Widget;
#else
	#include <glib.h>
	#include <QtWidgets>
//	typedef QWidget Widget;

	extern QApplication*	holdapp;
#endif


#define REPLACE			100
#define FINDNEXT		200
#define FINDPREV		300

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "https://sites.google.com/site/kkeditlinuxtexteditor/home"
#define PLUGINSLINK "https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins#getplugs"
#define NAGTIMELINK "https://dl.dropboxusercontent.com/s/xcvp8af3qp7ffxg/NAGTIME"

#define TOOL_ASYNC 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2
#define TOOL_SHOW_DOC 4
#define	TOOL_INSERT_MASK 0b10111
#define	TOOL_VIEW_OP 16

#define VALIDFILENAMECHARS "[A-Za-z0-9_-./ <>]"
#define VALIDFUNCTIONCHARS "[A-Za-z0-9_*:/@ ()-,.#;[\"]"

#define BOOKMAXMARKMENULEN 40
#define MAXRECENT 10
#define MAXMENUFUNCLEN 100

#define MARK_TYPE_1		"one"
#define MARK_TYPE_2		"two"

#define USEURI			-1
#define USEFILE			-2

#define	POLEPATH		PREFIX "/bin/KKEditProgressBar"
#define CUSTOMWORDFILE ".KKEdit/customcompletions"

#define NUMSHORTCUTS	14

enum {PIXBUF_COLUMN,TEXT_COLUMN,BUTTON_NUM};

#if 1
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
	Widget*			tabName;
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
	Widget*			tabVbox;
	bool				showingChanged;
	GtkTextMark*		backMark;
	GtkTextTag*			highlightTag;
	GList*				userDataList;
	GtkSourceCompletion* completion;
	gpointer			reserved2;
	gpointer			reserved3;
	gpointer			reserved4;
};
#endif
#endif

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
	Widget*				mark;
	char*				markName;
	int					line;
};

//extern HistoryClass*	history;

extern Widget*		window;
extern Widget*	accgroup;
extern Widget*		notebook;

extern GList*			newBookMarksList;
extern Widget*		menuBookMark;
extern Widget*		menuBookMarkSubMenu;
extern char*			highlightColour;
extern char*			tmpHighlightColour;
extern bool				showBMBar;
extern Widget*		bmHighlightBox;
extern int				bmMarkNumber;

extern char*			toolBarLayout;
extern GtkToolbar*		toolBar;
extern Widget*		toolBarBox;

extern Widget*		menubar;
extern Widget*		menufile;
extern Widget*		menuedit;
extern Widget*		menufunc;
extern Widget*		menunav;
extern Widget*		menutools;
extern Widget*		menuView;

extern Widget*		menuhelp;
extern Widget*		menuprint;
extern Widget*		menuclose;
extern Widget*		menucloseall;
extern Widget*		menusaveall;
extern Widget*		menurevert;

extern Widget*		menuItemOpen;
extern Widget*		menuToolOut;
extern Widget*		menuStatusBar;

extern Widget*		redoMenu;
extern Widget*		undoMenu;
extern Widget*		redoAllMenu;
extern Widget*		undoAllMenu;
extern Widget*		cutMenu;
extern Widget*		copyMenu;
extern Widget*		pasteMenu;

extern Widget*		saveMenu;
extern Widget*		saveAsMenu;

extern Widget*		lineNumberWidget;
extern Widget*		findApiWidget;
extern Widget*		findQtApiWidget;
extern Widget*		findDefWidget;
extern Widget*		liveSearchWidget;

extern int				currentPage;
//nag
extern bool				nagScreen;
extern bool				tmpNagScreen;
extern unsigned int		nagTime;
extern unsigned int		lastNagTime;
extern bool				timeToNag;
extern unsigned int		lastUpdate;
extern unsigned int		lastPlugUpdate;
extern unsigned int		updateWait;

//docview size and position
extern int				docWindowWidth;
extern int				docWindowHeight;
extern int				docWindowX;
extern int				docWindowY;

//prefs
extern Widget*		prefswin;
extern bool				indent;
extern bool				lineNumbers;
extern bool				lineWrap;
extern bool				highLight;
extern bool				noSyntax;
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
extern bool				autoShowComps;
extern bool				tmpAutoShowComps;
extern bool				autoCheck;
extern bool				tmpAutoCheck;

extern uint				autoShowMinChars;
extern int				tmpAutoShowMinChars;

extern Widget*		fontButton;
extern Widget*		terminalBox;
extern Widget*		rootCommandBox;
extern Widget*		defaultBrowserBox;
extern char*			browserCommand;

//key short cuts
extern Widget*		keysWindow;

extern bool				tmpIndent;
extern bool				tmpLineNumbers;
extern bool				tmpLineWrap;
extern bool				tmpHighLight;
extern bool				tmpNoSyntax;
extern bool				tmpSingleUse;
extern int				tmpTabWidth;
extern int				tmpDepth;
extern bool				tmpSaveSessionOnExit;
extern bool				tmpRestoreBookmarks;
extern char*			tmpStyleName;

extern Widget*		toolNameWidget;
extern Widget*		commandLineWidget;
extern Widget*		keyWidget;
extern Widget*		commentWidget;
extern Widget*		inTermWidget;
extern Widget*		inPopupWidget;
extern Widget*		alwaysPopupWidget;

extern Widget*		syncWidget;
extern Widget*		ignoreWidget;
extern Widget*		pasteWidget;
extern Widget*		replaceWidget;
extern Widget*		outputWidget;
extern Widget*		showDocWidget;
extern Widget*		toolSelect;
extern Widget*		clearViewWidget;
extern Widget*		runAsRootWidget;
extern Widget*		useBarWidget;

extern toolStruct*		selectedToolFromDrop;

//view tool output
extern Widget*		mainVPane;
extern bool				showToolOutWin;
extern Widget*		toolOutputView;
//extern GtkTextBuffer*	toolOutputBuffer;
extern Widget*		toolOutVBox;
extern bool				showStatus;
extern Widget*		statusWidget;

extern GList*			toolsList;

extern Widget*		restoreBMs;

//main window
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
extern int				toolOutHeight;
extern int				bottomVPaneHite;
extern int				topVPaneHite;

extern Widget*		mainWindowScrollbox;

extern Widget*		mainWindowVBox;
extern Widget*		mainTopUserVBox;
extern Widget*		mainWindowHBox;
extern Widget*		mainLeftUserVBox;
extern Widget*		mainNotebookVBox;
extern Widget*		mainRightUserVBox;
extern Widget*		mainBottomUserVBox;

extern Widget*		mainWindowVPane;
extern Widget*		secondWindowVPane;

extern Widget*		mainWindowHPane;
extern Widget*		secondWindowHPane;

extern int				currentTabNumber;
extern int 				untitledNumber;

extern Widget*		newButton;
extern Widget*		openButton;
extern Widget*		saveButton;
extern Widget*		redoButton;
extern Widget*		undoButton;
extern Widget*		cutButton;
extern Widget*		copyButton;
extern Widget*		pasteButton;
extern Widget*		findButton;
extern Widget*		gotoDefButton;
extern Widget*		backButton;
extern Widget*		gotoLineButton;
extern Widget*		findApiButton;
extern Widget*		findQtApiButton;
extern Widget*		findFuncDefButton;
extern Widget*		liveSearchButton;

//find replace
extern Widget*		findReplaceDialog;
extern Widget*		findBox;
extern Widget*		replaceBox;
extern Widget*		findDropBox;
extern Widget*		replaceDropBox;
//extern GSList*			findList;
//extern GSList*			replaceList;

//custom toolbar
extern Widget*		fromHBox;
extern Widget*		iconViewBox;
extern bool				showToolBar;

extern int				listFunction;
extern Widget*		funcListDrop;

extern char*			functionSearchText;
extern char*			thePage;
extern char*			htmlFile;
extern char*			htmlURI;
extern char*			tmpFolderName;

//runtime deps
extern int				gotManEditor;
extern int				gotDoxygen;

//extern //GtkSourceStyleSchemeManager*	schemeManager;
//extern //GtkSourceStyleScheme*		styleScheme;

#ifdef _BUILDDOCVIEWER_
extern Widget*		docView;
extern WebKitWebView*	webView;
extern bool				showHideDocviewer;
extern Widget*		showDocViewWidget;
#endif

extern Widget*		spellCheckWord;
extern Widget*		wordListDropbox;
extern char*			badWord;
extern char*			goodWord;
#ifdef _ASPELL_
extern AspellConfig*	aspellConfig;
extern AspellSpeller*	spellChecker;
#endif
extern StringSlice*		globalSlice;

extern unsigned int		shortCuts[NUMSHORTCUTS][2];
extern char*			shortCutStrings[NUMSHORTCUTS];

extern Widget*		progressWindow;
extern Widget*		progressBar;

#include "pluginclass.h"

extern PluginClass*		globalPlugins;

//save and load var lists
extern int				intermarg;
extern int				flagsarg;
extern int				inpopup;
extern int				alwayspopup;
extern int				clearview;
extern char*			commandarg;
extern char*			commentarg;
extern char*			menuname;
extern int				rootarg;
extern int				keycode;
extern int				usebar;

extern char*			windowAllocData;
extern char*			docWindowAllocData;
extern args				kkedit_window_rc[];
extern args				kkedit_rc[];
extern args				tool_vars[];
extern args				keybindings_rc[];

//status bar message
extern char*			statusMessage;

extern const char*		localeLang;

void plugRunFunction(gpointer data,gpointer funcname);
pageStruct* getPageStructPtr(int pagenum);
void getMimeType(char* filepath,void* ptr);
void setLanguage(pageStruct* page);
void runCommand(char* commandtorun,void* ptr,bool interm,int flags,int useroot,char* title);
functionData* getFunctionByName(char* name,bool recurse);
void destroyData(functionData* fdata);
void getRecursiveTagList(char* filepath,void* ptr);
void buildToolsList(void);
void getRecursiveTagListFileName(char* filepath,void* ptr);
void scrollToIterInPane(pageStruct* page,GtkTextIter* iter);
void goBack(Widget* widget,gpointer data);
void showBarberPole(const char* title);
void killBarberPole(void);
void debugFree(gpointer ptr,const char* message);
char* truncateWithElipses(char* str,unsigned int maxlen);

#endif
#endif




