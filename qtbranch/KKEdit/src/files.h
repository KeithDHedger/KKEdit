/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * files.h
 *
*/

#include "config.h"
 
#ifndef _FILES_
#define _FILES_

void saveVarsToFile(char* filepath,args* dataptr);
void loadVarsFromFile(char* filepath,args* dataptr);
bool openFile(const gchar *filepath,int linenumber,bool warn);
bool saveFile(Widget* widget,gpointer data);
#ifndef _USEQT5_
VISIBLE void newFile(Widget* widget,gpointer data);
#else
VISIBLE void newFile(void);
#endif

void resetAllFilePrefs(void);
void openAsHexDump(Widget *widget,gpointer user_data);
void reloadFile(Widget* widget,gpointer data);
void saveSession(Widget* widget,gpointer data);
void restoreSession(Widget* widget,gpointer data);
//void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data);
void setFilePrefs(pageStruct* page);


#endif
