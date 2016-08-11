/*
 *
 * ©K. D. Hedger. Fri  6 May 10:52:11 BST 2016 kdhedger68713@gmail.com

 * This file (callbacks.h) is part of KKEdit.

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

#ifndef _CALLBACKS_
#define _CALLBACKS_

enum {NEWROOTEDITOR=1,NEWEDITOR,NEWMANPAGEEDITOR};

void closeTab(GtkWidget *widget,gpointer data);
void switchPage(GtkNotebook *notebook,gpointer arg1,guint arg2,gpointer user_data);
void copyToClip(GtkWidget *widget,gpointer data);
void cutToClip(GtkWidget *widget,gpointer data);
void pasteFromClip(GtkWidget *widget,gpointer data);
void undo(GtkWidget *widget,gpointer data);
void redo(GtkWidget *widget,gpointer data);
void externalTool(GtkWidget *widget,gpointer data);
void openHelp(GtkWidget *widget,gpointer data);
void printFile(GtkWidget *widget,gpointer data);
void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data);
bool tabPopUp(GtkWidget *my_widget, GdkEventButton *event,gpointer user_data);
bool doShutdown(GtkWidget *widget,GdkEvent *event,gpointer data);
void setPrefs(GtkWidget *widget,gpointer data);
void setToolOptions(GtkWidget *widget,gpointer data);
void doAbout(GtkWidget *widget,gpointer data);
void writeExitData(void);
bool doSaveAll(GtkWidget *widget,gpointer data);
void closeAllTabs(GtkWidget *widget,gpointer data);
void refreshMainWindow(void);
void newEditor(GtkWidget *widget,gpointer data);
void changeSourceStyle(GtkWidget *widget,gpointer data);
int yesNo(char *question,char *file);

#ifdef _USEGTK3_
void line_mark_activated(GtkSourceView *view,GtkTextIter *iter,GdkEvent *event,gpointer user_data);
#else
void line_mark_activated(GtkSourceGutter *gutter,GtkTextIter *iter,GdkEventButton *ev,pageStruct *page);
#endif

void toggleToolBar(GtkWidget *widget,gpointer data);
void toggleToolOutput(GtkWidget *widget,gpointer data);
void unRedoAll(GtkWidget *widget,gpointer data);
gboolean keyShortCut(GtkWidget *window,GdkEventKey *event,gpointer data);
void loadKeybindings(void);
void toggleStatusBar(GtkWidget *widget,gpointer data);
void updateStatusBar(GtkTextBuffer *textbuffer,GtkTextIter *location,GtkTextMark *mark,gpointer data);
void toggleDocviewer(GtkWidget *widget,gpointer data);
void closeDocViewer(GtkWidget *widget,GdkEvent *event,gpointer data);
void getPlugins(GtkWidget *widget,gpointer data);
void showToolOutput(bool immediate);
void hideToolOutput(bool immediate);
void sortTabs(GtkWidget *widget,gpointer data);
void showAllTabs(GtkWidget *widget,gpointer data);
void hideTab(GtkWidget *widget,gpointer data);

void toggleLineNumbers(GtkWidget *widget,gpointer data);
void toggleWrapLines(GtkWidget *widget,gpointer data);
void toggleSyntax(GtkWidget *widget,gpointer data);
void toggleWhitespace(GtkWidget *widget,gpointer data);
void toggleAutoComplete(GtkWidget *widget,gpointer data);
void toggleHighlightCurrent(GtkWidget *widget,gpointer data);

void markUndo(GtkSourceUndoManager *manager,pageStruct *page);
void markRedo(GtkSourceUndoManager *manager,pageStruct *page);
void markDirty(GtkTextBuffer *textbuffer,pageStruct *page);
void realCloseTab(GtkNotebook *notebook,GtkWidget *child,guint page_num,gpointer user_data);
void doTabMenu(GtkWidget *widget,gpointer user_data);

#endif
