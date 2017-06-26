/*
 *
 * ©K. D. Hedger. Fri  6 May 10:51:24 BST 2016 kdhedger68713@gmail.com

 * This file (kkedit-includes.h) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _KKEDIT_INCLUDES_
#define _KKEDIT_INCLUDES_

#include "config.h"

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>


#include <signal.h>
#include <stdio.h>
#include <errno.h>

#include <glib.h>
#ifndef _USEGTK3_
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourceiter.h>
#else
#include <gtksourceview/gtksource.h>

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
#define GTK_STOCK_REMOVE "list-remove"
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
#define GTK_STOCK_DIALOG_INFO "dialog-information"
//navigation
#define GTK_STOCK_GO_BACK "go-previous"
#define GTK_STOCK_GO_HOME "go-home"
#define GTK_STOCK_GO_DOWN "go-down"
#define GTK_STOCK_GO_FORWARD "go-next"
#define GTK_STOCK_HOME "go-home"
#define GTK_STOCK_GO_UP "go-up"
#define GTK_STOCK_EXECUTE "system-run"
#define GTK_STOCK_MEDIA_PLAY "media-playback-start"

//gtk stock widget labels
#define GTK_STOCK_YES gettext("_Yes")
#define GTK_STOCK_NO gettext("_No")
#define GTK_STOCK_CANCEL gettext("_Cancel")
#define GTK_STOCK_OPEN_LABEL gettext("_Open")
#define GTK_STOCK_APPLY gettext("_Apply")
#define GTK_STOCK_EDIT "accessories-text-editor"
#define GTK_STOCK_OK gettext("_OK")

#endif

//button labels
#define CLOSE_LABEL gettext("_Close")

//menus labels
//file menu
#define MENU_FILE_MENU_LABEL gettext("_File")
#define MENU_OPEN_LABEL gettext("_Open")
#define MENU_OPEN_AS_HEXDUMP_LABEL gettext("Open As He_xdump")
#define MENU_NEW_LABEL gettext("_New")
#define MENU_NEW_ADMIN_LABEL gettext("New A_dmin Editor")
#define MENU_NEW_EDITOR_LABEL gettext("New _Editor")
#define MENU_MANPAGE_EDITOR_LABEL gettext("_Manpage Editor")
#define MENU_BUILD_DOCS_LABEL gettext("B_uild Documentation")
#define MENU_RECENT_FILES_LABEL gettext("_Recent Files")
#define MENU_SAVE_LABEL gettext("_Save")
#define MENU_SAVE_AS_LABEL gettext("Save _As...")
#define MENU_SAVE_ALL_LABEL gettext("Save A_ll")
#define MENU_SAVE_SESSION_LABEL gettext("Sa_ve Session")
#define MENU_RESTORE_SESSION_LABEL gettext("Res_tore Session")
#define MENU_PRINT_LABEL gettext("_Print...")
#define MENU_CLOSE_LABEL gettext("_Close")
#define MENU_CLOSE_ALL_LABEL gettext("Close All Ta_bs")
#define MENU_REVERT_TO_SAVED_LABEL gettext("Re_vert")
#define MENU_QUIT_LABEL gettext("_Quit")

//edit menu
#define MENU_EDIT_MENU_LABEL gettext("_Edit")
#define MENU_UNDO_LABEL gettext("_Undo")
#define MENU_REDO_LABEL gettext("_Redo")
#define MENU_UNDO_ALL_LABEL gettext("U_ndo All")
#define MENU_REDO_ALL_LABEL gettext("Re_do All")
#define MENU_CUT_LABEL gettext("Cu_t")
#define MENU_COPY_LABEL gettext("_Copy")
#define MENU_PASTE_LABEL gettext("_Paste")
#define MENU_FIND_LABEL gettext("_Find")
#define MENU_FIND_NEXT_LABEL gettext("Find Ne_xt")
#define MENU_SORT_TABS_LABEL gettext("_Sort Tabs")
#define MENU_SHOW_ALL_TABS_LABEL gettext("S_how All Tabs")
#define MENU_HIDE_TAB_LABEL gettext("_Hide Tab")
#define MENU_LOCK_TAB_LABEL gettext("_Lock Contents")
#define MENU_UNLOCK_TAB_LABEL gettext("_Unlock Contents")
#define MENU_SELECT_TAB_LABEL gettext("Se_lect Tab")
#define MENU_PREFERENCES_LABEL gettext("Pr_eferences")
#define MENU_PLUG_PREFS_LABEL gettext("Plu_gin Prefs")

//view menu
#define MENU_VIEW_MENU_LABEL gettext("_View")
#define MENU_SHOW_DOCS_LABEL gettext("Show Documentation")
#define MENU_HIDE_BM_BAR_LABEL gettext("Hide Bookmarks Bar")
#define MENU_SHOW_BM_BAR_LABEL gettext("Show Bookmarks Bar")
#define MENU_HIDE_TOOLBAR_LABEL gettext("Hide Tool Bar")
#define MENU_SHOW_TOOLBAR_LABEL gettext("Show Tool Bar")
#define MENU_HIDE_TOOL_OP_LABEL gettext("Hide Tool Output")
#define MENU_SHOW_TOOL_OP_LABEL gettext("Show Tool Output")
#define MENU_HIDE_STATUS_LABEL gettext("Hide Status Bar")
#define MENU_SHOW_STATUS_LABEL gettext("Show Status Bar")
#define MENU_SHOW_DOCVIEWER_LABEL gettext("Show Docviewer")
#define MENU_HIDE_DOCVIEWER_LABEL gettext("Hide Docviewer")
#define MENU_SHOW_LINE_NUMBERS_LABEL gettext("Show Line Numbers")
#define MENU_WRAP_LINES_LABEL gettext("Wrap Lines")
#define MENU_HIGHLIGHT_LINE_LABEL gettext("Hightlght Current Line")
#define MENU_NO_SYNTAX_LABEL gettext("No Syntax Highlighting")
#define MENU_SHOW_SPACES_LABEL gettext("Show Whitespace")
#define MENU_SHOW_AUTO_COMPLETE gettext("Auto Show Completions")

//nav menu
#define MENU_NAV_MENU_LABEL gettext("_Navigation")
#define MENU_GOTO_DEFINE_LABEL gettext("Go To _Definition")
#define MENU_OPEN_INCLUDE_LABEL gettext("Open Include File")
#define MENU_GOTO_LINE_LABEL gettext("_Go To Line")
#define MENU_FIND_DEFINE_LABEL gettext("Search For Define")
#define MENU_FIND_IN_GTKAPI_LABEL gettext("_Search In Gtk Docs")
#define MENU_FIND_IN_QTAPI_LABEL gettext("S_earch In Qt5 Docs")
#define MENU_FIND_IN_DOCS_LABEL gettext("_Find In Documentation")
#define MENU_GO_BACK_LABEL gettext("_Back")
#define MENU_GO_FORWARD_LABEL DIALOG_GO_FORWARD_LABEL
#define MENU_CONTINUE_LABEL gettext("_Continue")

//func menu
#define MENU_FUNC_MENU_LABEL gettext("Fun_ctions")

//bookmarks menu
#define MENU_BM_MENU_LABEL gettext("_Bookmarks")
#define MENU_BM_TOGGLE_BM_LABEL gettext("_Toggle Bookmark")
#define MENU_BM_REMOVE_BMS gettext("Remove All Bookmarks")

//tools menu
#define MENU_TOOLS_MENU_LABEL gettext("_Tools")
#define MENU_EXT_TOOLS_LABEL gettext("Manage External Tools")

//plugins menu
#define MENU_GET_PLUGS_MENU_LABEL gettext("Plu_gins")

//help menu
#define MENU_HELP_MENU_LABEL gettext("_Help")
#define MENU_ABOUT_LABEL gettext("_About")
#define MENU_HELP_LABEL gettext("_Help")
#define MENU_HELP_ONLINE_LABEL gettext("_Online Help")
#define MENU_GET_PLUGS_LABEL gettext("_Get Plugins")

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
#define FORWARD_TOOLBAR_LABEL gettext("Forward")

//context labels
#define CUSTOM_WORD_CONTEXT_LABEL gettext("_Add To Custom Word List")
#define CHECK_SPELLING_CONTEXT_LABEL gettext("C_heck Spellling")
#define COPY_FOLDER_PATH_LABEL gettext("Copy Folder Path")
#define COPY_FILE_PATH_LABEL gettext("Copy Filepath")
#define COPY_FILE_NAME_LABEL gettext("Copy FileName")
#define SOURCE_HILITE_LABEL gettext("Source Highlight")

//odd
#define SPELL_CHECK_LABEL gettext("Spell Check Document")
#define REFRESH_LABEL gettext("Refresh")
#define NO_LABEL gettext("_No")
#define CANCEL_LABEL gettext("_Cancel")

//kkedit general text
#define BOOKMARK_LABEL gettext("Bookmark")
#define CHECKING_LABEL gettext("Checking for updates ...")
#define KKEDIT_UPDATE_AVAILABLE_LABEL gettext("KKEdit update available to")
#define UPDATE_FROM_VERS_LABEL gettext("from")
#define FROM_HERE_LABEL gettext("From here")
#define PLUGIN_UPDATES_LABEL gettext("Plugin updates are available from here")
#define UPDATES_AVAILABLE_LABEL gettext("Updates Available")
#define REPLACE_INFO_LABEL gettext("Replaced %i instances of '%s' with '%s'")
#define SAVE_BEFORE_CLOSE_LABEL gettext("Save file %s before closing?")
#define UNSAVED_DATA_LABEL gettext("Warning unsaved data!")
#define STATUS_LINE_LABEL gettext("Line %i Column %i \t\tSyntax Highlighting %s\t\tFilePath %s")
#define UNTITLED_LABEL gettext("Untitled")
#define CANT_SAVE_LABEL gettext("Can't save file '%s' :(")

//kb shortcts
#define KBSC_DEFINE_KB_LABEL gettext("Define Keyboard Shortcuts")
#define KBSC_INFO_LABEL gettext("To set a custom shortcut:\nClick in the appropriate box and press CONTROL(and optionally SHIFT ) plus your custom key.\nJust press 'Delete' to remove the shortcut\nClick 'Apply' to keep changes or 'Cancel' to discard any changes.")
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
#define HIDE_TAB_LABEL gettext("Hide Tab")

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
#define BACK_MENU_TT_LABEL gettext("Back To")
#define FORWARD_TT_LABEL gettext("Go Forward")
#define FORWARD_MENU_TT_LABEL gettext("Forward To")
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
#define TOOLS_SHORT_LABEL gettext("Shortcut:\t\t")
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

//prefs dialog
#define PREFS_CUSTOMIZE_LABEL gettext("<b>Customize Tool Bar</b>")
#define PREFS_AUTO_INDENT_LABEL gettext("Auto Indent Lines")
#define PREFS_SHOWLINE_NUMBERS_LABEL gettext("Show Line Numbers")
#define PREFS_WRAP_LINES_LABEL gettext("Wrap Lines")
#define PREFS_HIGHLIGHT_LINE_LABEL gettext("Highlight Current Line")
#define PREFS_NO_SYNTAX_LABEL gettext("No Syntax Highlighting")
#define PREFS_SHOW_WHITESPACE_LABEL gettext("Show Whitespace")
#define PREFS_USE_SINGLE_LABEL gettext("Use Single Instance")
#define PREFS_AUTO_MENU_SAVE_LABEL gettext("Auto Save/Restore Session")
#define PREFS_NO_DUPLICATE_LABEL gettext("Don't Open Duplicate File")
#define PREFS_NO_WARN_LABEL gettext("Don't Warn On File Change")
#define PREFS_READ_LINK_LABEL gettext("Read Link Before Opening File")
#define PREFS_AUTO_COMPLETE_LABEL gettext("Auto Show Completions")
#define PREFS_SHOW_MENU_ICONS_LABEL gettext("Show Icons In Gtk3 Menus ( Requires Restart )")
#define PREFS_AUTO_SEARCH_LABEL gettext("Auto Search Docs")
#define PREFS_DOCVIEWER_IN_WINDOW gettext("Show Docviewer In A Window")
#define PREFS_GEN_APPEARANCE_LABEL gettext("General Appearance")
#define PREFS_TAB_WIDTH_LABEL gettext("Tab width:")
#define PREFS_THEME_LABEL gettext("Theme:")
#define PREFS_FONT_LABEL gettext("Font:")
#define PREFS_BM_COLOUR_LABEL gettext("Bookmark Highlight Colour:")
#define PREFS_COMPLETION_SIZE_LABEL gettext("Completion Minimum Word Size:")
#define PREFS_NBTABS_SIZE_LABEL gettext("Notebook Tab Size:")
#define PREFS_FUNC_TYPE_ALPHA_LABEL gettext("Display functions etc in menu by type and alphabetically")
#define PREFS_FUNC_TYPE_FILE_LABEL gettext("Display functions etc in menu by type and file position")
#define PREFS_FUNC_FILE_LABEL gettext("Display functions etc in menu by file position")
#define PREFS_FUNC_ALPHA_LABEL gettext("Display functions etc in menu alphabetically")
#define PREFS_FUNC_CAT_LABEL gettext("Display functions etc in menu in categorised format")
#define PREFS_CUSTOM_KBSC_LABEL gettext("Customize Keyboard Shortcuts")
#define PREFS_TEXT_STYLE_LABEL gettext("Text Style")
#define PREFS_TAG_SEARCH_DEPTH_LABEL gettext("Tag File Search Depth:")
#define PREFS_TERM_COMMAND_LABEL gettext("Terminal Command:")
#define PREFS_AS_ROOT_COMMAND_LABEL gettext("Run As Root Command:")
#define PREFS_BROWSER_LABEL gettext("Default Browser:")
#define PREFS_MAX_FIND_HISTORY_LABEL gettext("Max Find/Replace History:")
#define PREFS_MAX_CHARS_IN_TAB_LABEL gettext("Max Characters In Tab:")
#define PREFS_MAX_CHARS_IN_FUNC_DEFS_LABEL gettext("Max Characters In Function Defs:")
#define PREFS_MAX_CHARS_IN_BMS_LABEL gettext("Max Characters In Bookmarks:")
#define PREFS_MAX_JUMP_HISTORY_LABEL gettext("Max Back/Forward History:")
#define PREFS_UPDATE_CHECK_LABEL gettext("Check For Updates")
#define PREFS_GLOBAL_PLUG_MENU_LABEL gettext("Use Global Plugins Menu( Requires Restart )")
#define PREFS_ADMIN_LABEL gettext("Administration")
#define PREFS_BE_KIND_LABEL gettext("<b>Be Kind To Poor Programmers</b>")
#define PREFS_I_DONATED_LABEL gettext("I have donated")
#define PREFS_KARMA_LABEL gettext("I have really donated some some money to the author.\nMy conscience is clear and my Karma is squeaky clean :)")

//find dialog
#define FIND_FIND_REPLACE_LABEL gettext("Find/Replace")
#define FIND_FIND_TEXT_LABEL gettext("Find Text")
#define FIND_REPLACE_TEXT_LABEL gettext("Replace With")
#define FIND_INSENSITIVE_LABEL gettext("Case insensitive")
#define FIND_USE_REGEX_LABEL gettext("Use Regex")
#define FIND_WRAP_LABEL gettext("Wrap")
#define FIND_BACK_LABEL gettext("Backwards")
#define FIND_ALL_FILES_LABEL gettext("All Files")
#define FIND_HIGHLIGHT_ALL_LABEL gettext("Highlight All")
#define FIND_REPLACE_ALL_LABEL gettext("Replace All")
#define FIND_REPLACE_LABEL gettext("_Replace")
#define FIND_REPLACE_ALL_MNEMONIC gettext("_Replace All")

//aspell
#define SPELL_CHECK_WORD_LABEL gettext("Spell check word")
#define SPELL_CHANGE_TO_LABEL gettext("Change <i><b>%s</b></i> to: ")
#define SPELL_IGNORE_LABEL gettext("Ignore")
#define SPELL_ADD_LABEL gettext("_Add")

//misc dialogs
#define DIALOG_ENTER_FUNC_NAME_LABEL gettext("Enter Function Name")
#define DIALOG_FIND_FUNC_LABEL gettext("Find Function")
#define DIALOG_GO_FORWARD_LABEL gettext("For_ward")
#define DIALOG_PLEASE_DONATE_LABEL gettext("Please donate")
#define DIALOG_PAYPAL_LABEL gettext("If you have a PayPal account you can donate any amount you like by logging into yor account and click the 'Send Money' tab, enter my email address")
#define DIALOG_SEND_IT_LABEL gettext("and then send it.")
#define DIALOG_THANKS_LABEL gettext("Thank you for helping to support Free software.")
#define DIALOG_GOTO_LINE_LABEL gettext("Go To Line")
#define DIALOG_ENTER_LINE_LABEL gettext("Enter Line Number")
#define DIALOG_POLE_BUILDING_LABEL gettext("Building Documentaion ...")
#define DIALOG_YESNO_REPLACE_IN_ALL_FILES gettext("Do you want to replace in ALL open files?")
#define DIALOG_YESNO_WHAT_LABEL gettext("What Do You Want To Do?")
#define DIALOG_YESNO_CONFIRM_DELETE_LABEL gettext("Are you sure you want to delete")
#define DIALOG_ABOUT_MORE_LABEL gettext("\nMore by the same author\n") 
#define DIALOG_ABOUT_KKEDIT_LABEL gettext("KKEdit Code Text Editor")
#define DIALOG_ABOUT_FRENCH_LABEL gettext("French Translation")
#define DIALOG_ABOUT_ARTISTS_LABEL gettext("GTK3 Application Icon")
#define DIALOG_POLE_RESTORING gettext("Restoring Session ...")
#define DIALOG_FILE_CHANGED_LABEL gettext("File %s Has Changed on disk\nDo you want to reload it?")
#define DIALOG_WARN_CHANGED_LABEL gettext("Warning file changed!")
#define DIALOG_NO_FILE_LABEL gettext("File '%s' doesn't exist :(")
#define DIALOG_CANT_OPEN_LABEL gettext("Can't open file '%s' :(")
#define DIALOG_YESNO_NON_TEXT_LABEL gettext("Contains non text data,continue loading?\n")

//docviewer
#define DOCVIEW_DOCVIEWER_LABEL gettext("Doc Viewer")
#define DOCVIEW_HOME_LABEL gettext("_Home")
#define DOCVIEW_GO_DOWN_LABEL gettext("_Down")
#define DOCVIEW_GO_UP_LABEL gettext("_Up")
#define DOCVIEW_GTK_DOCS_LABEL gettext("Gtk Docs")
#define DOCVIEW_DOXY_DOCS_LABEL gettext("Doxygen Docs")
#define DOCVIEW_QT5_DOCS_LABEL gettext("Qt5 Docs")
#define DOCVIEW_KKEDIT_HELP_LABEL gettext("KKEdit Help")
#define DOCVIEW_SEARCHING_GOOGLE_LABEL gettext("Searching Google...")

//plug prefs
#define PLUGS_PLUG_WARN_UNLOAD_LABEL gettext("Plugin '%s' cannot be unloaded yet.\nRestart KKEdit to unload.")
#define PLUGS_ENABLE_LABEL gettext("Enable")
#define PLUGS_PLUGIN_LABEL gettext("Plug In")

//completions
#define COMPS_FUNCS_LABEL gettext("Functions")
#define COMPS_VARS_LABEL gettext("Variables")
#define COMPS_WORDS_LABEL gettext("Custom Words")

//pixmaps
#define PREFSNEW DATADIR _EXECSUFFIX_ "/pixmaps/new.png"
#define PREFSOPEN DATADIR _EXECSUFFIX_ "/pixmaps/open.png"
#define PREFSSAVE DATADIR _EXECSUFFIX_ "/pixmaps/save.png"
#define PREFSCUT DATADIR _EXECSUFFIX_ "/pixmaps/cut.png"
#define PREFSCOPY DATADIR _EXECSUFFIX_ "/pixmaps/copy.png"
#define PREFSPASTE DATADIR _EXECSUFFIX_ "/pixmaps/paste.png"
#define PREFSUNDO DATADIR _EXECSUFFIX_ "/pixmaps/undo.png"
#define PREFSREDO DATADIR _EXECSUFFIX_ "/pixmaps/redo.png"
#define PREFSFIND DATADIR _EXECSUFFIX_ "/pixmaps/find.png"
#define PREFSQUESTION DATADIR _EXECSUFFIX_ "/pixmaps/question.png"
#define PREFSBACK DATADIR _EXECSUFFIX_ "/pixmaps/back.png"
#define PREFSFORWARD DATADIR _EXECSUFFIX_ "/pixmaps/forward.png"
#define PREFSLINENUM DATADIR _EXECSUFFIX_ "/pixmaps/num.png"
#define PREFSAPI DATADIR _EXECSUFFIX_ "/pixmaps/api.png"
#define PREFSQTAPI DATADIR _EXECSUFFIX_ "/pixmaps/qtapi.png"
#define PREFSFINDDEF DATADIR _EXECSUFFIX_ "/pixmaps/finddef.png"
#define PREFSLIVE DATADIR _EXECSUFFIX_ "/pixmaps/live.png"
#define PREFSSEPERATOR DATADIR _EXECSUFFIX_ "/pixmaps/sep.png"
#define PREFSEXPAND DATADIR _EXECSUFFIX_ "/pixmaps/expand.png"

#define BOOKMARKPNG DATADIR _EXECSUFFIX_ "/pixmaps/BookMark.png"
#define USERMARK1PNG DATADIR _EXECSUFFIX_ "/pixmaps/UserMark1.png"
#define USERMARK2PNG DATADIR _EXECSUFFIX_ "/pixmaps/UserMark2.png"
#define USERMARK3PNG DATADIR _EXECSUFFIX_ "/pixmaps/UserMark3.png"
#define USERMARK4PNG DATADIR _EXECSUFFIX_ "/pixmaps/UserMark4.png"
#define ROOTEDITPNG DATADIR _EXECSUFFIX_ "/pixmaps/ROOTKKEdit.png"
#define MENUKKEDITPNG DATADIR _EXECSUFFIX_  "/pixmaps/MenuKKEdit.png"
#define MANPAGEEDITPNG DATADIR _EXECSUFFIX_  "/pixmaps/ManPageEditor.png"
#define PLUGMENUPNG DATADIR _EXECSUFFIX_  "/pixmaps/KKEditPlugMenu.png"

//folders
#define GHELPFOLDER DATADIR _EXECSUFFIX_ "/help"
#define GSTYLESFOLDER DATADIR _EXECSUFFIX_ "/styles"
#define GPLUGSFOLDER DATADIR _EXECSUFFIX_ "/plugins-gtk"
#define GAPPFOLDER DATADIR _EXECSUFFIX_
#define GDOCSFOLDER  DATADIR _EXECSUFFIX_ "/docs"
#define GTOOLSFOLDER DATADIR _EXECSUFFIX_ "/tools"

//files
#define DOXYDOCFILE DATADIR _EXECSUFFIX_ "/docs/Doxyfile"

#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#include <ctype.h>
#include <gdk/gdkkeysyms.h>

#include <libgen.h>
#include <time.h>
#include <libintl.h>
#include <locale.h>

//#include "../src/links.h"

#include "sliceclass.h"
#include "kkedit-plugins.h"

#include "textbuffer.h"
#include "backclass.h"

#include "bookmarks.h"

#ifdef _ASPELL_
#include <aspell.h>
#endif

#ifdef _BUILDDOCVIEWER_
#include <webkit/webkit.h>
#endif

#include "globals.h"
#include "internet.h"
#include "pages.h"
#include "navcallbacks.h"

#include "plugins.h"
#include "callbacks.h"
#include "guis.h"
#include "searchcallbacks.h"
#include "spellcheck.h"
#include "files.h"
#include "completion.h"
#include "scripting.h"


#define VISIBLE __attribute__((visibility("default")))
#define PROTECTED __attribute__((visibility("protected")))
#endif



