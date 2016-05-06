/*
 *
 * ©K. D. Hedger. Fri  6 May 10:50:42 BST 2016 kdhedger68713@gmail.com

 * This file (navcallbacks.h) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NAVCALLBACKS_
#define _NAVCALLBACKS_

void goToDefine(functionData *fdata);
void goToDefinition(GtkWidget *widget,gpointer data);
void findFile(GtkWidget *widget,gpointer data);
void jumpToLine(GtkWidget *widget,gpointer data);
void gotoLine(GtkWidget *widget,gpointer data);
void jumpToLineFromBar(GtkWidget *widget,gpointer data);
void functionSearch(GtkWidget *widget,gpointer data);
void gotoLine(GtkWidget *widget,gpointer data);
void jumpToMark(GtkWidget *widget,gpointer glist);
void rebuildBookMarkMenu(void);
#ifdef _BUILDDOCVIEWER_
gboolean docLinkTrap(WebKitWebView *web_view,WebKitWebFrame *frame,WebKitNetworkRequest *request,WebKitWebNavigationAction *navigation_action,WebKitWebPolicyDecision *policy_decision, gpointer user_data);
#endif
void rebuildTabsMenu(void);
void gotoLineSavePos(GtkWidget *widget,gpointer data);

#endif