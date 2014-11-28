/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * functioncallbacks.h
 *
*/

#ifndef _FUNCCALLBACKS_
#define _FUNCCALLBACKS_

#ifdef _BUILDDOCVIEWER_
#ifndef _USEQT5_
void webKitGoBack(GtkWidget* widget,gpointer data);
#else
//TODO//
void webKitGoBack(void);
#endif
#ifndef _USEQT5_
void webKitGoForward(GtkWidget* widget,gpointer data);
#else
//TODO//
void webKitGoForward(void);
#endif
#ifndef _USEQT5_
void webKitGoHome(GtkWidget* widget,gpointer data);
#else
//TODO//
void webKitGoHome(void);
#endif
#ifndef _USEQT5_
void docSearchInPageFoward(GtkWidget* widget,gpointer data);
#else
//TODO//
void docSearchInPageFoward(void);
#endif
#ifndef _USEQT5_
void docSearchInPageBack(GtkWidget* widget,gpointer data);
#else
//TODO//
void docSearchInPageBack(void);
#endif
#endif

#ifndef _USEQT5_
void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data);
#else
//TODO//
void doFindReplace(void);
#endif

void find(Widget* widget,uPtr data);

#ifndef _USEQT5_
void doSearchPrefs(GtkWidget* widget,gpointer data);
#else
//TODO//
void doSearchPrefs(void);
#endif
#ifndef _USEQT5_
void docSearchFromBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void docSearchFromBar(void);
#endif
#ifndef _USEQT5_
void defSearchFromBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void defSearchFromBar(void);
#endif
#ifndef _USEQT5_
void doLiveSearch(GtkWidget* widget,GdkEvent *event,gpointer data);
#else
//TODO//
void doLiveSearch(void);
#endif
void basicFind(int dowhat);
void regexFind(int dowhat);

void searchQT5Docs(Widget* widget,uPtr data);

#ifndef _USEQT5_
void qt5DocSearchFromBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void qt5DocSearchFromBar(void);
#endif

void searchGtkDocs(Widget* widget,uPtr data);

void showDocView(int howtodisplay,char* text,const char* title);
#ifndef _USEQT5_
void pasteFRClip(GtkWidget* widget,gpointer data);
#else
//TODO//
void pasteFRClip(void);
#endif

void doDoxy(Widget* widget,uPtr data);
void doxyDocs(Widget* widget,uPtr data);

#endif