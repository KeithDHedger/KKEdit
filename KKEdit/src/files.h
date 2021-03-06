/*
 *
 * ©K. D. Hedger. Fri  6 May 10:49:00 BST 2016 kdhedger68713@gmail.com

 * This file (files.h) is part of KKEdit.

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

#ifndef _FILES_
#define _FILES_

void saveVarsToFile(char *filepath,args *dataptr);
int loadVarsFromFile(char *filepath,args *dataptr);
bool openFile(const gchar *filepath,int linenumber,bool warn);
bool saveFile(GtkWidget *widget,gpointer data);
void newFile(GtkWidget *widget,gpointer data);
void resetAllFilePrefs(void);
void openAsHexDump(GtkWidget *widget,gpointer user_data);
void reloadFile(GtkWidget *widget,gpointer data);
void saveSession(const char *filename,const char *path);
void restoreSession(GtkWidget *widget,gpointer data);
void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data);
void setFilePrefs(pageStruct *page);
void doOpenFile(GtkWidget *widget,gpointer data);
void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data);
void recentFileMenu(GtkRecentChooser *chooser,gpointer *data);
void openFromTab(GtkMenuItem *widget,pageStruct *page);

#endif
