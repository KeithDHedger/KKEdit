/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#ifndef _GUIS_
#define _GUIS_

enum	{AUTOINDENT=0,SHOWNUMS,WRAP,HIGHLIGHT,NOSYNTAX,USESINGLE,AUTOSAVE,AUTOBM,NODUPLICATE,NOWARN,READLINK,AUTOSHOW,UPDATECHECK,BEKIND,GLOBALPLUGMENU,MAXPREFSWIDGETS};
enum {MAXTABCHARS=0,MAXHISTORY,MAXFUNCDEPTH,COMPLETIONSIZE,TABWIDTH,MENUWIDTH,MAXBMWIDTH,MAXPREFSINTWIDGETS};

#ifndef _USEQT5_
extern	GtkWidget*	prefsWidgets[MAXPREFSWIDGETS];
extern	GtkObject*	prefsIntWidgets[MAXPREFSINTWIDGETS];
#else
extern	QWidget*	prefsWidgets[MAXPREFSWIDGETS];
extern	QWidget*	prefsIntWidgets[MAXPREFSINTWIDGETS];
#endif

enum {STOCKMENU=0,IMAGEMENU,PIXMAPMENU,NORMALMENU};

void buildMainGui(void);
void buildMainGuiQT(void);
void buildTools(void);
void buildFindReplace(void);
void buildWordCheck(int documentCheck);
int showFunctionEntry(void);
#ifdef _BUILDDOCVIEWER_
void buildGtkDocViewer(void);
#endif
void setUpToolBar(void);
char* makeToolBarList(void);
#endif