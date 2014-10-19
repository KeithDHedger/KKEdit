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
#ifndef _USEQT5_
bool saveFile(GtkWidget* widget,gpointer data);
#else
//TODO//
bool saveFile(void);
#endif
#ifndef _USEQT5_
void newFile(GtkWidget* widget,gpointer data);
#else
//TODO//
void newFile(void);
#endif
void resetAllFilePrefs(void);
#ifndef _USEQT5_
void openAsHexDump(GtkWidget *widget,gpointer user_data);
#else
//TODO//
void openAsHexDump(void);
#endif
#ifndef _USEQT5_
void reloadFile(GtkWidget* widget,gpointer data);
#else
//TODO//
void reloadFile(void);
#endif
#ifndef _USEQT5_
void saveSession(GtkWidget* widget,gpointer data);
#else
//TODO//
void saveSession(void);
#endif
#ifndef _USEQT5_
void restoreSession(GtkWidget* widget,gpointer data);
#else
//TODO//
void restoreSession(void);
#endif
#ifndef _USEQT5_
void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data);
#else
//TODO//
void fileChangedOnDisk(void);
#endif
void setFilePrefs(pageStruct* page);

#endif
