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
void webKitGoBack(Widget* widget,uPtr data);
void webKitGoForward(Widget* widget,uPtr data);
void webKitGoHome(Widget* widget,uPtr data);
void docSearchInPageFoward(Widget* widget,uPtr data);
void docSearchInPageBack(Widget* widget,uPtr data);
#endif

#ifndef _USEQT5_
void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data);
#else
//TODO//
void doFindReplace(void);
#endif

void find(Widget* widget,uPtr data);

void doSearchPrefs(Widget* widget,uPtr data);
void docSearchFromBar(Widget* widget,uPtr data);
void defSearchFromBar(Widget* widget,uPtr data);

#ifndef _USEQT5_
void doLiveSearch(GtkWidget* widget,GdkEvent *event,gpointer data);
#else
//TODO//
void doLiveSearch(void);
#endif
void basicFind(int dowhat);
void regexFind(int dowhat);

void searchQT5Docs(Widget* widget,uPtr data);
void qt5DocSearchFromBar(Widget* widget,uPtr data);
void searchGtkDocs(Widget* widget,uPtr data);

void showDocView(int howtodisplay,char* text,const char* title);
void pasteFRClip(Widget* widget,uPtr data);
void doDoxy(Widget* widget,uPtr data);
void doxyDocs(Widget* widget,uPtr data);

#endif