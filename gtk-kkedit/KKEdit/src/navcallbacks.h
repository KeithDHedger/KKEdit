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
void goToDefinition(GtkWidget* widget,gpointer data);
void findFile(GtkWidget* widget,gpointer data);
void jumpToLine(GtkWidget* widget,gpointer data);
void gotoLine(GtkWidget* widget,gpointer data);
void jumpToLineFromBar(GtkWidget* widget,gpointer data);
void functionSearch(GtkWidget* widget,gpointer data);
void gotoLine(GtkWidget* widget,gpointer data);
void jumpToMark(GtkWidget* widget,gpointer glist);
void rebuildBookMarkMenu(void);
#ifdef _BUILDDOCVIEWER_
gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigation_action,WebKitWebPolicyDecision* policy_decision, gpointer user_data);
#endif
void rebuildTabsMenu(void);
//v//oid selectTab(GtkWidget* widget,gpointer data);
void gotoLineSavePos(GtkWidget* widget,gpointer data);

#endif