/*
 *
 * K.D.Hedger 2014 <kdhedger68713@gmail.com>
 *
 * kkedit-includes.h
 *
*/

#ifndef _KKEDIT_INCLUDES_
#define _KKEDIT_INCLUDES_

#include "config.h"

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <glib.h>

#include <signal.h>
#include <stdio.h>
#include <errno.h>

#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>

#ifndef _USEGTK3_
#include <gtksourceview/gtksourceiter.h>
#else
#define GTK_STOCK_NEW "document-new"
#define GTK_STOCK_OPEN "document-open"

#define GTK_STOCK_SAVE "document-save"
#define GTK_STOCK_SAVE_AS "document-save-as"
#define GTK_STOCK_CUT "edit-cut"
#define GTK_STOCK_COPY "edit-copy"
#define GTK_STOCK_PASTE "edit-paste"
#define GTK_STOCK_UNDO "edit-undo"
#define GTK_STOCK_REDO "edit-redo"
#define GTK_STOCK_FIND "edit-find"
#define GTK_STOCK_SPELL_CHECK "tools-check-spelling"
#define GTK_STOCK_CLOSE "window-close"
#define GTK_STOCK_REFRESH "view-refresh"
#define GTK_STOCK_GO_BACK "go-previous"
#define GTK_STOCK_GO_HOME "go-home"
#define GTK_STOCK_DELETE "edit-delete"
#define GTK_STOCK_DIALOG_QUESTION "dialog-question"
#define GTK_STOCK_ADD "list-add"
#define GTK_STOCK_PREFERENCES "preferences-system"
#define GTK_STOCK_PRINT "document-print"
#define GTK_STOCK_REVERT_TO_SAVED "document-revert"
#define GTK_STOCK_QUIT "application-exit"
#define GTK_STOCK_SORT_ASCENDING "view-sort-ascending"
#define GTK_STOCK_GO_DOWN "go-down"
#define GTK_STOCK_ABOUT "help-about"
#define GTK_STOCK_HELP "help-browser"
#define GTK_STOCK_GO_FORWARD "go-next"
#define GTK_STOCK_FIND_AND_REPLACE "edit-find-replace"
#define GTK_STOCK_HOME "go-home"
#define GTK_STOCK_GO_UP "go-up"

#define GTK_STOCK_YES gettext("Yes")
#define GTK_STOCK_NO gettext("No")
#define GTK_STOCK_CANCEL gettext("Cancel")
#define GTK_STOCK_SELECT_COLOR gettext("Select Colour")
#define GTK_STOCK_APPLY gettext("Apply")
#define GTK_STOCK_EDIT gettext("Edit")
#define GTK_STOCK_OK gettext("OK")

#endif

//#define GTK_STOCK_NEW3 gettext("_New")
//#define GTK_STOCK_OPEN3 gettext("Open")
//#define GTK_STOCK_SAVE3 gettext("_Save")
//#define GTK_STOCK_SAVE_AS3 gettext("Save As")
//#define GTK_STOCK_PRINT3 gettext("_Print")
//#define GTK_STOCK_CLOSE3 gettext("_Close")
//#define GTK_STOCK_REVERT_TO_SAVED3 gettext("_Revert")
#define GTK_STOCK_QUIT3 gettext("_Quit")
#define GTK_STOCK_PREFERENCES3 "_Preferences"

#define GTK_STOCK_CUT3 gettext("Cut")
#define GTK_STOCK_COPY3 gettext("Copy")
#define GTK_STOCK_PASTE3 gettext("Paste")
#define GTK_STOCK_UNDO3 gettext("Undo")
#define GTK_STOCK_REDO3 gettext("Redo")
#define GTK_STOCK_FIND3 gettext("Find")
#define GTK_STOCK_SPELL_CHECK3 gettext("Spell Check Document")
#define GTK_STOCK_REFRESH3 gettext("Refresh")
#define GTK_STOCK_GO_BACK3 gettext("Back")
#define GTK_STOCK_HOME3 gettext("Home")
#define GTK_STOCK_ABOUT3 gettext("About")
#define GTK_STOCK_HELP3 gettext("Help")
#define GTK_STOCK_GO_DOWN3 gettext("Down")
#define GTK_STOCK_GO_UP3 gettext("Up")
#define GTK_STOCK_GO_FORWARD3 gettext("Forward")

#define GTK_STOCK_OPEN_LABEL gettext("_Open")
#define GTK_STOCK_CLOSE_LABEL gettext("_Close")
#define GTK_STOCK_NEW_LABEL gettext("_New")
#define GTK_STOCK_SAVE_LABEL gettext("_Save")
#define GTK_STOCK_SAVE_AS_LABEL gettext("Save _As...")
#define GTK_STOCK_SAVE_ALL_LABEL gettext("Save A_ll")
#define GTK_STOCK_PRINT_LABEL gettext("_Print...")
#define GTK_STOCK_REVERT_TO_SAVED_LABEL gettext("_Revert")





#define GTK_STOCK_NO3 gettext("_No")
#define GTK_STOCK_CANCEL3 gettext("_Cancel")


#define PREFSNEW DATADIR"/pixmaps/new.png"
#define PREFSOPEN DATADIR"/pixmaps/open.png"
#define PREFSSAVE DATADIR"/pixmaps/save.png"
#define PREFSCUT DATADIR"/pixmaps/cut.png"
#define PREFSCOPY DATADIR"/pixmaps/copy.png"
#define PREFSPASTE DATADIR"/pixmaps/paste.png"
#define PREFSUNDO DATADIR"/pixmaps/undo.png"
#define PREFSREDO DATADIR"/pixmaps/redo.png"
#define PREFSFIND DATADIR"/pixmaps/find.png"
#define PREFSQUESTION DATADIR"/pixmaps/question.png"
#define PREFSBACK DATADIR"/pixmaps/back.png"
#define PREFSLINENUM DATADIR"/pixmaps/num.png"
#define PREFSAPI DATADIR"/pixmaps/api.png"
#define PREFSQTAPI DATADIR"/pixmaps/qtapi.png"
#define PREFSFINDDEF DATADIR"/pixmaps/finddef.png"
#define PREFSLIVE DATADIR"/pixmaps/live.png"
#define PREFSSEPERATOR DATADIR"/pixmaps/sep.png"
#define PREFSEXPAND DATADIR"/pixmaps/expand.png"

#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#include <ctype.h>
#include <gdk/gdkkeysyms.h>

#include <libgen.h>
#include <time.h>
#include <libintl.h>
#include <locale.h>

//#include "config.h"
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



