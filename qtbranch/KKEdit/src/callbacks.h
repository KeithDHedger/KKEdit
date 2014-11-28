/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.h
 *
*/

#ifndef _CALLBACKS_
#define _CALLBACKS_

void doOpenFile(Widget* widget,uPtr data);

#ifndef _USEQT5_
void closeTab(GtkWidget* widget,gpointer data);
#else
//TODO//
void closeTab(void);
#endif
#ifndef _USEQT5_
void switchPage(GtkNotebook *notebook,gpointer arg1,guint arg2,gpointer user_data);
#else
//TODO//
void switchPage(void);
#endif
void setSensitive(void);
#ifndef _USEQT5_
void copyToClip(GtkWidget* widget,gpointer data);
#else
//TODO//
void copyToClip(void);
#endif
#ifndef _USEQT5_
void cutToClip(GtkWidget* widget,gpointer data);
#else
//TODO//
void cutToClip(void);
#endif
#ifndef _USEQT5_
void pasteFromClip(GtkWidget* widget,gpointer data);
#else
//TODO//
void pasteFromClip(void);
#endif
#ifndef _USEQT5_
void undo(GtkWidget* widget,gpointer data);
#else
//TODO//
void undo(void);
#endif
#ifndef _USEQT5_
void redo(GtkWidget* widget,gpointer data);
#else
//TODO//
void redo(void);
#endif
#ifndef _USEQT5_
void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data);
#else
//TODO//
void dropUri(void);
#endif
#ifndef _USEQT5_
void externalTool(GtkWidget* widget,gpointer data);
#else
//TODO//
void externalTool(void);
#endif
#ifndef _USEQT5_
void openHelp(GtkWidget* widget,gpointer data);
#else
//TODO//
void openHelp(void);
#endif

void printFile(Widget* widget,uPtr data);

#ifndef _USEQT5_
void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data);
#else
//TODO//
void populatePopupMenu(void);
#endif
#ifndef _USEQT5_
bool tabPopUp(GtkWidget *my_widget, GdkEventButton *event,gpointer user_data);
#else
//TODO//
bool tabPopUp(void);
#endif
#ifndef _USEQT5_
UniqueResponse messageReceived(UniqueApp *app,UniqueCommand command,UniqueMessageData *message,guint time,gpointer user_data);
#else
//TODO//
#endif
#ifndef _USEQT5_
void doShutdown(GtkWidget* widget,gpointer data);
#else
//TODO//
void doShutdown(void);
#endif
#ifndef _USEQT5_
void setPrefs(GtkWidget* widget,gpointer data);
#else
//TODO//
void setPrefs(void);
#endif
#ifndef _USEQT5_
void setToolOptions(GtkWidget* widget,gpointer data);
#else
//TODO//
void setToolOptions(void);
#endif
#ifndef _USEQT5_
void doAbout(GtkWidget* widget,gpointer data);
#else
//TODO//
void doAbout(void);
#endif
void writeExitData(void);

bool doSaveAll(Widget* widget,uPtr data);

#ifndef _USEQT5_
void closeAllTabs(GtkWidget* widget,gpointer data);
#else
//TODO//
void closeAllTabs(void);
#endif
#ifndef _USEQT5_
void recentFileMenu(GtkRecentChooser* chooser,gpointer* data);
#else
//TODO//
void recentFileMenu(void);
#endif
void refreshMainWindow(void);
#ifndef _USEQT5_
gboolean whatPane(GtkWidget *widget,GdkEvent *event,gpointer data);
#else
//TODO//
gboolean whatPane(void);
#endif

void newEditor(Widget* widget,uPtr data);

#ifndef _USEQT5_
void changeSourceStyle(GtkWidget* widget,gpointer data);
#else
//TODO//
void changeSourceStyle(void);
#endif
int yesNo(char* question,char* file);

#ifndef _USEQT5_
void line_mark_activated (GtkSourceGutter *gutter,GtkTextIter *iter,GdkEventButton  *ev,pageStruct* page);
#else
//TODO//
void line_mark_activated (void);
#endif
#ifndef _USEQT5_
void toggleBookmark(GtkWidget*,GtkTextIter* titer);
#else
//TODO//
void toggleBookmark(void);
#endif
#ifndef _USEQT5_
void toggleBookMarkBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void toggleBookMarkBar(void);
#endif
void setToobarSensitive(void);
#ifndef _USEQT5_
void toggleToolBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void toggleToolBar(void);
#endif
#ifndef _USEQT5_
void toggleToolOutput(GtkWidget* widget,gpointer data);
#else
//TODO//
void toggleToolOutput(void);
#endif
#ifndef _USEQT5_
void removeAllBookmarks(GtkWidget* widget,GtkTextIter* titer);
#else
//TODO//
void removeAllBookmarks(void);
#endif
#ifndef _USEQT5_
void unRedoAll(GtkWidget* widget,gpointer data);
#else
//TODO//
void unRedoAll(void);
#endif
#ifndef _USEQT5_
gboolean keyShortCut(GtkWidget* window,GdkEventKey* event,gpointer data);
#else
//TODO//
gboolean keyShortCut(void);
#endif
void loadKeybindings(void);
#ifndef _USEQT5_
void toggleStatusBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void toggleStatusBar(void);
#endif
#ifndef _USEQT5_
void updateStatuBar(GtkTextBuffer* textbuffer,GtkTextIter* location,GtkTextMark* mark,gpointer data);
#else
//TODO//
void updateStatuBar(void);
#endif
#ifndef _USEQT5_
void toggleDocviewer(GtkWidget* widget,gpointer data);
#else
//TODO//
void toggleDocviewer(void);
#endif
#ifndef _USEQT5_
void closeDocViewer(GtkWidget *widget,GdkEvent *event,gpointer data);
#else
//TODO//
void closeDocViewer(void);
#endif
#ifndef _USEQT5_
void getPlugins(GtkWidget* widget,gpointer data);
#else
//TODO//
void getPlugins(void);
#endif
void showToolOutput(bool immediate);
void hideToolOutput(bool immediate);

#endif
