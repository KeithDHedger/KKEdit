/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#ifndef _GUIS_
#define _GUIS_

enum	{AUTOINDENT=0,SHOWNUMS,WRAP,HIGHLIGHT,NOSYNTAX,USESINGLE,AUTOSAVE,AUTOBM,NODUPLICATE,NOWARN,READLINK,AUTOSHOW,MAXPREFSWIDGETS};
extern	GtkWidget*	prefsWidgets[MAXPREFSWIDGETS];

void buildMainGui(void);
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