/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * functioncallbacks.h
 *
*/

#ifndef _FUNCCALLBACKS_
#define _FUNCCALLBACKS_
#if 0
#ifdef _BUILDDOCVIEWER_
void webKitGoBack(Widget* widget,gpointer data);
void webKitGoForward(Widget* widget,gpointer data);
void webKitGoHome(Widget* widget,gpointer data);
void docSearchInPageFoward(Widget* widget,gpointer data);
void docSearchInPageBack(Widget* widget,gpointer data);
#endif

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data);
void find(Widget* widget,gpointer data);
void doSearchPrefs(Widget* widget,gpointer data);
void docSearchFromBar(Widget* widget,gpointer data);
void defSearchFromBar(Widget* widget,gpointer data);
void doLiveSearch(Widget* widget,GdkEvent *event,gpointer data);
void basicFind(int dowhat);
void regexFind(int dowhat);
void searchQT5Docs(Widget* widget,gpointer data);
void qt5DocSearchFromBar(Widget* widget,gpointer data);
void searchGtkDocs(Widget* widget,gpointer data);
void showDocView(int howtodisplay,char* text,const char* title);
void pasteFRClip(Widget* widget,gpointer data);
void doDoxy(Widget* widget,long data);
void doxyDocs(Widget* widget,gpointer data);

#endif
#endif
