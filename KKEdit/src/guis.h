/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#ifndef _GUIS_
#define _GUIS_

void buildMainGui(void);
void buildTools(void);
void buildFindReplace(void);
int showFunctionEntry(void);
//void liveGtkDocSearch(GtkWidget* widget,gpointer data);
#ifdef BUILDDOCVIEWER
void buildGtkDocViewer(void);
#endif

#endif