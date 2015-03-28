/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * globals.h
 *
*/

#include "kkedit-includes.h"

#ifndef _GLOBALS_
#define _GLOBALS_

#define DBG0 0
#define DBG1 1
#define DBG2 2
#define DBG3 3
#define DBG4 4
#define DBGFREE 5

#define _DEBUGLEVEL_ DBG3

#define REPLACE			100
#define FINDNEXT		200
#define FINDPREV		300

#if _DEBUGLEVEL_ == DBG0
#define ERRDATA
#endif

#if _DEBUGLEVEL_ == DBG1 || _DEBUGLEVEL_ == DBG2 || _DEBUGLEVEL_ == DBG3
#define ERRDATA errLine=__LINE__,errFile=__FILE__,errFunc=__func__;
#endif

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "https://sites.google.com/site/kkeditlinuxtexteditor/home"
#define PLUGINSLINK "https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins#getplugs"
#define NAGTIMELINK "https://dl.dropboxusercontent.com/s/xcvp8af3qp7ffxg/NAGTIME"
#define DBUSNAME "org.keithhedge.kkedit"

#define TOOL_ASYNC 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2
#define TOOL_SHOW_DOC 4
#define	TOOL_INSERT_MASK 0b10111
#define	TOOL_VIEW_OP 16

#define VALIDFILENAMECHARS "[A-Za-z0-9_-./ <>]"
#define VALIDFUNCTIONCHARS "[A-Za-z0-9_*:/@ ()-,.#;[\"]"

#define MAXRECENT 10
#define MAXTEXTWIDTH 500

#define MARK_TYPE_1		"one"
#define MARK_TYPE_2		"two"

#define USEURI			-1
#define USEFILE			-2

#define	POLEPATH		PREFIX "/bin/KKEditProgressBar"
#define CUSTOMWORDFILE ".KKEdit/customcompletions"

#define NUMSHORTCUTS	14
#define PLATFORM		"gtk"

enum {PIXBUF_COLUMN,TEXT_COLUMN,BUTTON_NUM};

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

extern int				errno;
extern int				errLine;
extern const char		*errFile;
extern const char		*errFunc;

extern GApplication*	mainApp;
extern bool				busyFlag;
extern bool				autoSeleced;
extern bool				sessionBusy;

//app
//main window
extern GtkWidget*		mainWindow;
extern GtkWidget*		menuBar;
extern GtkToolbar*		toolBar;
extern GtkWidget*		statusBarMenu;
extern GtkNotebook*		mainNotebook;

//file menu
extern GtkWidget*		fileMenu;
extern GtkWidget*		saveMenu;
extern GtkWidget*		saveAsMenu;
extern GtkWidget*		saveAllMenu;
extern GtkWidget*		closeMenu;
extern GtkWidget*		closeAllMenu;
extern GtkWidget*		revertMenu;
extern GtkWidget*		printMenu;
extern GtkWidget		*sortTabsMenu;
//edit menu
extern GtkWidget*		editMenu;
extern GtkWidget*		redoMenu;
extern GtkWidget*		undoMenu;
extern GtkWidget*		redoAllMenu;
extern GtkWidget*		undoAllMenu;
extern GtkWidget*		cutMenu;
extern GtkWidget*		copyMenu;
extern GtkWidget*		pasteMenu;
//nav menu
extern GtkWidget*		navMenu;
//view menu
extern GtkWidget*		goBackMenu;
extern GtkWidget*		viewMenu;
//func menu
extern GtkWidget*		funcMenu;
//bookmark menu
extern GtkWidget*		bookMarkMenu;
extern GtkWidget*		bookMarkSubMenu;
//tools menu
extern GtkWidget*		toolsMenu;
extern GtkWidget*		toolOutMenu;
//plug menu
extern GtkWidget*		globalPlugMenu;
//help menu
extern GtkWidget*		helpMenu;

//status bar message
extern char*			statusMessage;

extern HistoryClass*	history;

extern GtkAccelGroup*	accgroup;

extern GList*			newBookMarksList;
extern char*			highlightColour;
extern char*			tmpHighlightColour;
extern bool				showBMBar;
extern GtkWidget*		bmHighlightBox;
extern int				bmMarkNumber;

extern char*			toolBarLayout;
extern GtkWidget*		toolBarBox;



extern GtkWidget*		menuItemOpen;



extern GtkWidget*		lineNumberWidget;
extern GtkWidget*		findApiWidget;
extern GtkWidget*		findQtApiWidget;
extern GtkWidget*		findDefWidget;
extern GtkWidget*		liveSearchWidget;

extern int				currentPage;
//nag
extern bool				nagScreen;
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
extern GtkWidget*		prefswin;
extern bool				indent;
extern bool				lineNumbers;
extern bool				lineWrap;
extern bool				highLight;
extern bool				noSyntax;
extern bool				singleUse;
extern unsigned int		tabWidth;
extern char*			fontAndSize;
extern char*			terminalCommand;
extern char*			rootCommand;
extern unsigned int		depth;
extern bool				onExitSaveSession;
extern bool				restoreBookmarks;
extern char*			styleName;
extern bool				noDuplicates;
extern bool				noWarnings;
extern bool				readLinkFirst;
extern bool				autoShowComps;
extern bool				autoCheck;
extern bool				useGlobalPlugMenu;
extern unsigned int		maxTabChars;
extern unsigned int		autoShowMinChars;
extern unsigned int		maxBMChars;
extern unsigned int		maxFuncDefs;

extern GtkWidget*		fontButton;
extern GtkWidget*		terminalBox;
extern GtkWidget*		rootCommandBox;
extern GtkWidget*		defaultBrowserBox;
extern char*			browserCommand;

//key short cuts
extern GtkWidget*		keysWindow;

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

extern GtkWidget*		mainWindowScrollbox;

extern GtkWidget*		mainWindowVBox;
extern GtkWidget*		mainTopUserVBox;
extern GtkWidget*		mainWindowHBox;
extern GtkWidget*		mainLeftUserVBox;
extern GtkWidget*		mainNotebookVBox;
extern GtkWidget*		mainRightUserVBox;
extern GtkWidget*		mainBottomUserVBox;

extern GtkWidget*		mainWindowVPane;
extern GtkWidget*		secondWindowVPane;

extern GtkWidget*		mainWindowHPane;
extern GtkWidget*		secondWindowHPane;

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
extern unsigned int		maxFRHistory;

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
extern GtkWidget*		wordListEntrybox;
extern char*			badWord;
extern char*			goodWord;
#ifdef _ASPELL_
extern AspellConfig*	aspellConfig;
extern AspellSpeller*	spellChecker;
#endif

extern StringSlice*		globalSlice;

extern unsigned int		shortCuts[NUMSHORTCUTS][2];
extern char*			shortCutStrings[NUMSHORTCUTS];

extern GtkWidget*		progressWindow;
extern GtkWidget*		progressBar;

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
extern args				kkedit_startup_vars[];
extern args				keybindings_rc[];


extern const char*		localeLang;
extern bool				doUpdateWidgets;

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
void goBack(GtkWidget* widget,gpointer data);
void showBarberPole(const char* title);
void killBarberPole(void);
void debugFree(char** ptr,const char* message);
char* truncateWithElipses(char* str,unsigned int maxlen);
char* truncateTabNameWithElipses(char* str,unsigned int maxlen);
void doBusy(bool busy,pageStruct* page);

void setWidgets(void);
void resetWidgetSenisitive(void);

void catchSignal(int signal);
void freeAndNull(char** ptr);

#endif




