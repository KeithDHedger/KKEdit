/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * navcallbacks.h
 *
*/

#include <gtk/gtk.h>
#include <unique/unique.h>

#include "globals.h"
#include "files.h"

#ifndef _NAVCALLBACKS_
#define _NAVCALLBACKS_

void goToDefine(functionData* fdata);
void goToDefinition(GtkWidget* widget,gpointer data);
void findFile(GtkWidget* widget,gpointer data);
void jumpToLine(GtkWidget* widget,gpointer data);
void gotoLine(GtkWidget* widget,gpointer data);
void jumpToLineFromBar(GtkWidget* widget,gpointer data);
void functionSearch(GtkWidget* widget,gpointer data);
void addBookmark(GtkWidget* widget,gpointer data);
void gotoLine(GtkWidget* widget,gpointer data);


void addBookmarkNew(GtkWidget* widget,gpointer data);


#endif