/*
 *
 * ©K. D. Hedger. Fri  6 May 10:50:29 BST 2016 kdhedger68713@gmail.com

 * This file (searchcallbacks.h) is part of KKEdit.

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

#ifndef _FUNCCALLBACKS_
#define _FUNCCALLBACKS_

#ifdef _BUILDDOCVIEWER_
void webKitGoBack(GtkWidget *widget,gpointer data);
void webKitGoForward(GtkWidget *widget,gpointer data);
void webKitGoHome(GtkWidget *widget,gpointer data);
void docSearchInPageFoward(GtkWidget *widget,gpointer data);
void docSearchInPageBack(GtkWidget *widget,gpointer data);
#endif

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data);
void find(GtkWidget *widget,gpointer data);
void doSearchPrefs(GtkWidget *widget,gpointer data);
void docSearchFromBar(GtkWidget *widget,gpointer data);
bool defSearchFromBar(GtkWidget *widget,gpointer data);
void doLiveSearch(GtkWidget *widget,GdkEvent *event,gpointer data);
void basicFind(int dowhat);
void regexFind(int dowhat);
bool searchQT5Docs(GtkWidget *widget,gpointer data);
void searchQT5Assist(GtkWidget *widget,gpointer data);
void qt5DocSearchFromBar(GtkWidget *widget,gpointer data);
bool searchGtkDocs(GtkWidget *widget,gpointer data);
void showDocView(int howtodisplay,char *text,const char *title);
void pasteFRClip(GtkWidget *widget,gpointer data);
void doDoxy(GtkWidget *widget,long data);
void doxyDocs(GtkWidget *widget,gpointer data);

#endif