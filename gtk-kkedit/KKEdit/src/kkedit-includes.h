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

//documents
#define GTK_STOCK_NEW "document-new"
#define GTK_STOCK_OPEN "document-open"
#define GTK_STOCK_SAVE "document-save"
#define GTK_STOCK_SAVE_AS "document-save-as"
#define GTK_STOCK_REVERT_TO_SAVED "document-revert"
#define GTK_STOCK_PRINT "document-print"

//edit
#define GTK_STOCK_CUT "edit-cut"
#define GTK_STOCK_COPY "edit-copy"
#define GTK_STOCK_PASTE "edit-paste"
#define GTK_STOCK_UNDO "edit-undo"
#define GTK_STOCK_REDO "edit-redo"
#define GTK_STOCK_FIND "edit-find"
#define GTK_STOCK_DELETE "edit-delete"
#define GTK_STOCK_FIND_AND_REPLACE "edit-find-replace"

//applications
#define GTK_STOCK_CLOSE "window-close"
#define GTK_STOCK_PREFERENCES "preferences-system"
#define GTK_STOCK_QUIT "application-exit"
#define GTK_STOCK_ABOUT "help-about"
#define GTK_STOCK_HELP "help-browser"

//odds
#define GTK_STOCK_REFRESH "view-refresh"
#define GTK_STOCK_SPELL_CHECK "tools-check-spelling"
#define GTK_STOCK_DIALOG_QUESTION "dialog-question"
#define GTK_STOCK_ADD "list-add"
#define GTK_STOCK_SORT_ASCENDING "view-sort-ascending"

//navigation
#define GTK_STOCK_GO_BACK "go-previous"
#define GTK_STOCK_GO_HOME "go-home"
#define GTK_STOCK_GO_DOWN "go-down"
#define GTK_STOCK_GO_FORWARD "go-next"
#define GTK_STOCK_HOME "go-home"
#define GTK_STOCK_GO_UP "go-up"

//gtk stock widget labels
#define GTK_STOCK_YES gettext("Yes")
#define GTK_STOCK_NO gettext("No")
#define GTK_STOCK_CANCEL gettext("Cancel")
#define GTK_STOCK_SELECT_COLOR gettext("Select Colour")
#define GTK_STOCK_APPLY gettext("Apply")
#define GTK_STOCK_EDIT gettext("Edit")
#define GTK_STOCK_OK gettext("OK")

#endif
//labels
//file menu
#define OPEN_LABEL gettext("_Open")
#define CLOSE_LABEL gettext("_Close")
#define NEW_LABEL gettext("_New")
#define SAVE_LABEL gettext("_Save")
#define SAVE_AS_LABEL gettext("Save _As...")
#define SAVE_ALL_LABEL gettext("Save A_ll")
#define PRINT_LABEL gettext("_Print...")
#define REVERT_TO_SAVED_LABEL gettext("_Revert")
#define QUIT_LABEL gettext("_Quit")

//edit menu
#define UNDO_LABEL gettext("_Undo")
#define REDO_LABEL gettext("_Redo")
#define CUT_LABEL gettext("Cu_t")
#define COPY_LABEL gettext("_Copy")
#define PASTE_LABEL gettext("_Paste")
#define FIND_LABEL gettext("_Find")
#define PREFERENCES_LABEL gettext("Pr_eferences")

//nav menu
#define GO_BACK_LABEL gettext("_Back")
#define GOTO_LINE_LABEL gettext("_Go To Line")

//help menu
#define ABOUT_LABEL gettext("_About")
#define HELP_LABEL gettext("_Help")

//odd
#define SPELL_CHECK_LABEL gettext("Spell Check Document")
#define REFRESH_LABEL gettext("Refresh")
#define NO_LABEL gettext("_No")
#define CANCEL_LABEL gettext("_Cancel")

//docviewer
#define HOME_LABEL gettext("_Home")
#define GO_DOWN_LABEL gettext("_Down")
#define GO_UP_LABEL gettext("_Up")
#define GO_FORWARD_LABEL gettext("For_ward")






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



