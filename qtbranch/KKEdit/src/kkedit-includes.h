/*
 *
 * K.D.Hedger 2014 <kdhedger68713@gmail.com>
 *
 * kkedit-includes.h
 *
*/

#ifndef _KKEDIT_INCLUDES_
#define _KKEDIT_INCLUDES_

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <glib.h>
#include <ctype.h>

#include "config.h"

#ifndef _USEQT5_
#include <gtk/gtk.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourceiter.h>

#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#include <gdk/gdkkeysyms.h>
#include <unique/unique.h>
#else
	#include <QtWidgets>
#endif

#include <libgen.h>
#include <time.h>
#include <libintl.h>
#include <locale.h>

#include "sliceclass.h"
#include "kkedit-plugins.h"

#include "textbuffer.h"
#include "backclass.h"

#ifdef _ASPELL_
#include <aspell.h>
#endif

#ifdef _BUILDDOCVIEWER_
#include <webkit/webkit.h>
#endif

#include "globals.h"
#include "navcallbacks.h"

#include "plugins.h"
#include "callbacks.h"
#include "guis.h"
#include "searchcallbacks.h"
#include "spellcheck.h"
#include "files.h"
#include "encoding.h"
#include "completion.h"

#define VISIBLE __attribute__((visibility("default")))
#define PROTECTED __attribute__((visibility("protected")))

#endif



