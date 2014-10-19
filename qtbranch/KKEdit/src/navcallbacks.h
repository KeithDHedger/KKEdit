/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * navcallbacks.h
 *
*/

#ifndef _NAVCALLBACKS_
#define _NAVCALLBACKS_

void goToDefine(functionData* fdata);
#ifndef _USEQT5_
void goToDefinition(GtkWidget* widget,gpointer data);
#else
//TODO//
void goToDefinition(void);
#endif
#ifndef _USEQT5_
void findFile(GtkWidget* widget,gpointer data);
#else
//TODO//
void findFile(void);
#endif
#ifndef _USEQT5_
void jumpToLine(GtkWidget* widget,gpointer data);
#else
//TODO//
void jumpToLine(void);
#endif
#ifndef _USEQT5_
void gotoLine(GtkWidget* widget,gpointer data);
#else
//TODO//
void gotoLine(void);
#endif
#ifndef _USEQT5_
void jumpToLineFromBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void jumpToLineFromBar(void);
#endif
#ifndef _USEQT5_
void functionSearch(GtkWidget* widget,gpointer data);
#else
//TODO//
void functionSearch(void);
#endif
#ifndef _USEQT5_
void gotoLine(GtkWidget* widget,gpointer data);
#else
//TODO//
void gotoLine(void);
#endif
#ifndef _USEQT5_
void jumpToMark(GtkWidget* widget,gpointer glist);
#else
//TODO//
void jumpToMark(void);
#endif
void rebuildBookMarkMenu(void);
#ifdef _BUILDDOCVIEWER_
gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigation_action,WebKitWebPolicyDecision* policy_decision, gpointer user_data);
#endif

#endif