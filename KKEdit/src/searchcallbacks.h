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
void webKitGoBack(GtkWidget* widget,gpointer data);
void webKitGoForward(GtkWidget* widget,gpointer data);
void webKitGoHome(GtkWidget* widget,gpointer data);
void docSearchInPageFoward(GtkWidget* widget,gpointer data);
void docSearchInPageBack(GtkWidget* widget,gpointer data);
#endif

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data);
void find(GtkWidget* widget,gpointer data);
void doSearchPrefs(GtkWidget* widget,gpointer data);
void docSearchFromBar(GtkWidget* widget,gpointer data);
void defSearchFromBar(GtkWidget* widget,gpointer data);
void doLiveSearch(GtkWidget* widget,GdkEvent *event,gpointer data);
void basicFind(int dowhat);
void regexFind(int dowhat);
void searchQT5Docs(GtkWidget* widget,gpointer data);
void qt5DocSearchFromBar(GtkWidget* widget,gpointer data);
void seachGtkDocs(GtkWidget* widget,gpointer data);
void showDocView(int howtodisplay,char* text,const char* title);
void pasteFRClip(GtkWidget* widget,gpointer data);
void doDoxy(GtkWidget* widget,long data);
void doxyDocs(GtkWidget* widget,gpointer data);

#endif