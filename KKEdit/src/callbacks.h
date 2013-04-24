/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.h
 *
*/

#include <gtk/gtk.h>
#include <unique/unique.h>

#include "globals.h"
#include "files.h"

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
void find(GtkWidget* widget,gpointer data);
void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data);
int show_question(char* filename);
void externalTool(GtkWidget* widget,gpointer data);
void addBookmark(GtkWidget* widget,gpointer data);
void removeBookmark(GtkWidget* widget,gpointer data);
void openHelp(GtkWidget* widget,gpointer data);
void reloadFile(GtkWidget* widget,gpointer data);
void doSearchPrefs(GtkWidget* widget,gpointer data);
void saveSession(GtkWidget* widget,gpointer data);
void restoreSession(GtkWidget* widget,gpointer data);
void printFile(GtkWidget* widget,gpointer data);
void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data);
void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data);
void tabPopUp(GtkWidget *my_widget, GdkEventButton *event,gpointer user_data);
void functionSearch(GtkWidget* widget,gpointer data);
void openAsHexDump(GtkWidget *widget,gpointer user_data);
UniqueResponse messageReceived(UniqueApp *app,UniqueCommand command,UniqueMessageData *message,guint time,gpointer user_data);
void doShutdown(GtkWidget* widget,gpointer data);
void setPrefs(GtkWidget* widget,gpointer data);
void setToolOptions(GtkWidget* widget,gpointer data);
void doAbout(GtkWidget* widget,gpointer data);
void writeExitData(void);

//void doWebKit(GtkWidget* widget,gpointer data);

#endif
