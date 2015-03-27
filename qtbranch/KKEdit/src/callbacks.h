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
void closeTab(Widget* widget,uPtr data);
#ifdef _USEQT5_
void closeTabQT(int tabnum);
#endif

#ifndef _USEQT5_
void switchPage(GtkNotebook *notebook,gpointer arg1,guint arg2,gpointer user_data);
#else
//TODO//
void switchPage(int thispage);
#endif
void setSensitive(void);

void copyToClip(Widget* widget,uPtr data);
void cutToClip(Widget* widget,uPtr data);
void pasteFromClip(Widget* widget,uPtr data);

void undo(Widget* widget,uPtr data);
void redo(Widget* widget,uPtr data);

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

void openHelp(Widget* widget,uPtr data);
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

void doShutdown(Widget* widget,uPtr data);

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

void doAbout(Widget* widget,uPtr data);

void writeExitData(void);

bool doSaveAll(Widget* widget,uPtr data);
void closeAllTabs(Widget* widget,uPtr data);

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

//view callbacks
void toggleBookMarkBar(Widget* widget,uPtr data);
void setToobarSensitive(void);
void toggleToolBar(Widget* widget,uPtr data);
void toggleToolOutput(Widget* widget,uPtr data);
void toggleStatusBar(Widget* widget,uPtr data);
#ifdef _BUILDDOCVIEWER_
void toggleDocviewer(Widget* widget,uPtr data);
#endif

#ifndef _USEQT5_
void removeAllBookmarks(GtkWidget* widget,GtkTextIter* titer);
#else
//TODO//
void removeAllBookmarks(void);
#endif

void unRedoAll(Widget* widget,uPtr data);

#ifndef _USEQT5_
gboolean keyShortCut(GtkWidget* window,GdkEventKey* event,gpointer data);
#else
//TODO//
gboolean keyShortCut(void);
#endif
void loadKeybindings(void);


#ifndef _USEQT5_
void updateStatusBar(GtkTextBuffer* textbuffer,GtkTextIter* location,GtkTextMark* mark,gpointer data);
#else
//TODO//
void updateStatusBar(void);
#endif


#ifndef _USEQT5_
void closeDocViewer(GtkWidget *widget,GdkEvent *event,gpointer data);
#else
//TODO//
void closeDocViewer(void);
#endif

void getPlugins(Widget* widget,uPtr data);

void showToolOutput(bool immediate);
void hideToolOutput(bool immediate);
void sortTabs(Widget* widget,uPtr data);

#endif
