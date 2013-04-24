/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * functioncallbacks.h
 *
*/

#include <gtk/gtk.h>
#include <unique/unique.h>

#include "globals.h"
#include "files.h"

#ifndef _FUNCCALLBACKS_
#define _FUNCCALLBACKS_

#ifdef BUILDDOCVIEWER
void webKitGoBack(GtkWidget* widget,gpointer data);
void webKitGoForward(GtkWidget* widget,gpointer data);
#endif
void showDocView(GtkWidget* widget,gpointer data);

#endif