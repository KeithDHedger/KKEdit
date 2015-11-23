/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#ifndef _GUIS_
#define _GUIS_

enum	{AUTOINDENT=0,SHOWNUMS,WRAP,HIGHLIGHT,NOSYNTAX,USESINGLE,AUTOSAVE,AUTOBM,NODUPLICATE,NOWARN,READLINK,AUTOSHOW,UPDATECHECK,BEKIND,GLOBALPLUGMENU,MAXPREFSWIDGETS};
enum {MAXTABCHARS=0,MAXHISTORY,MAXFUNCDEPTH,COMPLETIONSIZE,TABWIDTH,MENUWIDTH,MAXBMWIDTH,MAXPREFSINTWIDGETS};
enum {FUNCTIONCOMBO=0,THEMECOMBO,FONTNAMECOMBO,FONTSIZECOMBO,BMHIGHLIGHTCOLOUR,PREFSTERMCOMMAND,PREFSROOTCOMMAND,PREFSBROWSERCOMMAND,MAXPREFSOTHERWIDGETS};

#ifndef _USEQT5_
extern	GtkWidget*	prefsWidgets[MAXPREFSWIDGETS];
extern	GtkObject*	prefsIntWidgets[MAXPREFSINTWIDGETS];
#else
extern	QWidget		*prefsWidgets[MAXPREFSWIDGETS];
extern	QWidget		*prefsIntWidgets[MAXPREFSINTWIDGETS];
extern	QWidget		*prefsOtherWidgets[MAXPREFSOTHERWIDGETS];
extern	QListWidget	*listWidget;
#endif

enum {STOCKMENU=0,IMAGEMENU,PIXMAPMENU,NORMALMENU};

void buildMainGui(void);
void buildMainGuiQT(void);
void buildTools(void);
void buildFindReplace(void);
void buildWordCheck(int documentCheck);
int showFunctionEntry(void);

#ifdef _USEQT5_
void buildDocViewer(void);
#endif

#ifdef _BUILDDOCVIEWER_
void buildGtkDocViewer(void);
#endif

void setUpToolBar(void);
char* makeToolBarList(void);
#ifdef _USEQT5_
void cancelPrefs(void);
#endif

typedef void (*menuCallbackVoid)(Widget* widget,long data);
typedef bool (*menuCallbackBool)(Widget* widget,long data);

#ifndef _USEQT5_
GtkWidget*	makeMenuItem(const char* stocklabel,GtkWidget* parent,void* function,char hotkey,const char* name,int setimage,const char* menulabel,void* userdata);
#else
QAction* makeMenuItem(Widget* menu,const char* name,const QKeySequence key,const char* iconname,const char* widgetname,menuCallbackVoid ptrvoid,menuCallbackBool ptrbool,int data);
#endif

#endif