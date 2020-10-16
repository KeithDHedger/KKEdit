/*
 *
 * ©K. D. Hedger. Fri  6 May 10:51:35 BST 2016 kdhedger68713@gmail.com

 * This file (globals.h) is part of KKEdit.

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

#include "kkedit-includes.h"

#ifndef _GLOBALS_
#define _GLOBALS_

#define DBG0 0 //no debug
#define DBG1 1 //debug to stderr
#define DBG2 2 //debug to file
#define DBG3 3 //debug to both
#define DBG4 4 //only catch faults
#define DBG5 5 //trace progflow
#define DBG6 6 //trace and timing

#define REPLACENEXT		100
#define REPLACEPREV		200
#define FINDNEXT		400
#define FINDPREV		800

#if _DEBUGLEVEL_ == DBG0
#define ERRDATA
#define _ENTER_
#define _LEAVE_
#else
#if _DEBUGLEVEL_ == DBG5
#define ERRDATA errLine=__LINE__,errFile=__FILE__,errFunc=__func__;printf("File: %s,Func: %s,Line: %i\n",errFile,errFunc,errLine);
#else
#define ERRDATA errLine=__LINE__,errFile=__FILE__,errFunc=__func__;
#define _ENTER_
#define _LEAVE_
#endif
#if _DEBUGLEVEL_ == DBG6
#define _ENTER_ struct timeval then;struct timeval now;gettimeofday(&then,NULL);printf("Entered %s\n",__func__);
#define _LEAVE_ gettimeofday(&now,NULL);double diff=(double)((((now.tv_sec-then.tv_sec)*1000000L+now.tv_usec)-then.tv_usec))/1000000L;printf("Time spent in function=%.3f\n",diff);
#endif
#endif

//#define PLUGINSLINK "https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins#getplugs"
#define PLUGINSLINK "https://github.com/KeithDHedger/KKEditPlugins"

#define TOOL_ASYNC 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2
#define TOOL_SHOW_DOC 4
#define	TOOL_INSERT_MASK 0b10111
#define	TOOL_VIEW_OP 16

#define VALIDFILENAMECHARS "[A-Za-z0-9_-./ <>]"
#define VALIDFUNCTIONCHARS "[A-Za-z0-9_*:/@()-,.#;[\"]"

#define MAXRECENT 10
#define MAXTEXTWIDTH 500
#define MAXBFHISTORY 100

#define MAXSAVEPERIOD	100

#define MARK_TYPE_1		"bookmark"
#define MARK_TYPE_2		"user1"
#define MARK_TYPE_3		"user2"
#define MARK_TYPE_4		"user3"
#define MARK_TYPE_5		"user4"

#define USEURI			-1
#define USEFILE			-2

#define PAGESTARTID		1000

#define	POLEPATH PREFIX "/bin/KKEditProgressBar" _EXECSUFFIX_
#define	APPEXECNAME "kkedit" _EXECSUFFIX_
#define APPFOLDENAME ".KKEdit" _EXECSUFFIX_
#define CUSTOMWORDFILE APPFOLDENAME "/customcompletions"
#define DBUSNAME "org.keithhedge." APPEXECNAME
#define ABOUTICON "KKEdit" _EXECSUFFIX_
#define NUMSHORTCUTS	15

#ifndef _ENUMS_
#define _ENUMS_
enum {PIXBUF_COLUMN,TEXT_COLUMN,BUTTON_NUM};
enum {NEWVBOX=0,NEWHBOX};
enum {NOERR=0,NOOPENFILE,NOSAVEFILE};
enum {NAVLAST=0,NAVNEXT};
enum {FUNCDATA=0};
#endif

enum {FIRSTCHAR=0,CURRENTCHAR,LASTCHAR};

//extern const char	*moreapps[];
extern const char	*authorskk[];

struct toolStruct
{
	char				*menuName;
	char				*filePath;
	char				*command;
	int					flags;
	bool				inTerminal;					
	bool				inPopUp;
	bool				alwaysPopup;
	bool				clearView;
	char				*comment;
	bool				global;
	bool				runAsRoot;
	int					keyCode;
	bool				useBar;
};

struct functionData
{
	char				*name;
	char				*type;
	int					line;
	char				*file;
	char				*define;
	int					intab;
};

struct varStrings
{
	char				*name;
	char				*data;
};

extern int				errno;
extern int				errLine;
extern const char		*errFile;
extern const char		*errFunc;
extern char				*logFile;

//app
extern GApplication	*mainApp;
extern bool				autoSeleced;
extern bool				fromGOpen;
extern char				*prefsFolder;
extern GtkRecentManager	*recentMan;
extern int				sessionID;
extern bool				safeMode;
extern bool				overRideLoadSession;

//main window
extern GtkWidget		*mainWindow;
extern GtkWidget		*menuBar;
extern GtkToolbar		*toolBar;
extern GtkWidget		*statusBarMenu;
extern GtkNotebook		*mainNotebook;
extern  int				switchPageHandler;

//file menu
extern GtkWidget		*fileMenu;
extern GtkWidget		*menuItemNew;
extern GtkWidget		*menuItemOpen;
extern GtkWidget		*buildDocsMenu;
extern GtkWidget		*saveMenu;
extern GtkWidget		*saveAsMenu;
extern GtkWidget		*saveAllMenu;
extern GtkWidget		*closeMenu;
extern GtkWidget		*closeAllMenu;
extern GtkWidget		*revertMenu;
extern GtkWidget		*printMenu;

//edit menu
extern GtkWidget		*editMenu;
extern GtkWidget		*redoMenu;
extern GtkWidget		*undoMenu;
extern GtkWidget		*redoAllMenu;
extern GtkWidget		*undoAllMenu;
extern GtkWidget		*cutMenu;
extern GtkWidget		*copyMenu;
extern GtkWidget		*pasteMenu;
extern GtkWidget		*sortTabsMenu;
extern GtkWidget		*showAllTabsMenu;

//nav menu
extern GtkWidget		*navMenu;
extern GtkWidget		*gotoDefMenu;
extern GtkWidget		*searchInGtkDocsMenu;
extern GtkWidget		*searchInQTDocsMenu;
extern GtkWidget		*searchInDocsMenu;
extern GtkWidget		*goBackMenu;
extern GtkWidget		*goForwardMenu;
extern GtkWidget		*continueMenu;

//view menu
extern GtkWidget		*viewMenu;
extern GtkWidget		*viewTabMenu;
extern GtkWidget		*viewTabSubMenu;
//func menu
extern GtkWidget		*funcMenu;
//bookmark menu
extern GtkWidget		*bookMarkMenu;
extern GtkWidget		*bookMarkSubMenu;
//tools menu
extern GtkWidget		*toolsMenu;
extern GtkWidget		*toolOutMenu;
//plug menu
extern GtkWidget		*globalPlugMenu;
//help menu
extern GtkWidget		*helpMenu;

extern GtkAccelGroup	*accgroup;

extern GList			*newBookMarksList;
extern char				*highlightColour;
extern char				*tmpHighlightColour;
extern bool				showBMBar;
extern GtkWidget		*bmHighlightBox;
extern int				bmMarkNumber;
extern unsigned			pageID;

extern char				*toolBarLayout;
extern GtkWidget		*toolBarBox;

extern GtkWidget		*lineNumberWidget;
extern GtkWidget		*findApiWidget;
extern GtkWidget		*findQtApiWidget;
extern GtkWidget		*findDefWidget;
extern GtkWidget		*liveSearchWidget;

extern int				currentPage;
extern int				sinkReturn;
extern char				*sinkReturnStr;

//nag
extern bool				nagScreen;

//docview size and position
extern int				docWindowWidth;
extern int				docWindowHeight;
extern int				docWindowX;
extern int				docWindowY;

//prefs
extern GtkWidget		*prefswin;
extern bool				indent;
extern bool				lineNumbers;
extern bool				lineWrap;
extern bool				highLight;
extern bool				noSyntax;
extern bool				showWhiteSpace;
extern bool				singleUse;
extern unsigned int		tabWidth;
extern unsigned int		tabsSize;
extern char				*fontAndSize;
extern char				*terminalCommand;
extern char				*rootCommand;
extern unsigned int		depth;
extern bool				onExitSaveSession;
extern char				*styleName;
extern bool				noDuplicates;
extern bool				noWarnings;
extern bool				readLinkFirst;
extern bool				autoShowComps;
extern bool				autoSearchDocs;
extern bool				useGlobalPlugMenu;
extern unsigned int		maxTabChars;
extern unsigned int		autoShowMinChars;
extern unsigned int		maxBMChars;
extern unsigned int		maxJumpHistory;
extern unsigned int		maxFuncDefs;
extern unsigned int		autoSavePeriod;
extern int				autoSaveSource;

extern GtkWidget		*fontButton;
extern GtkWidget		*terminalBox;
extern GtkWidget		*rootCommandBox;
extern GtkWidget		*defaultBrowserBox;
extern char				*browserCommand;
extern bool				showMenuIcons;

#ifdef _USEGTK3_
extern GtkStyleProvider	*provider;
extern GtkStyleProvider	*tabBoxProvider;
#endif

//key short cuts
extern GtkWidget		*keysWindow;

extern char				*tmpStyleName;

extern GtkWidget		*toolNameWidget;
extern GtkWidget		*commandLineWidget;
extern GtkWidget		*keyWidget;
extern GtkWidget		*commentWidget;
extern GtkWidget		*inTermWidget;
extern GtkWidget		*inPopupWidget;
extern GtkWidget		*alwaysPopupWidget;

extern GtkWidget		*syncWidget;
extern GtkWidget		*ignoreWidget;
extern GtkWidget		*pasteWidget;
extern GtkWidget		*replaceWidget;
extern GtkWidget		*outputWidget;
extern GtkWidget		*showDocWidget;
extern GtkWidget		*toolSelect;
extern GtkWidget		*clearViewWidget;
extern GtkWidget		*runAsRootWidget;
extern GtkWidget		*useBarWidget;

extern toolStruct		*selectedToolFromDrop;

//view tool output
extern GtkWidget		*mainVPane;
extern bool				showToolOutWin;
extern GtkWidget		*toolOutputView;
extern GtkTextBuffer	*toolOutputBuffer;
extern GtkWidget		*toolOutVBox;
extern bool				showStatus;
extern GtkWidget		*statusWidget;
extern unsigned			statusID;

extern GList			*toolsList;

extern GtkWidget		*restoreBMs;

//main window
extern int				windowWidth;
extern int				windowHeight;
extern int				windowX;
extern int				windowY;
extern int				toolOutHeight;
extern int				bottomVPaneHite;
extern int				topVPaneHite;

extern GtkWidget		*mainWindowScrollbox;

extern GtkWidget		*mainWindowVBox;
extern GtkWidget		*mainTopUserVBox;
extern GtkWidget		*mainWindowHBox;
extern GtkWidget		*mainLeftUserVBox;
extern GtkWidget		*mainNotebookVBox;
extern GtkWidget		*mainRightUserVBox;
extern GtkWidget		*mainBottomUserVBox;

extern GtkWidget		*mainWindowVPane;
extern GtkWidget		*secondWindowVPane;

extern GtkWidget		*mainWindowHPane;
extern GtkWidget		*secondWindowHPane;

extern int				currentTabNumber;
extern int 				untitledNumber;

extern GtkToolItem		*newButton;
extern GtkToolItem		*openButton;
extern GtkToolItem		*saveButton;
extern GtkToolItem		*redoButton;
extern GtkToolItem		*undoButton;
extern GtkToolItem		*cutButton;
extern GtkToolItem		*copyButton;
extern GtkToolItem		*pasteButton;
extern GtkToolItem		*findButton;
extern GtkToolItem		*gotoDefButton;
extern GtkToolItem		*backButton;
extern GtkToolItem		*forwardButton;
extern GtkToolItem		*gotoLineButton;
extern GtkToolItem		*findApiButton;
extern GtkToolItem		*findQtApiButton;
extern GtkToolItem		*findFuncDefButton;
extern GtkToolItem		*liveSearchButton;

//back/forward history

//find replace
extern GtkWidget		*findReplaceDialog;
extern GtkWidget		*findBox;
extern GtkWidget		*replaceBox;
extern GtkWidget		*findDropBox;
extern GtkWidget		*replaceDropBox;
extern GSList			*findList;
extern GSList			*replaceList;
extern unsigned int		maxFRHistory;
extern bool				wrapSearch;
extern bool				searchBack;
extern bool				insensitiveSearch;
extern bool				useRegex;
extern bool				replaceAll;
extern bool				findInAllFiles;
extern bool				hightlightAll;

//custom toolbar
extern GtkWidget		*fromHBox;
extern GtkWidget		*iconViewBox;
extern bool				showToolBar;

extern int				listFunction;
extern GtkWidget		*funcListDrop;

//misc
extern char				*functionSearchText;
extern char				*thePage;
extern char				*htmlFile;
extern char				*htmlURI;
extern char				*tmpFolderName;
extern char				*thisSessionPath;
extern char				*thisSessionName;

//runtime deps
extern int				gotManEditor;
extern int				gotDoxygen;
extern char				*ctagsPath;

extern GtkSourceStyleSchemeManager	*schemeManager;
extern GtkSourceStyleScheme	*styleScheme;

#ifdef _BUILDDOCVIEWER_
extern GtkWidget		*docView;
extern WebKitWebView	*webView;
extern bool				showHideDocviewer;
extern GtkWidget		*showDocViewWidget;
#endif
extern bool				inWindow;

extern GtkWidget		*spellCheckWord;
extern GtkWidget		*wordListDropbox;
extern GtkWidget		*wordListEntrybox;
extern char				*badWord;
extern char				*goodWord;
#ifdef _ASPELL_
extern AspellConfig		*aspellConfig;
extern AspellSpeller	*spellChecker;
#endif

extern unsigned int		shortCuts[NUMSHORTCUTS][2];
extern char				*shortCutStrings[NUMSHORTCUTS];

extern GtkWidget		*progressWindow;
extern GtkWidget		*progressBar;

#include "pluginclass.h"

extern PluginClass		*globalPlugins;

//save and load var lists
extern int				intermarg;
extern int				flagsarg;
extern int				inpopup;
extern int				alwayspopup;
extern int				clearview;
extern char				*commandarg;
extern char				*commentarg;
extern char				*menuname;
extern int				rootarg;
extern int				keycode;
extern int				usebar;

extern char				*windowAllocData;
extern char				*docWindowAllocData;
extern args				kkedit_window_rc[];
extern args				kkedit_rc[];
extern args				tool_vars[];
extern args				kkedit_startup_vars[];
extern args				keybindings_rc[];

extern const char		*localeLang;

//notebook
extern int				openInThisTab;

//general app
extern bool				loadingSession;
extern HistoryClass		*globalHistory;
extern StringSlice		*globalSlice;
extern bool				doGoogleSearch;

void plugRunFunction(gpointer data,gpointer funcname);

void getMimeType(char *filepath,void *ptr);
void setLanguage(pageStruct *page);
void runCommand(char *commandtorun,void *ptr,bool interm,int flags,int useroot,char *title);

functionData *getFunctionByName(char *name,bool recurse,bool casesensitive);
functionData *getFunctionByNameOpenFiles(char *name,bool casesensitive,bool wholefname);

void destroyData(functionData *fdata);
void getRecursiveTagList(char *filepath,void *ptr);
void buildToolsList(void);
void getRecursiveTagListFileName(char *filepath,void *ptr);
void scrollToIterInPane(pageStruct *page,GtkTextIter *iter);
void navigateHistory(GtkWidget *widget,gpointer data);
void showBarberPole(const char *title);
void killBarberPole(void);
void setBarberPoleMessage(const char *msg);
void debugFree(char** ptr);
char *truncateWithElipses(char *str,unsigned int maxlen);
char *truncateTabNameWithElipses(char *str,unsigned int maxlen);

void catchSignal(int signal);
void freeAndNull(char** ptr);

GtkWidget *createNewBox(int orient,bool homog,int spacing);
GtkToolItem *createNewToolItem(const char *stock,const char *label);
GtkWidget *createNewStockButton(const char *stock,const char *label);
GdkPixbuf *createNewIcon(const char *stock,GtkWidget *widg);
GtkWidget *createNewStockMenuItem(const char *stock,const char *label);

void setToobarWidgetsSensitive(pageStruct *page);
void setChangedSensitive(GtkTextBuffer *textbuffer,pageStruct *page);
void resetSensitive(void);
void setPageSensitive(void);
void setDocViewSensitive(void);
void getOldConfigs(const char *file,args *dataptr);
void infoMessage(const char *message);
int gerrorMessage(GError **err);

#ifdef _USEGTK3_
void applyCSS (GtkWidget *widget, GtkStyleProvider *widgprovider);
void setGtk3NBTabSize(void);
#endif

functionData* allocFStrings(unsigned what,char *string);
functionData* initFunctionData(void);
varStrings* allocVStrings(char *string);

int getCurrentCursorPos(void);
int getCharacterPos(unsigned what,GtkTextBuffer *buf);

char* oneLiner(const char *command);

#endif




