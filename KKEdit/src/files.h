/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * files.h
 *
*/
#ifndef _FILES_
#define _FILES_

void saveVarsToFile(char* filepath,args* dataptr);
void loadVarsFromFile(char* filepath,args* dataptr);
bool openFile(const gchar *filepath,int linenumber,bool warn);
bool saveFile(GtkWidget* widget,gpointer data);
void newFile(GtkWidget* widget,gpointer data);
void resetAllFilePrefs(void);
void openAsHexDump(GtkWidget *widget,gpointer user_data);
void reloadFile(GtkWidget* widget,gpointer data);
void saveSession(GtkWidget* widget,gpointer data);
void restoreSession(GtkWidget* widget,gpointer data);
void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data);
void setFilePrefs(pageStruct* page);



void doCompletionPopUp(pageStruct* page);

#endif
