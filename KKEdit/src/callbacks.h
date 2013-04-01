/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 * callbacks.h
 *
*/

#include <gtk/gtk.h>

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

#endif
