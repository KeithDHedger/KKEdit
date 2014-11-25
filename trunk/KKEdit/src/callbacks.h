/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.h
 *
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
void doShutdown(GtkWidget* widget,gpointer data);
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

#endif
