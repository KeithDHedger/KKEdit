/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:49:02 GMT 2015 kdhedger68713@gmail.com

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

void doOpenFile(GtkWidget* widget,gpointer data);
void closeTab(GtkWidget* widget,gpointer data);
void switchPage(GtkNotebook *notebook,gpointer arg1,guint arg2,gpointer user_data);
void setSensitive(void);
void copyToClip(GtkWidget* widget,gpointer data);
void cutToClip(GtkWidget* widget,gpointer data);
void pasteFromClip(GtkWidget* widget,gpointer data);
void undo(GtkWidget* widget,gpointer data);
void redo(GtkWidget* widget,gpointer data);
void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data);
void externalTool(GtkWidget* widget,gpointer data);
void openHelp(GtkWidget* widget,gpointer data);
void printFile(GtkWidget* widget,gpointer data);
void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data);
bool tabPopUp(GtkWidget *my_widget, GdkEventButton *event,gpointer user_data);
bool doShutdown(GtkWidget* widget,GdkEvent *event,gpointer data);
void setPrefs(GtkWidget* widget,gpointer data);
void setToolOptions(GtkWidget* widget,gpointer data);
void doAbout(GtkWidget* widget,gpointer data);
void writeExitData(void);
bool doSaveAll(GtkWidget* widget,gpointer data);
void closeAllTabs(GtkWidget* widget,gpointer data);
void recentFileMenu(GtkRecentChooser* chooser,gpointer* data);
void refreshMainWindow(void);
gboolean whatPane(GtkWidget *widget,GdkEvent *event,gpointer data);
void newEditor(GtkWidget* widget,gpointer data);
void changeSourceStyle(GtkWidget* widget,gpointer data);
int yesNo(char* question,char* file);

void line_mark_activated (GtkSourceGutter *gutter,GtkTextIter *iter,GdkEventButton  *ev,pageStruct* page);
void toggleBookmark(GtkWidget*,GtkTextIter* titer);
void toggleBookMarkBar(GtkWidget* widget,gpointer data);
void setToobarSensitive(void);
void toggleToolBar(GtkWidget* widget,gpointer data);
void toggleToolOutput(GtkWidget* widget,gpointer data);
void removeAllBookmarks(GtkWidget* widget,GtkTextIter* titer);
void unRedoAll(GtkWidget* widget,gpointer data);
gboolean keyShortCut(GtkWidget* window,GdkEventKey* event,gpointer data);
void loadKeybindings(void);
void toggleStatusBar(GtkWidget* widget,gpointer data);
void updateStatusBar(GtkTextBuffer* textbuffer,GtkTextIter* location,GtkTextMark* mark,gpointer data);
void toggleDocviewer(GtkWidget* widget,gpointer data);
void closeDocViewer(GtkWidget *widget,GdkEvent *event,gpointer data);
void getPlugins(GtkWidget* widget,gpointer data);
void showToolOutput(bool immediate);
void hideToolOutput(bool immediate);
void sortTabs(GtkWidget* widget,gpointer data);

void toggleLineNumbers(GtkWidget* widget,gpointer data);
void toggleWrapLines(GtkWidget* widget,gpointer data);
void toggleSyntax(GtkWidget* widget,gpointer data);
void toggleAutoComplete(GtkWidget* widget,gpointer data);
void toggleHighlightCurrent(GtkWidget* widget,gpointer data);


#endif
