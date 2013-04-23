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

extern int	theLineNum;

void gtkDocSearch(GtkWidget* widget,gpointer data);
void goToDefine(functionData* fdata);
void goToDefinition(GtkWidget* widget,gpointer data);
void findFile(GtkWidget* widget,gpointer data);
void jumpToLine(GtkWidget* widget,gpointer data);
void gotoLine(GtkWidget* widget,gpointer data);

#endif