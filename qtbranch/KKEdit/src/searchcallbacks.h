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
#ifndef _USEQT5_
void find(GtkWidget* widget,gpointer data);
#else
//TODO//
void find(void);
#endif
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
#ifndef _USEQT5_
void searchQT5Docs(GtkWidget* widget,gpointer data);
#else
//TODO//
void searchQT5Docs(void);
#endif
#ifndef _USEQT5_
void qt5DocSearchFromBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void qt5DocSearchFromBar(void);
#endif
#ifndef _USEQT5_
void searchGtkDocs(GtkWidget* widget,gpointer data);
#else
//TODO//
void searchGtkDocs(void);
#endif
void showDocView(int howtodisplay,char* text,const char* title);
#ifndef _USEQT5_
void pasteFRClip(GtkWidget* widget,gpointer data);
#else
//TODO//
void pasteFRClip(void);
#endif
#ifndef _USEQT5_
void doDoxy(GtkWidget* widget,long data);
#else
//TODO//
void doDoxy(void);
#endif
#ifndef _USEQT5_
void doxyDocs(GtkWidget* widget,gpointer data);
#else
//TODO//
void doxyDocs(void);
#endif

#endif