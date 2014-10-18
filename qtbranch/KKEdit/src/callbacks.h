/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.h
 *
*/

#ifndef _CALLBACKS_
#define _CALLBACKS_
#if 1
void doOpenFile(Widget* widget,gpointer data);
void closeTab(Widget* widget,gpointer data);

#ifndef _USEQT5_
void switchPage(GtkNotebook *notebook,gpointer arg1,guint arg2,gpointer user_data);
#else
//TODO
void switchPage(void);
#endif

void setSensitive(void);
void copyToClip(Widget* widget,gpointer data);
void cutToClip(Widget* widget,gpointer data);
void pasteFromClip(Widget* widget,gpointer data);
void undo(Widget* widget,gpointer data);
void redo(Widget* widget,gpointer data);
//void dropUri(Widget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data);
void externalTool(Widget* widget,gpointer data);
void openHelp(Widget* widget,gpointer data);
void printFile(Widget* widget,gpointer data);
void populatePopupMenu(Widget *entry,Widget *menu,gpointer user_data);
//bool tabPopUp(Widget *my_widget, GdkEventButton *event,gpointer user_data);

#ifdef _USEQT5_
bool tabPopUp(void);
//TODO//
#else
bool tabPopUp(GtkWidget *widget, GdkEventButton *event,gpointer user_data);
#endif

//UniqueResponse messageReceived(UniqueApp *app,UniqueCommand command,UniqueMessageData *message,guint time,gpointer user_data);
void doShutdown(Widget* widget,gpointer data);
void setPrefs(Widget* widget,gpointer data);
void setToolOptions(Widget* widget,gpointer data);
void doAbout(Widget* widget,gpointer data);
void writeExitData(void);
bool doSaveAll(Widget* widget,gpointer data);
void closeAllTabs(Widget* widget,gpointer data);
//void recentFileMenu(GtkRecentChooser* chooser,gpointer* data);
void refreshMainWindow(void);
//gboolean whatPane(Widget *widget,GdkEvent *event,gpointer data);
void newEditor(Widget* widget,gpointer data);
void changeSourceStyle(Widget* widget,gpointer data);
int yesNo(char* question,char* file);

//void line_mark_activated (GtkSourceGutter *gutter,GtkTextIter *iter,GdkEventButton  *ev,pageStruct* page);
//void toggleBookmark(Widget*,GtkTextIter* titer);
void toggleBookMarkBar(Widget* widget,gpointer data);
void setToobarSensitive(void);
void toggleToolBar(Widget* widget,gpointer data);
void toggleToolOutput(Widget* widget,gpointer data);
//void removeAllBookmarks(Widget* widget,GtkTextIter* titer);
void unRedoAll(Widget* widget,gpointer data);
//gboolean keyShortCut(Widget* window,GdkEventKey* event,gpointer data);
void loadKeybindings(void);
void toggleStatusBar(Widget* widget,gpointer data);
//void updateStatuBar(GtkTextBuffer* textbuffer,GtkTextIter* location,GtkTextMark* mark,gpointer data);
void toggleDocviewer(Widget* widget,gpointer data);
//void closeDocViewer(Widget *widget,GdkEvent *event,gpointer data);
void getPlugins(Widget* widget,gpointer data);
void showToolOutput(bool immediate);
void hideToolOutput(bool immediate);

#endif
#endif
