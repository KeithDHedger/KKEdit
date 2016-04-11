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
#define GTK_STOCK_YES gettext("_Yes")
#define GTK_STOCK_NO gettext("_No")
#define GTK_STOCK_CANCEL gettext("_Cancel")
#define GTK_STOCK_SELECT_COLOR gettext("Select Colour")
#define GTK_STOCK_APPLY gettext("_Apply")
#define GTK_STOCK_EDIT gettext("_Edit")
#define GTK_STOCK_OK gettext("_OK")

#endif

//menus labels
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
#define REPLACE_ALL_LABEL gettext("Replace All")

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
#define GOTO_DEFINE_LABEL gettext("Go To _Definition")
#define FIND_IN_DOCS_LABEL gettext("_Find In Documentation")
#define FIND_IN_GTKAPI_LABEL gettext("_Search In Gtk Docs")
#define FIND_IN_QTAPI_LABEL gettext("S_earch In Qt5 Docs")

//toolbar labels
#define GOTO_DEFINE_TOOLBAR_LABEL gettext("Definition")
#define OPEN_TOOLBAR_LABEL gettext("Open")
#define CLOSE_TOOLBAR_LABEL gettext("Close")
#define NEW_TOOLBAR_LABEL gettext("New")
#define SAVE_TOOLBAR_LABEL gettext("Save")
#define UNDO_TOOLBAR_LABEL gettext("Undo")
#define REDO_TOOLBAR_LABEL gettext("Redo")
#define CUT_TOOLBAR_LABEL gettext("Cut")
#define COPY_TOOLBAR_LABEL gettext("Copy")
#define PASTE_TOOLBAR_LABEL gettext("Paste")
#define FIND_TOOLBAR_LABEL gettext("Find")
#define BACK_TOOLBAR_LABEL gettext("Back")

//context labels
#define CUSTOM_WORD_CONTEXT_LABEL gettext("_Add To Custom Word List")
#define CHECK_SPELLING_CONTEXT_LABEL gettext("C_heck Spellling")

//#define GTK_STOCK_GO_BACK "go-previous"
//#define GTK_STOCK_GO_HOME "go-home"
//#define GTK_STOCK_GO_DOWN "go-down"

//#define GTK_STOCK_HOME "go-home"
//#define GTK_STOCK_GO_UP "go-up"

//help menu
#define ABOUT_LABEL gettext("_About")
#define HELP_LABEL gettext("_Help")

//odd
#define SPELL_CHECK_LABEL gettext("Spell Check Document")
#define REFRESH_LABEL gettext("Refresh")
#define NO_LABEL gettext("_No")
#define CANCEL_LABEL gettext("_Cancel")

//kkedit general text
#define BOOKMARK_LABEL gettext("Bookmark")
#define REPLACE_ALL_LABEL gettext("Replace All")
#define REPLACE_ALL_MNEMONIC gettext("_Replace All")
#define REPLACE_LABEL gettext("_Replace")
#define WRAP_LABEL gettext("Wrap")
#define ALL_FILES_LABEL gettext("All Files")
#define HIGHLIGHT_ALL_LABEL gettext("Highlight All")
#define FIND_TEXT_LABEL gettext("Find Text")
#define REPLACE_TEXT_LABEL gettext("Replace With")

//kb shortcts
#define DELETE_LINE_LABEL gettext("Delete Current Line")
#define DELETE_TO_EOL_LABEL gettext("Delete To End Of Line")
#define DELETE_TO_BOL_LABEL gettext("Delete To Beginning Of Line")
#define SELECT_WORD_LABEL gettext("Select Word Under Cursor")
#define DELETE_WORD_LABEL gettext("Delete Word Under Cursor")
#define DUPLICATE_LINE_LABEL gettext("Duplicate Current Line")
#define SELECT_LINE_LABEL gettext("Select Current Line")
#define LINE_UP_LABEL gettext("Move Current Line Up")
#define LINE_DOWN_LABEL gettext("Move Current Line Down")
#define SELECT_TO_EOL_LABEL gettext("Select From Cursor To End Of Line")
#define SELECT_TO_BOL_LABEL gettext("Select From Beginning Of Line To Cursor")
#define SELECTION_UP_LABEL gettext("Move Selection Up")
#define SELECTION_DOWN_LABEL gettext("Move Selection Down")
#define SHOW_COMPLETION_LABEL gettext("Show Completion")

//tooltips
#define NEW_TT_LABEL gettext("New File")
#define OPEN_TT_LABEL gettext("Open File")
#define SAVE_TT_LABEL gettext("Save File")
#define RECENT_FILE_TT_LABEL gettext("Open Recent File")
#define CUT_TT_LABEL gettext("Cut")
#define COPY_TT_LABEL gettext("Copy")
#define PASTE_TT_LABEL gettext("Paste")
#define UNDO_TT_LABEL gettext("Undo")
#define REDO_TT_LABEL gettext("Redo")
#define FIND_TT_LABEL gettext("Find")
#define GOTO_DEFINE_TT_LABEL gettext("Go To Definition")
#define BACK_TT_LABEL gettext("Go Back")
#define TO_LINE_TT_LABEL gettext("Go To Line")
#define TO_API_TT_LABEL gettext("Find API In Gtk Docs")
#define TO_QTAPI_TT_LABEL gettext("Find API In Qt5 Docs")
#define TO_DEFINE_TT_LABEL gettext("Search For Define")
#define LIVE_TT_LABEL gettext("Live Search")
#define SEPERATOR_TT_LABEL gettext("Separator")
#define EXPANDER_TT_LABEL gettext("Expander")



//tools
#define TOOLS_PLACEHOLDER_LABEL gettext("PLACEHOLDERS:\n%t - Currently selected text. Passed to command as $KKEDIT_SELECTION\n%f - Filepath of the current document. Passed to command as $KKEDIT_CURRENTFILE\n%d - Directory of the current document or $HOME. Passed to command as $KKEDIT_CURRENTDIR\n%i - The location of the globally installed tools. Passed to command as $KKEDIT_DATADIR\n%h - Tempory file for displaying html in doc viewer. Passed to command as $KKEDIT_HTMLFILE\n%l - Highlihting language. Passed to command as $KKEDIT_SOURCE_LANG")
#define TOOLS_EDIT_LABEL gettext("Edit External Tools")
#define TOOLS_NAME_LABEL gettext("Tool Name:\t")
#define TOOLS_COMMAND_LABEL gettext("Command: \t")
#define TOOLS_SHORTCUT_LABEL gettext("Shortcut:\t\t")
#define TOOLS_COMMENT_LABEL gettext("Comment:  \t")
#define TOOLS_IN_TERM_LABEL gettext("Run Tool In Terminal")
#define TOOLS_IN_POPUP_LABEL gettext("Show Tool In Pop-Up Menu")
#define TOOLS_ALWAYS_IN_POPUP_LABEL gettext("Always Show Tool In Pop-Up Menu")
#define TOOLS_SYNC_LABEL gettext("Run Tool Synchronously")
#define TOOLS_SHOW_HTML_LABEL gettext("Show HTML Doc")
#define TOOLS_CLEAR_OP_LABEL gettext("Clear Tool Output First")
#define TOOLS_AS_ROOT_LABEL gettext("Run Tool As Root")
#define TOOLS_USE_BAR_LABEL gettext("Use Progress Bar")
#define TOOLS_IGNORE_OP_LABEL gettext("Ignore Output")
#define TOOLS_PASTE_OP_LABEL gettext("Paste Output")
#define TOOLS_REPLACE_CONTENTS_LABEL gettext("Replace All Contents")
#define TOOLS_VIEW_OP_LABEL gettext("View Output")
#define TOOLS_DELETE_LABEL gettext("Delete")

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



