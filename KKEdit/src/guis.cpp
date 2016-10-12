/*
 *
 * ©K. D. Hedger. Fri  6 May 10:51:14 BST 2016 kdhedger68713@gmail.com

 * This file (guis.cpp) is part of KKEdit.

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

#include "kkedit-includes.h"

#define TABLECOLS 2

GtkWidget		*recent;
GtkToolItem		*tool[MAXITEMS]={NULL,};
GtkIconView		*iconView=NULL;
GtkListStore	*listStore=NULL;

GtkWidget		*prefHBox;
GtkWidget		*prefHBox1;
GtkWidget		*prefHBox2;
GtkWidget		*pagevbox;

GtkWidget		*entries[NUMSHORTCUTS];

const char		*shortcuttext[NUMSHORTCUTS]={DELETE_LINE_LABEL,DELETE_TO_EOL_LABEL,DELETE_TO_BOL_LABEL,SELECT_WORD_LABEL,DELETE_WORD_LABEL,DUPLICATE_LINE_LABEL,SELECT_LINE_LABEL,LINE_UP_LABEL,LINE_DOWN_LABEL,SELECT_TO_EOL_LABEL,SELECT_TO_BOL_LABEL,SELECTION_UP_LABEL,SELECTION_DOWN_LABEL,SHOW_COMPLETION_LABEL,HIDE_TAB_LABEL};

GtkWidget	*prefsWidgets[MAXPREFSWIDGETS];
GObject	*prefsIntWidgets[MAXPREFSINTWIDGETS];

menuDataStruct	menuData[]=
	{
//file
		{MENU_NEW_LABEL,GTK_STOCK_NEW,GDK_KEY_N,GDK_CONTROL_MASK,(void*)&newFile,NEWMENUNAME,NULL},
		{MENU_OPEN_LABEL,GTK_STOCK_OPEN,GDK_KEY_O,GDK_CONTROL_MASK,(void*)&doOpenFile,OPENMENUNAME,NULL},
		{MENU_OPEN_AS_HEXDUMP_LABEL,GTK_STOCK_OPEN,0,0,(void*)&openAsHexDump,HEXDUMPMENUNAME,NULL},
		{MENU_NEW_ADMIN_LABEL,ROOTEDITPNG,0,0,(void*)&newEditor,NEWADMINMENUNAME,(gpointer)NEWROOTEDITOR},
		{MENU_NEW_EDITOR_LABEL,MENUKKEDITPNG,0,0,(void*)&newEditor,NEWEDITORMENUNAME,(gpointer)NEWEDITOR},
		{MENU_MANPAGE_EDITOR_LABEL,MANPAGEEDITPNG,0,0,(void*)&newEditor,MANEDITORMENUNAME,(gpointer)NEWMANPAGEEDITOR},
		{MENU_BUILD_DOCS_LABEL,GTK_STOCK_COPY,0,0,(void*)&doDoxy,DOXYBUILDMENUNAME,(gpointer)1},
		{MENU_RECENT_FILES_LABEL,NULL,0,0,NULL,RECENTMENUNAME,NULL},
		{MENU_SAVE_LABEL,GTK_STOCK_SAVE,GDK_KEY_S,GDK_CONTROL_MASK,(void*)&saveFile,SAVEMENUNAME,NULL},
		{MENU_SAVE_AS_LABEL,GTK_STOCK_SAVE_AS,GDK_KEY_S,GDK_CONTROL_MASK+GDK_SHIFT_MASK,(void*)&saveFile,SAVEASMENUNAME,(void*)1},
		{MENU_SAVE_ALL_LABEL,GTK_STOCK_SAVE,0,0,(void*)&doSaveAll,SAVEALLMENUNAME,NULL},
		{MENU_SAVE_SESSION_LABEL,GTK_STOCK_SAVE,0,0,(void*)&saveSession,SAVESESSIONMENUNAME,NULL},
		{MENU_RESTORE_SESSION_LABEL,GTK_STOCK_OPEN,0,0,(void*)&restoreSession,RESTORESESSIONMENUNAME,NULL},
		{MENU_PRINT_LABEL,GTK_STOCK_PRINT,GDK_KEY_P,GDK_CONTROL_MASK,(void*)&printFile,PRINTMENUNAME,NULL},
		{MENU_CLOSE_LABEL,GTK_STOCK_CLOSE,GDK_KEY_W,GDK_CONTROL_MASK,(void*)&closeTab,CLOSEMENUNAME,NULL},
		{MENU_CLOSE_ALL_LABEL,GTK_STOCK_CLOSE,0,0,(void*)&closeAllTabs,CLOSEALLMENUNAME,NULL},
		{MENU_REVERT_TO_SAVED_LABEL,GTK_STOCK_REVERT_TO_SAVED,0,0,(void*)&reloadFile,REVERTMENUNAME,NULL},
		{MENU_QUIT_LABEL,GTK_STOCK_QUIT,GDK_KEY_Q,GDK_CONTROL_MASK,(void*)&doShutdown,QUITMENUNAME,NULL},
//edit
		{MENU_UNDO_LABEL,GTK_STOCK_UNDO,GDK_KEY_Z,GDK_CONTROL_MASK,(void*)&undo,UNDOMENUNAME,NULL},
		{MENU_REDO_LABEL,GTK_STOCK_REDO,GDK_KEY_Z,GDK_CONTROL_MASK+GDK_SHIFT_MASK,(void*)&redo,REDOMENUNAME,NULL},
		{MENU_UNDO_ALL_LABEL,GTK_STOCK_UNDO,0,0,(void*)&unRedoAll,UNDOALLMENUNAME,(void*)0},
		{MENU_REDO_ALL_LABEL,GTK_STOCK_REDO,0,0,(void*)&unRedoAll,REDOALLMENUNAME,(void*)1},
		{MENU_CUT_LABEL,GTK_STOCK_CUT,GDK_KEY_X,GDK_CONTROL_MASK,(void*)&cutToClip,CUTMENUNAME,NULL},
		{MENU_COPY_LABEL,GTK_STOCK_COPY,GDK_KEY_C,GDK_CONTROL_MASK,(void*)&copyToClip,COPYMENUNAME,NULL},
		{MENU_PASTE_LABEL,GTK_STOCK_PASTE,GDK_KEY_V,GDK_CONTROL_MASK,(void*)&pasteFromClip,PASTEMENUNAME,NULL},
//find
		{MENU_FIND_LABEL,GTK_STOCK_FIND,GDK_KEY_F,GDK_CONTROL_MASK,(void*)&find,FINDMENUNAME,NULL},
		{MENU_FIND_NEXT_LABEL,GTK_STOCK_GO_FORWARD,GDK_KEY_G,GDK_CONTROL_MASK,(void*)&findNext,FINDNEXTMENUNAME,NULL},
//tabs
		{MENU_SORT_TABS_LABEL,GTK_STOCK_SORT_ASCENDING,0,0,(void*)&sortTabs,SORTTABSMENUNAME,NULL},
		{MENU_SHOW_ALL_TABS_LABEL,GTK_STOCK_ADD,0,0,(void*)&showAllTabs,SHOWTABSMENUNAME,NULL},
		{MENU_SELECT_TAB_LABEL,GTK_STOCK_EDIT,0,0,NULL,SELECTTABMENUNAME,NULL},
//prefs
		{MENU_PREFERENCES_LABEL,GTK_STOCK_PREFERENCES,0,0,(void*)&doPrefs,PREFSMENUNAME,NULL},
		{MENU_PLUG_PREFS_LABEL,GTK_STOCK_PREFERENCES,0,0,(void*)&doPlugPrefs,PLUGPREFSMENUNAME,NULL},
//view
		{MENU_SHOW_DOCS_LABEL,NULL,0,0,(void*)&doDoxy,SHOWDOCSMENUNAME,NULL},
		{MENU_SHOW_BM_BAR_LABEL,NULL,0,0,(void*)&toggleBookMarkBar,SHOWDOCSMENUNAME,NULL},
		{MENU_HIDE_BM_BAR_LABEL,NULL,0,0,(void*)&toggleBookMarkBar,SHOWDOCSMENUNAME,NULL},
		{MENU_HIDE_TOOLBAR_LABEL,NULL,0,0,(void*)&toggleToolBar,SHOWTOOLBARMENUNAME,NULL},
		{MENU_SHOW_TOOLBAR_LABEL,NULL,0,0,(void*)&toggleToolBar,SHOWTOOLBARMENUNAME,NULL},
		{MENU_SHOW_TOOL_OP_LABEL,NULL,0,0,(void*)&toggleToolOutput,SHOWTOOLOUTMENUNAME,NULL},
		{MENU_SHOW_TOOL_OP_LABEL,NULL,0,0,(void*)&toggleToolOutput,SHOWTOOLOUTMENUNAME,NULL},
		{MENU_HIDE_STATUS_LABEL,NULL,0,0,(void*)&toggleStatusBar,SHOWSTATUSMENUNAME,NULL},
		{MENU_SHOW_STATUS_LABEL,NULL,0,0,(void*)&toggleStatusBar,SHOWSTATUSMENUNAME,NULL},
#ifdef _BUILDDOCVIEWER_
		{MENU_SHOW_DOCVIEWER_LABEL,NULL,0,0,(void*)&toggleDocviewer,SHOWDOCVIEWERMENUNAME,NULL},
#else
		{"",NULL,0,0,NULL,NULL,NULL},
#endif

//toggles
		{MENU_SHOW_LINE_NUMBERS_LABEL,NULL,0,0,(void*)&toggleLineNumbers,VIEWSHOWLINENUMERS,NULL},
		{MENU_WRAP_LINES_LABEL,NULL,0,0,(void*)&toggleWrapLines,VIEWWRAPLINES,NULL},
		{MENU_HIGHLIGHT_LINE_LABEL,NULL,0,0,(void*)&toggleHighlightCurrent,VIEWHIGHLIGHT,NULL},
		{MENU_NO_SYNTAX_LABEL,NULL,0,0,(void*)&toggleSyntax,VIEWNOSYNTAX,NULL},
		{MENU_SHOW_SPACES_LABEL,NULL,0,0,(void*)&toggleWhitespace,VIEWWHITESPACE,NULL},
		{MENU_SHOW_AUTO_COMPLETE,NULL,0,0,(void*)&toggleAutoComplete,VIEWAUTOCOMPLETE,NULL},
//nav
		{MENU_GOTO_DEFINE_LABEL,GTK_STOCK_EXECUTE,0,0,(void*)&goToDefinition,GOTODEFMENUNAME,NULL},
		{MENU_OPEN_INCLUDE_LABEL,GTK_STOCK_OPEN,GDK_KEY_I,GDK_CONTROL_MASK,(void*)&findFile,OPENINCLUDEMENUNAME,NULL},
		{MENU_GOTO_LINE_LABEL,GTK_STOCK_GO_DOWN,0,0,(void*)&jumpToLine,GOTOLINEMENUNAME,NULL},
		{MENU_FIND_DEFINE_LABEL,GTK_STOCK_FIND,0,0,(void*)&functionSearch,SEARCHFORDEFMENUNAME,NULL},
		{MENU_FIND_IN_GTKAPI_LABEL,GTK_STOCK_FIND,0,0,(void*)&searchGtkDocs,SEARCHGTKMENUNAME,NULL},
		{MENU_FIND_IN_QTAPI_LABEL,GTK_STOCK_FIND,0,0,(void*)&searchQT5Docs,SEARCHQT5MENUNAME,NULL},
		{MENU_FIND_IN_DOCS_LABEL,GTK_STOCK_FIND,0,0,(void*)&doxyDocs,SEARCHDOXYMENUNAME,NULL},
		{MENU_GO_BACK_LABEL,GTK_STOCK_GO_BACK,0,0,(void*)&navigateHistory,GOBACKMENUNAME,(void*)NAVLAST},
		{MENU_GO_FORWARD_LABEL,GTK_STOCK_GO_FORWARD,0,0,(void*)&navigateHistory,GOFORWARDMENUNAME,(void*)NAVNEXT},
		{MENU_CONTINUE_LABEL,GTK_STOCK_MEDIA_PLAY,0,0,(void*)&sendContinue,CONTINUEMENUNAME,NULL},
//functions
//bookmarks
//tools
		{MENU_EXT_TOOLS_LABEL,GTK_STOCK_EDIT,0,0,(void*)&doMakeTool,EXTOOLSMENUNAME,NULL},
//help
		{MENU_ABOUT_LABEL,GTK_STOCK_ABOUT,0,0,(void*)&doAbout,ABOUTMENUNAME,NULL},
		{MENU_HELP_LABEL,GTK_STOCK_HELP,0,0,(void*)&openHelp,HELPLOCALMENUNAME,(void*)0},
		{MENU_HELP_ONLINE_LABEL,GTK_STOCK_HELP,0,0,(void*)&openHelp,HELPONLINEMENUNAME,(void*)1},
		{MENU_GET_PLUGS_LABEL,PLUGMENUPNG,0,0,(void*)&getPlugins,GETPLUGSMENUNAME,NULL},

//context menus
		{CUSTOM_WORD_CONTEXT_LABEL,GTK_STOCK_EDIT,0,0,(void*)&addtoCustomWordList,NULL,NULL},
//other
		{"",NULL,0,0,NULL,NULL,NULL},
#ifdef _ASPELL_
		{CHECK_SPELLING_CONTEXT_LABEL,GTK_STOCK_SPELL_CHECK,0,0,(void*)&checkWord,NULL,NULL},
#else
		{"",NULL,0,0,NULL,NULL,NULL},
#endif

//tab popups
		{COPY_FOLDER_PATH_LABEL,GTK_STOCK_COPY,0,0,(void*)&doTabMenu,NULL,NULL},
		{COPY_FILE_PATH_LABEL,GTK_STOCK_COPY,0,0,(void*)&doTabMenu,NULL,NULL},
		{COPY_FILE_NAME_LABEL,GTK_STOCK_COPY,0,0,(void*)&doTabMenu,NULL,NULL},
#ifdef _ASPELL_
		{SPELL_CHECK_LABEL,GTK_STOCK_SPELL_CHECK,0,0,(void*)&doSpellCheckDoc,NULL,NULL},
#else
		{NULL,NULL,0,0,NULL,NULL,NULL},
#endif
		{SOURCE_HILITE_LABEL,GTK_STOCK_PREFERENCES,0,0,NULL,NULL,NULL},
		{MENU_HIDE_TAB_LABEL,GTK_STOCK_REMOVE,0,0,(void*)&hideTab,NULL,NULL},

		{NULL,NULL,0,0,NULL,NULL,NULL}
	};

char	keyStrBuffer[32];
void setHotKeyString(unsigned menunumber)
{
	const char	*ckeystr="";
	const char	*skeystr="";

	if(menuData[menunumber].key!=0)
		{
			if(GDK_CONTROL_MASK & menuData[menunumber].mod)
				ckeystr="Ctrl+";
			if(GDK_SHIFT_MASK & menuData[menunumber].mod)
				skeystr="Shift+";

			sprintf(keyStrBuffer,"%s%s%s",skeystr,ckeystr,gdk_keyval_name(menuData[menunumber].key));
		}
	else
		keyStrBuffer[0]=0;
}

GtkWidget* newCheckMenuItem(unsigned menunumber,GtkWidget *parent,bool initial)
{
	GtkWidget	*menu;

	menu=gtk_check_menu_item_new_with_label(gettext(menuData[menunumber].menuLabel));
	gtk_check_menu_item_set_active((GtkCheckMenuItem*)menu,initial);

	if(menuData[menunumber].key>0)
		gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accgroup,menuData[menunumber].key,(GdkModifierType)menuData[menunumber].mod,GTK_ACCEL_VISIBLE);

	gtk_menu_shell_append(GTK_MENU_SHELL(parent),menu);
	g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuData[menunumber].cb),menuData[menunumber].userData);
	gtk_widget_set_name(menu,menuData[menunumber].widgetName);

	return(menu);
}

GtkWidget* newMenuItem(unsigned menunumber,GtkWidget *parent)
{
	GtkWidget	*menu;
#ifdef _USEGTK3_
	char		*menulabel;
	GtkWidget	*menuhbox;
	GtkWidget	*pad;
	GtkWidget	*image;
	GtkWidget	*ritelabel;
	char		*labelwithspace;

	menu=gtk_menu_item_new_with_mnemonic(gettext(menuData[menunumber].menuLabel));
	if((showMenuIcons==true) && (menuData[menunumber].stockID!=NULL))
		{
			setHotKeyString(menunumber);
			gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(menu)));
			menuhbox=createNewBox(NEWHBOX,false,0);
			pad=createNewBox(NEWHBOX,false,0);

			image=gtk_image_new_from_icon_name(menuData[menunumber].stockID,GTK_ICON_SIZE_MENU);
			gtk_box_pack_start((GtkBox*)menuhbox,image,false,false,0);

			asprintf(&labelwithspace," %s",gettext(menuData[menunumber].menuLabel));
			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(labelwithspace),false,false,0);
			free(labelwithspace);
			gtk_box_pack_start(GTK_BOX(menuhbox),pad,true,true,0);

			ritelabel=gtk_label_new(keyStrBuffer);
			gtk_widget_set_sensitive(ritelabel,false);
			gtk_box_pack_start(GTK_BOX(menuhbox),ritelabel,false,false,8);

			gtk_container_add(GTK_CONTAINER(menu),menuhbox);
		}
#else
	if(menuData[menunumber].stockID!=NULL)
		menu=gtk_image_menu_item_new_from_stock(menuData[menunumber].stockID,NULL);
	else
		menu=gtk_menu_item_new_with_mnemonic(gettext(menuData[menunumber].menuLabel));
#endif
	if(menuData[menunumber].key>0)
		gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accgroup,menuData[menunumber].key,(GdkModifierType)menuData[menunumber].mod,GTK_ACCEL_VISIBLE);

	if(parent!=NULL)
		gtk_menu_shell_append(GTK_MENU_SHELL(parent),menu);
	g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuData[menunumber].cb),menuData[menunumber].userData);
	gtk_widget_set_name(menu,menuData[menunumber].widgetName);

	return(menu);
}

GtkWidget* newImageMenuItem(unsigned menunumber,GtkWidget *parent)
{
	GtkWidget	*menu;
#ifdef _USEGTK3_
	char		*menulabel;
	GtkWidget	*menuhbox;
	GtkWidget	*pad;
	GtkWidget	*image;
	GtkWidget	*ritelabel;
	char		*labelwithspace;

	menu=gtk_menu_item_new_with_mnemonic(gettext(menuData[menunumber].menuLabel));
	if(showMenuIcons==true)
		{
			setHotKeyString(menunumber);
			gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(menu)));
			menuhbox=createNewBox(NEWHBOX,false,0);
			pad=createNewBox(NEWHBOX,false,0);

			image=gtk_image_new_from_icon_name(menuData[menunumber].stockID,GTK_ICON_SIZE_MENU);
			gtk_box_pack_start((GtkBox*)menuhbox,image,false,false,0);

			//gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new(" "),false,false,0);
			asprintf(&labelwithspace," %s",gettext(menuData[menunumber].menuLabel));
			//gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(gettext(menuData[menunumber].menuLabel)),false,false,0);
			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(labelwithspace),false,false,0);
			free(labelwithspace);
			gtk_box_pack_start(GTK_BOX(menuhbox),pad,true,true,0);

			ritelabel=gtk_label_new(keyStrBuffer);
			gtk_widget_set_sensitive(ritelabel,false);
			gtk_box_pack_start(GTK_BOX(menuhbox),ritelabel,false,false,8);

			gtk_container_add(GTK_CONTAINER(menu),menuhbox);
		}
#else
	GtkWidget	*image;
	menu=gtk_image_menu_item_new_with_mnemonic(gettext(menuData[menunumber].menuLabel));
	image=gtk_image_new_from_stock(menuData[menunumber].stockID,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menu,image);
#endif
	if(menuData[menunumber].key>0)
		gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accgroup,menuData[menunumber].key,(GdkModifierType)menuData[menunumber].mod,GTK_ACCEL_VISIBLE);

	if(parent!=NULL)
		gtk_menu_shell_append(GTK_MENU_SHELL(parent),menu);
	if(menuData[menunumber].cb!=NULL)
		g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuData[menunumber].cb),menuData[menunumber].userData);
	if(menuData[menunumber].widgetName!=NULL)
		gtk_widget_set_name(menu,menuData[menunumber].widgetName);

	return(menu);
}

GtkWidget* newPixmapMenuItem(unsigned menunumber,GtkWidget *parent)
{
	GtkWidget	*menu;
#ifdef _USEGTK3_
	char		*menulabel;
	GtkWidget	*menuhbox;
	GtkWidget	*pad;
	GtkWidget	*image;
	GtkWidget	*ritelabel;
	char		*labelwithspace;

	menu=gtk_menu_item_new_with_mnemonic(gettext(menuData[menunumber].menuLabel));
	if(showMenuIcons==true)
		{
			setHotKeyString(menunumber);
			gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(menu)));
			menuhbox=createNewBox(NEWHBOX,false,0);
			pad=createNewBox(NEWHBOX,false,0);

			image=gtk_image_new_from_file(menuData[menunumber].stockID);
			gtk_box_pack_start((GtkBox*)menuhbox,image,false,false,0);

			//gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new(" "),false,false,0);
			asprintf(&labelwithspace," %s",gettext(menuData[menunumber].menuLabel));
//			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(gettext(menuData[menunumber].menuLabel)),false,false,0);
			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(labelwithspace),false,false,0);
			free(labelwithspace);
			gtk_box_pack_start(GTK_BOX(menuhbox),pad,true,true,0);

			ritelabel=gtk_label_new(keyStrBuffer);
			gtk_widget_set_sensitive(ritelabel,false);
			gtk_box_pack_start(GTK_BOX(menuhbox),ritelabel,false,false,8);

			gtk_container_add(GTK_CONTAINER(menu),menuhbox);
		}
#else
	GtkWidget	*image;
	menu=gtk_image_menu_item_new_with_mnemonic(gettext(menuData[menunumber].menuLabel));
	image=gtk_image_new_from_file(menuData[menunumber].stockID);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menu,image);
#endif
	if(menuData[menunumber].key>0)
		gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accgroup,menuData[menunumber].key,(GdkModifierType)menuData[menunumber].mod,GTK_ACCEL_VISIBLE);

	gtk_menu_shell_append(GTK_MENU_SHELL(parent),menu);
	g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuData[menunumber].cb),menuData[menunumber].userData);
	gtk_widget_set_name(menu,menuData[menunumber].widgetName);

	return(menu);
}

void findTool(toolStruct *data,char *toolname)
{
	ERRDATA
	if(strcmp(toolname,data->menuName)==0)
		selectedToolFromDrop=data;
	ERRDATA
}

void selectToolOptions(GtkWidget *widget,gpointer data)
{
	ERRDATA
	char	*text=gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
	int		flags=0;

	selectedToolFromDrop=NULL;

	g_list_foreach(toolsList,(GFunc)findTool,text);
	if(selectedToolFromDrop!=NULL)
		{
			gtk_toggle_button_set_active((GtkToggleButton*)inPopupWidget,selectedToolFromDrop->inPopUp);
			gtk_toggle_button_set_active((GtkToggleButton*)alwaysPopupWidget,selectedToolFromDrop->alwaysPopup);
			gtk_toggle_button_set_active((GtkToggleButton*)clearViewWidget,selectedToolFromDrop->clearView);
			gtk_toggle_button_set_active((GtkToggleButton*)runAsRootWidget,selectedToolFromDrop->runAsRoot);
			gtk_toggle_button_set_active((GtkToggleButton*)runAsRootWidget,selectedToolFromDrop->runAsRoot);
			gtk_toggle_button_set_active((GtkToggleButton*)useBarWidget,selectedToolFromDrop->useBar);

			gtk_entry_set_text((GtkEntry*)toolNameWidget,selectedToolFromDrop->menuName);
			gtk_entry_set_text((GtkEntry*)commandLineWidget,selectedToolFromDrop->command);
			if(selectedToolFromDrop->comment!=NULL)
				gtk_entry_set_text((GtkEntry*)commentWidget,selectedToolFromDrop->comment);
			else
				gtk_entry_set_text((GtkEntry*)commentWidget,"");

			if(selectedToolFromDrop->keyCode!=GDK_KEY_VoidSymbol)
				gtk_entry_set_text((GtkEntry*)keyWidget,gdk_keyval_name(selectedToolFromDrop->keyCode));
			else
				gtk_entry_set_text((GtkEntry*)keyWidget,"");

			flags=selectedToolFromDrop->flags;
			if(flags & TOOL_ASYNC)
				gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,false);
			else
				gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,true);

			if((flags & TOOL_INSERT_MASK)==TOOL_IGNORE_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
			if((flags & TOOL_INSERT_MASK)==TOOL_VIEW_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)outputWidget,true);
			if((flags & TOOL_INSERT_MASK)==TOOL_PASTE_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)pasteWidget,true);
			if((flags & TOOL_INSERT_MASK)==TOOL_REPLACE_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)replaceWidget,true);

			if((gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false) ||(gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget)==true))
				{
					gtk_widget_set_sensitive(ignoreWidget,false);
					gtk_widget_set_sensitive(pasteWidget,false);
					gtk_widget_set_sensitive(replaceWidget,false);
					gtk_widget_set_sensitive(outputWidget,false);
				}
			else
				{
					gtk_widget_set_sensitive(ignoreWidget,true);
					gtk_widget_set_sensitive(pasteWidget,true);
					gtk_widget_set_sensitive(replaceWidget,true);
					gtk_widget_set_sensitive(outputWidget,true);
				}

			if(flags & TOOL_SHOW_DOC)
				gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,true);
			else
				gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,false);
		}
	ERRDATA
}

void setUpToolBar(void)
{
	ERRDATA
	GtkToolItem		*toolbutton;
#ifdef _USEGTK3_
	GtkWidget		*image;
#endif
	GtkRecentFilter	*filter;
	GtkWidget		*menu;

	recent=gtk_recent_chooser_menu_new();
	gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recent),false);
	gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent),GTK_RECENT_SORT_MRU);
	gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent),MAXRECENT);

	filter=gtk_recent_filter_new();
	gtk_recent_filter_add_application(filter,APPEXECNAME);
	gtk_recent_chooser_set_filter(GTK_RECENT_CHOOSER(recent),filter);
	g_signal_connect(recent,"item_activated",G_CALLBACK(recentFileMenu),NULL);

	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						newButton=createNewToolItem(GTK_STOCK_NEW,NEW_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,newButton,-1);
						g_signal_connect(G_OBJECT(newButton),"clicked",G_CALLBACK(newFile),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)newButton,NEW_TT_LABEL);
						break;
					case 'O':
//open+recent
#ifdef _USEGTK3_
						image=gtk_image_new_from_icon_name(GTK_STOCK_OPEN,GTK_ICON_SIZE_LARGE_TOOLBAR);
						openButton=gtk_menu_tool_button_new(image,OPEN_TOOLBAR_LABEL);
#else
						openButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_OPEN);
#endif
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(openButton),recent);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(openButton),RECENT_FILE_TT_LABEL);
						g_signal_connect(G_OBJECT(openButton),"clicked",G_CALLBACK(doOpenFile),NULL);
						gtk_toolbar_insert(toolBar,openButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)openButton,OPEN_TT_LABEL);
						break;
					case 'S':
//save
						saveButton=createNewToolItem(GTK_STOCK_SAVE,SAVE_TOOLBAR_LABEL);
						g_signal_connect(G_OBJECT(saveButton),"clicked",G_CALLBACK(saveFile),NULL);
						gtk_toolbar_insert(toolBar,saveButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)saveButton,SAVE_TT_LABEL);
						break;
					case 's':
						gtk_toolbar_insert(toolBar,gtk_separator_tool_item_new(),-1);
						break;
					case 'X':
//cut
						cutButton=createNewToolItem(GTK_STOCK_CUT,CUT_TOOLBAR_LABEL);
						g_signal_connect(G_OBJECT(cutButton),"clicked",G_CALLBACK(cutToClip),NULL);
						gtk_toolbar_insert(toolBar,cutButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)cutButton,CUT_TT_LABEL);
						break;
					case 'C':
//copy
						copyButton=createNewToolItem(GTK_STOCK_COPY,COPY_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,copyButton,-1);
						g_signal_connect(G_OBJECT(copyButton),"clicked",G_CALLBACK(copyToClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)copyButton,COPY_TT_LABEL);
						break;
					case 'P':
//paste
						pasteButton=createNewToolItem(GTK_STOCK_PASTE,PASTE_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,pasteButton,-1);
						g_signal_connect(G_OBJECT(pasteButton),"clicked",G_CALLBACK(pasteFromClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)pasteButton,PASTE_TT_LABEL);
						break;
					case 'U':
//undo
						undoButton=createNewToolItem(GTK_STOCK_UNDO,UNDO_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,undoButton,-1);
						g_signal_connect(G_OBJECT(undoButton),"clicked",G_CALLBACK(undo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)undoButton,UNDO_TT_LABEL);
						break;
					case 'R':
//redo
						redoButton=createNewToolItem(GTK_STOCK_REDO,REDO_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,redoButton,-1);
						g_signal_connect(G_OBJECT(redoButton),"clicked",G_CALLBACK(redo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)redoButton,REDO_TT_LABEL);
						break;
					case 'F':
//find
						findButton=createNewToolItem(GTK_STOCK_FIND,FIND_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,findButton,-1);
						g_signal_connect(G_OBJECT(findButton),"clicked",G_CALLBACK(find),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)findButton,FIND_TT_LABEL);
						break;
					case 'G':
//navigation
						gotoDefButton=createNewToolItem(GTK_STOCK_EXECUTE,GOTO_DEFINE_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,gotoDefButton,-1);
						g_signal_connect(G_OBJECT(gotoDefButton),"clicked",G_CALLBACK(goToDefinition),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoDefButton,GOTO_DEFINE_TT_LABEL);
						break;
//go back
					case 'B':
//					{
#ifdef _USEGTK3_
						image=gtk_image_new_from_icon_name(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_LARGE_TOOLBAR);
						backButton=gtk_menu_tool_button_new(image,BACK_TOOLBAR_LABEL);
#else
						backButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
#endif

						gtk_toolbar_insert(toolBar,backButton,-1);
						g_signal_connect(G_OBJECT(backButton),"clicked",G_CALLBACK(navigateHistory),(void*)NAVLAST);
						gtk_widget_set_tooltip_text((GtkWidget*)backButton,BACK_TT_LABEL);
//back history
						menu=gtk_menu_new();
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(backButton),menu);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(backButton),BACK_MENU_TT_LABEL);

						globalHistory->setHistBackMenu(menu);		
						break;
//go forward
					case 'W':
#ifdef _USEGTK3_
						image=gtk_image_new_from_icon_name(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_LARGE_TOOLBAR);
						forwardButton=gtk_menu_tool_button_new(image,FORWARD_TOOLBAR_LABEL);
#else
						forwardButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
#endif
						gtk_toolbar_insert(toolBar,forwardButton,-1);
						g_signal_connect(G_OBJECT(forwardButton),"clicked",G_CALLBACK(navigateHistory),(void*)NAVNEXT);
						gtk_widget_set_tooltip_text((GtkWidget*)forwardButton,FORWARD_TT_LABEL);
//foward history
						menu=gtk_menu_new();
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(forwardButton),menu);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(forwardButton),FORWARD_MENU_TT_LABEL);

						globalHistory->setHistForwardMenu(menu);		
						break;

//jump to line
					case '9':
						lineNumberWidget=gtk_entry_new();
						gotoLineButton=gtk_tool_item_new();
						gtk_entry_set_width_chars((GtkEntry *)lineNumberWidget,6);
						gtk_container_add((GtkContainer *)gotoLineButton,lineNumberWidget);
						gtk_toolbar_insert(toolBar,gotoLineButton,-1);
						g_signal_connect_after(G_OBJECT(lineNumberWidget),"key-release-event",G_CALLBACK(jumpToLineFromBar),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoLineButton,TO_LINE_TT_LABEL);
						break;
					case 'A':
//find in gtkdoc
						findApiWidget=gtk_entry_new();
						findApiButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findApiButton,findApiWidget);
						gtk_toolbar_insert(toolBar,findApiButton,-1);
						g_signal_connect_after(G_OBJECT(findApiWidget),"activate",G_CALLBACK(docSearchFromBar),(void*)findApiWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findApiButton,TO_API_TT_LABEL);
						break;

					case 'Q':
//find in qt5doc
						findQtApiWidget=gtk_entry_new();
						findQtApiButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findQtApiButton,findQtApiWidget);
						gtk_toolbar_insert(toolBar,findQtApiButton,-1);
						g_signal_connect_after(G_OBJECT(findQtApiWidget),"activate",G_CALLBACK(qt5DocSearchFromBar),(void*)findQtApiWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findQtApiButton,TO_QTAPI_TT_LABEL);
						break;
					case 'D':
//find in function def
						findDefWidget=gtk_entry_new();
						findFuncDefButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findFuncDefButton,findDefWidget);
						gtk_toolbar_insert(toolBar,findFuncDefButton,-1);
						g_signal_connect_after(G_OBJECT(findDefWidget),"activate",G_CALLBACK(defSearchFromBar),(void*)findDefWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findFuncDefButton,TO_DEFINE_TT_LABEL);
						break;
					case 'L':
//livesearch
						liveSearchWidget=gtk_entry_new();
						liveSearchButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)liveSearchButton,liveSearchWidget);
						gtk_toolbar_insert(toolBar,liveSearchButton,-1);
						g_signal_connect_after(G_OBJECT(liveSearchWidget),"key-release-event",G_CALLBACK(doLiveSearch),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)liveSearchButton,LIVE_TT_LABEL);
						break;
					case 'E':
//expander
						toolbutton=gtk_separator_tool_item_new();
						gtk_tool_item_set_expand(toolbutton,true);
						gtk_separator_tool_item_set_draw((GtkSeparatorToolItem*)toolbutton,false);
						gtk_toolbar_insert(toolBar,toolbutton,-1);
						break;
				}
		}
	ERRDATA
}

void addToolToDrop(gpointer data,gpointer user_data)
{
	ERRDATA
	if(((toolStruct*)data)->global==false)
		gtk_combo_box_text_append_text((GtkComboBoxText*)toolSelect,((toolStruct*)data)->menuName);
	ERRDATA
}

void fillCombo(GtkComboBoxText *combo)
{
	ERRDATA
	g_list_foreach(toolsList,addToolToDrop,NULL);
	ERRDATA
}

gboolean getToolKey(GtkEntry *widget,GdkEventKey *event,gpointer data)
{
	ERRDATA
	if((event->type==GDK_KEY_PRESS) &&(event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			ERRDATA return(true);
		}

	if((event->type==GDK_KEY_PRESS)&&(event->state & GDK_CONTROL_MASK))
		gtk_entry_set_text(widget,gdk_keyval_name(event->keyval));

	ERRDATA return(true);
}

void doMakeTool(void)
{
	ERRDATA
	GtkWidget	*vbox;
	GtkWidget	*hbox;
	GtkWidget	*button;
	GtkWidget	*toolwin;
	GtkWidget	*infolabel;

	char		*placeholderinfo;

	sinkReturn=asprintf(&placeholderinfo,"%s",TOOLS_PLACEHOLDER_LABEL);

	toolwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)toolwin,TOOLS_EDIT_LABEL);
	vbox=createNewBox(NEWVBOX,false,8);

//select tool
	toolSelect=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(vbox),toolSelect,false,true,0);
	g_signal_connect(G_OBJECT(toolSelect),"changed",G_CALLBACK(selectToolOptions),NULL);
	fillCombo((GtkComboBoxText*) toolSelect);

//name
	toolNameWidget=gtk_entry_new();
	hbox=createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(TOOLS_NAME_LABEL),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),toolNameWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(toolNameWidget);
	gtk_entry_set_text((GtkEntry*)toolNameWidget,"New Tool");

//command
	commandLineWidget=gtk_entry_new();
	hbox=createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(TOOLS_COMMAND_LABEL),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commandLineWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(commandLineWidget);
	gtk_entry_set_text((GtkEntry*)commandLineWidget,"");
//key
	keyWidget=gtk_entry_new();
	gtk_widget_show(keyWidget);
	hbox=createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(TOOLS_SHORT_LABEL),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),keyWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)keyWidget,"");
	g_signal_connect(G_OBJECT(keyWidget),"key-press-event",G_CALLBACK(getToolKey),NULL);

//comment
	commentWidget=gtk_entry_new();
	gtk_widget_show(commentWidget);
	hbox=createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(TOOLS_COMMENT_LABEL),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commentWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)commentWidget,"");

//info
	infolabel=gtk_label_new(placeholderinfo);
	gtk_label_set_selectable((GtkLabel*)infolabel,true);
	gtk_box_pack_start(GTK_BOX(vbox),infolabel,false,false,0);
	ERRDATA debugFree(&placeholderinfo);
//in terminal
	inTermWidget=gtk_check_button_new_with_label(TOOLS_IN_TERM_LABEL);
	gtk_widget_set_name(inTermWidget,"interm");
	gtk_box_pack_start(GTK_BOX(vbox),inTermWidget,false,true,0);
	g_signal_connect(G_OBJECT(inTermWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//show in popup menu
	inPopupWidget=gtk_check_button_new_with_label(TOOLS_IN_POPUP_LABEL);
	gtk_widget_set_name(inPopupWidget,"inpopup");
	gtk_box_pack_start(GTK_BOX(vbox),inPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(inPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//always show in popup
	alwaysPopupWidget=gtk_check_button_new_with_label(TOOLS_ALWAYS_IN_POPUP_LABEL);
	gtk_widget_set_name(alwaysPopupWidget,"alwayspopup");
	gtk_box_pack_start(GTK_BOX(vbox),alwaysPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(alwaysPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags
//snch/async
	syncWidget=gtk_check_button_new_with_label(TOOLS_SYNC_LABEL);
	gtk_widget_set_name(syncWidget,"sync");
	gtk_box_pack_start(GTK_BOX(vbox),syncWidget,false,true,0);
	g_signal_connect(G_OBJECT(syncWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - show html doc
	showDocWidget=gtk_check_button_new_with_label(TOOLS_SHOW_HTML_LABEL);
	gtk_widget_set_name(showDocWidget,"showdoc");
	gtk_box_pack_start(GTK_BOX(vbox),showDocWidget,false,true,0);
	g_signal_connect(G_OBJECT(showDocWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - clear view first
	clearViewWidget=gtk_check_button_new_with_label(TOOLS_CLEAR_OP_LABEL);
	gtk_widget_set_name(clearViewWidget,"clearview");
	gtk_box_pack_start(GTK_BOX(vbox),clearViewWidget,false,true,0);
	g_signal_connect(G_OBJECT(clearViewWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - run as root
	runAsRootWidget=gtk_check_button_new_with_label(TOOLS_AS_ROOT_LABEL);
	gtk_widget_set_name(runAsRootWidget,"runasroot");
	gtk_box_pack_start(GTK_BOX(vbox),runAsRootWidget,false,true,0);
	g_signal_connect(G_OBJECT(runAsRootWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - use progressbar
	useBarWidget=gtk_check_button_new_with_label(TOOLS_USE_BAR_LABEL);
	gtk_widget_set_name(useBarWidget,"usebar");
	gtk_box_pack_start(GTK_BOX(vbox),useBarWidget,false,true,0);
	g_signal_connect(G_OBJECT(useBarWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - ignore
	ignoreWidget=gtk_radio_button_new_with_label(NULL,TOOLS_IGNORE_OP_LABEL);
	gtk_widget_set_name(ignoreWidget,"ignore");
	gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
	gtk_box_pack_start(GTK_BOX(vbox),ignoreWidget,false,true,0);

//flags - paste
	pasteWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,TOOLS_PASTE_OP_LABEL);
	gtk_widget_set_name(pasteWidget,"paste");
	gtk_box_pack_start(GTK_BOX(vbox),pasteWidget,false,true,0);

//flags - replace all
	replaceWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget, TOOLS_REPLACE_CONTENTS_LABEL);
	gtk_widget_set_name(replaceWidget,"replace");
	gtk_box_pack_start(GTK_BOX(vbox),replaceWidget,false,true,0);

//flags - view
	outputWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,TOOLS_VIEW_OP_LABEL);
	gtk_widget_set_name(outputWidget,"outtoview");
	gtk_box_pack_start(GTK_BOX(vbox),outputWidget,false,true,0);

//buttons
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),false,false,0);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);
#endif

	hbox=createNewBox(NEWHBOX,false,0);
	button=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);

	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"apply");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=createNewStockButton(GTK_STOCK_DELETE,TOOLS_DELETE_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"delete");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=createNewStockButton(GTK_STOCK_CANCEL,GTK_STOCK_CANCEL);

	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"cancel");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,false,2);

//show it
	gtk_container_add(GTK_CONTAINER(toolwin),(GtkWidget*)vbox);
	gtk_widget_show_all(toolwin);
	ERRDATA
}

void buildTools(void)
{
	ERRDATA
	GtkWidget		*menuitem;
	GtkWidget		*menu;
	GList			*ptr;
	bool			gotglobal=false;
	int				keyflags=0;

	buildToolsList();

	GtkWidget *submenu=gtk_menu_item_get_submenu((GtkMenuItem*)toolsMenu);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)toolsMenu,NULL);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(toolsMenu),menu);

//addtool
	menuitem=newImageMenuItem(MENUTOOLS,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	ptr=toolsList;
	menuData[MENUBLANK].cb=(void*)externalTool;
	menuData[MENUBLANK].stockID=NULL;
	menuData[MENUBLANK].key=0;
	while(ptr!=NULL)
		{
			if(((toolStruct*)ptr->data)->global==true)
				{
					gotglobal=true;
					menuData[MENUBLANK].menuLabel=((toolStruct*)ptr->data)->menuName;
					menuData[MENUBLANK].userData=(gpointer)ptr->data;
					menuitem=newMenuItem(MENUBLANK,menu);
					if( (((toolStruct*)ptr->data)->keyCode!=GDK_KEY_VoidSymbol) && (((toolStruct*)ptr->data)->keyCode!=0) )
						{
							keyflags=0;
							if(gdk_keyval_is_upper(((toolStruct*)ptr->data)->keyCode))
								keyflags=GDK_SHIFT_MASK;
							gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,((toolStruct*)ptr->data)->keyCode,(GdkModifierType)(GDK_CONTROL_MASK|keyflags),GTK_ACCEL_VISIBLE);
						}

					if(((toolStruct*)ptr->data)->comment!=NULL)
						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
				}
			ptr=g_list_next(ptr);
		}

	if(gotglobal==true)
		{
			menuitem=gtk_separator_menu_item_new();
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
		}

	ptr=toolsList;
	menuData[MENUBLANK].cb=(void*)externalTool;
	menuData[MENUBLANK].stockID=NULL;
	menuData[MENUBLANK].key=0;
	while(ptr!=NULL)
		{
			if(((toolStruct*)ptr->data)->global==false)
				{
					menuData[MENUBLANK].menuLabel=((toolStruct*)ptr->data)->menuName;
					menuData[MENUBLANK].userData=(gpointer)ptr->data;
					menuitem=newMenuItem(MENUBLANK,menu);
//TODO//
//needs tidying
					if( (((toolStruct*)ptr->data)->keyCode!=GDK_KEY_VoidSymbol) && (((toolStruct*)ptr->data)->keyCode!=0) )
						{
							keyflags=0;
							if(gdk_keyval_is_upper(((toolStruct*)ptr->data)->keyCode))
								{
									keyflags=GDK_SHIFT_MASK;
									if(gdk_keyval_is_lower(((toolStruct*)ptr->data)->keyCode))
									keyflags=0;
								}
							gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,((toolStruct*)ptr->data)->keyCode,(GdkModifierType)(GDK_CONTROL_MASK|keyflags),GTK_ACCEL_VISIBLE);
						}

					if(((toolStruct*)ptr->data)->comment!=NULL)
						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
				}
			ptr=g_list_next(ptr);
		}
	ERRDATA
}

void setNewPixbuf(const char *path,int bnum,const char *type)
{
	GtkTreeIter iter;
	GdkPixbuf	*pbuf;

	gtk_list_store_append(listStore, &iter);
	pbuf=gdk_pixbuf_new_from_file(path,NULL);
	if(pbuf!=NULL)
		{
			gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,bnum,-1);
			g_object_unref(pbuf);
		}
	gtk_widget_set_sensitive((GtkWidget*)tool[bnum],false);

}

void populateStore(void)
{
	ERRDATA
	char		*type;

	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			type=strndup((char*)&toolBarLayout[j],1);
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						setNewPixbuf(PREFSNEW,ITEMNEW,type);
						break;
					case 'O':
//open+recent
						setNewPixbuf(PREFSOPEN,ITEMOPEN,type);
						break;
					case 'S':
//save
						setNewPixbuf(PREFSSAVE,ITEMSAVE,type);
						break;

					case 'X':
//cut
						setNewPixbuf(PREFSCUT,ITEMCUT,type);
						break;
					case 'C':
//copy
						setNewPixbuf(PREFSCOPY,ITEMCOPY,type);
						break;
					case 'P':
//paste
						setNewPixbuf(PREFSPASTE,ITEMPASTE,type);
						break;
					case 'U':
//undo
						setNewPixbuf(PREFSUNDO,ITEMUNDO,type);
						break;
					case 'R':
//redo
						setNewPixbuf(PREFSREDO,ITEMREDO,type);
						break;
					case 'F':
//find
						setNewPixbuf(PREFSFIND,ITEMFIND,type);
						break;
					case 'G':
//navigation
						setNewPixbuf(PREFSQUESTION,ITEMGOTO,type);
						break;
//go back
					case 'B':
						setNewPixbuf(PREFSBACK,ITEMBACK,type);
						break;
//go forward
					case 'W':
						setNewPixbuf(PREFSFORWARD,ITEMFORWARD,type);
						break;
//goto line num
					case '9':
						setNewPixbuf(PREFSLINENUM,ITEMLINE,type);
						break;
//find api
					case 'A':
						setNewPixbuf(PREFSAPI,ITEMAPI,type);
						break;
//find qt api
					case 'Q':
						setNewPixbuf(PREFSQTAPI,ITEMQTAPI,type);
						break;
					case 'D':
//find define
						setNewPixbuf(PREFSFINDDEF,ITEMDEFINE,type);
						break;
					case 'L':
//live search
						setNewPixbuf(PREFSLIVE,ITEMLIVE,type);
						break;
//seperator
					case 's':
						setNewPixbuf(PREFSSEPERATOR,ITEMSEPERATOR,type);
						gtk_widget_set_sensitive((GtkWidget*)tool[14],true);
					break;
//expander
					case 'E':
						setNewPixbuf(PREFSEXPAND,ITEMEXPANDER,type);
						gtk_widget_set_sensitive((GtkWidget*)tool[15],true);
						break;
				}
			ERRDATA debugFree(&type);
		}
	ERRDATA
}

void addToToolBar(GtkWidget *widget,gpointer ptr)
{
	ERRDATA
	char	*holddata=toolBarLayout;
	char	*type;

	type=strndup((char*)ptr,1);
	toolBarLayout=type;
	populateStore();
	toolBarLayout=holddata;
	ERRDATA debugFree(&type);
}

void addPixbuf(const char *pixbuf,const char *data,int toolnumber,const char *tooltip)
{
	ERRDATA
	GtkWidget		*image;
	image=gtk_image_new_from_file(pixbuf);

	tool[toolnumber]=gtk_tool_button_new(image,NULL);
	gtk_box_pack_start(GTK_BOX(fromHBox),(GtkWidget*)tool[toolnumber],false,false,2);
	g_signal_connect(G_OBJECT(tool[toolnumber]),"clicked",G_CALLBACK(addToToolBar),(void*)data);
	gtk_widget_set_tooltip_text((GtkWidget*)tool[toolnumber],tooltip);
	ERRDATA
}

void populateDnD(void)
{
	ERRDATA
	addPixbuf(PREFSNEW,"N",ITEMNEW,NEW_TT_LABEL);
	addPixbuf(PREFSOPEN,"O",ITEMOPEN,OPEN_TT_LABEL);
	addPixbuf(PREFSSAVE,"S",ITEMSAVE,SAVE_TT_LABEL);
	addPixbuf(PREFSCUT,"X",ITEMCUT,CUT_TT_LABEL);
	addPixbuf(PREFSCOPY,"C",ITEMCOPY,COPY_TT_LABEL);
	addPixbuf(PREFSPASTE,"P",ITEMPASTE,PASTE_TT_LABEL);
	addPixbuf(PREFSUNDO,"U",ITEMUNDO,UNDO_TT_LABEL);
	addPixbuf(PREFSREDO,"R",ITEMREDO,REDO_TT_LABEL);
	addPixbuf(PREFSFIND,"F",ITEMFIND,FIND_TT_LABEL);
	addPixbuf(PREFSQUESTION,"G",ITEMGOTO,GOTO_DEFINE_TT_LABEL);
	addPixbuf(PREFSBACK,"B",ITEMBACK,BACK_TT_LABEL);
	addPixbuf(PREFSFORWARD,"W",ITEMFORWARD,FORWARD_TT_LABEL);
	addPixbuf(PREFSLINENUM,"9",ITEMLINE,TO_LINE_TT_LABEL);
	addPixbuf(PREFSAPI,"A",ITEMAPI,TO_API_TT_LABEL);
	addPixbuf(PREFSQTAPI,"Q",ITEMQTAPI,TO_QTAPI_TT_LABEL);
	addPixbuf(PREFSFINDDEF,"D",ITEMDEFINE,TO_DEFINE_TT_LABEL);
	addPixbuf(PREFSLIVE,"L",ITEMLIVE,LIVE_TT_LABEL);
	addPixbuf(PREFSSEPERATOR,"s",ITEMSEPERATOR,SEPERATOR_TT_LABEL);
	addPixbuf(PREFSEXPAND,"E",ITEMEXPANDER,EXPANDER_TT_LABEL);
	ERRDATA
}

char *makeToolBarList(void)
{
	ERRDATA
	GtkTreeIter iter;
	gboolean	valid;
	gchar		*str_data=NULL;
	GString	*str=g_string_new("");

	valid=gtk_tree_model_get_iter_first((GtkTreeModel *)listStore,&iter);
	while(valid)
		{
			gtk_tree_model_get((GtkTreeModel *)listStore,&iter,TEXT_COLUMN,&str_data,-1);
			g_string_append_c(str,str_data[0]);
			valid=gtk_tree_model_iter_next((GtkTreeModel *)listStore,&iter);
			ERRDATA debugFree(&str_data);
		}
	ERRDATA return(g_string_free(str,false));
}

bool clickIt(GtkWidget *widget,GdkEvent *event,gpointer data)
{
	ERRDATA
	GtkTreePath	*path=NULL;
	GtkTreeIter		iter;
	int				button;
	int				state=event->button.state;

	path=gtk_icon_view_get_path_at_pos((GtkIconView *)widget,event->button.x,event->button.y);

	if((GDK_CONTROL_MASK & state) &&(path!=NULL))
		{
			gtk_tree_model_get_iter((GtkTreeModel*)listStore,&iter,path);
			gtk_tree_model_get((GtkTreeModel*)listStore,&iter,BUTTON_NUM,&button,-1);
			gtk_widget_set_sensitive((GtkWidget*)tool[button],true);
			gtk_list_store_remove((GtkListStore*)listStore,&iter);
		}
	ERRDATA return(false);
}

void doIconView(void)
{
	ERRDATA
	GtkCellRenderer *renderer;

	listStore=gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_INT);
	iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(iconView),GTK_TREE_MODEL(listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(iconView),0);
	
	populateStore();

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(iconView),GTK_SELECTION_SINGLE);
	gtk_icon_view_set_item_orientation(GTK_ICON_VIEW(iconView),GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(iconView),24);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(iconView),TRUE);

	renderer=gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconView),renderer,false);

	renderer=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconView),renderer,false);

	gtk_box_pack_start(GTK_BOX(iconViewBox),(GtkWidget*)iconView,true,true,2);
	g_signal_connect(G_OBJECT(iconView),"button-press-event",G_CALLBACK(clickIt),NULL);
	ERRDATA
}

void setKeyCuts(GtkWidget *widget,gpointer data)
{
	ERRDATA

	char	*filename;
	const char		*text;

	if(strcasecmp(gtk_widget_get_name(widget),"cancel")==0)
		gtk_widget_hide(keysWindow);

	if(strcasecmp(gtk_widget_get_name(widget),"apply")==0)
		{
			for(int j=0;j<NUMSHORTCUTS;j++)
				{
					text=gtk_entry_get_text((GtkEntry*)entries[j]);
					shortCuts[j][0]=gdk_keyval_from_name(text);
					shortCuts[j][1]=j;
					if(shortCutStrings[j]!=NULL)
						{
							ERRDATA debugFree(&shortCutStrings[j]);
						}
					sinkReturn=asprintf(&shortCutStrings[j],"%i %i - ^%c %s",shortCuts[j][0],shortCuts[j][1],shortCuts[j][0],shortcuttext[j]);
				}
			sinkReturn=asprintf(&filename,"%s/%s/keybindings.rc",getenv("HOME"),APPFOLDENAME);
			saveVarsToFile(filename,keybindings_rc);
			gtk_widget_hide(keysWindow);
		}
	ERRDATA
}

gboolean setKeyInEntry(GtkEntry *widget,GdkEventKey *event,gpointer data)
{
	ERRDATA
	if((event->type==GDK_KEY_PRESS) &&(event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			ERRDATA return(true);
		}

	if((event->type==GDK_KEY_PRESS)&&(event->state & GDK_CONTROL_MASK))
		gtk_entry_set_text(widget,gdk_keyval_name(event->keyval));

	ERRDATA return(true);
}

void buildKeys()
{
	ERRDATA
	GtkWidget	*vbox;
	GtkWidget	*item;
	GtkWidget	*hbox;
	char		keystring[32];
	int			loop;
	char		*keycutsinfo;

	if(keysWindow==NULL)
		{
			keysWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
			gtk_window_set_title((GtkWindow*)keysWindow,KBSC_DEFINE_KB_LABEL);
			vbox=createNewBox(NEWVBOX,false,8);

			sinkReturn=asprintf(&keycutsinfo,"%s",KBSC_INFO_LABEL);
			item=gtk_label_new(keycutsinfo);
			gtk_label_set_justify((GtkLabel*)item,GTK_JUSTIFY_CENTER);
			gtk_label_set_line_wrap((GtkLabel*)item,true);
			gtk_box_pack_start(GTK_BOX(vbox),item,false,false,0);
			ERRDATA debugFree(&keycutsinfo);
//functions
			for(loop=0;loop<NUMSHORTCUTS;loop++)
				{
					hbox=createNewBox(NEWHBOX,true,0);
					gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext(shortcuttext[loop])),true,true,0);
					entries[loop]=gtk_entry_new();
					g_signal_connect(G_OBJECT(entries[loop]),"key-press-event",G_CALLBACK(setKeyInEntry),NULL);
					gtk_box_pack_start(GTK_BOX(hbox),entries[loop],true,true,0);
					gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
				}
//buttons
#ifdef _USEGTK3_
			gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,0);
#else
			gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
#endif

			hbox=createNewBox(NEWHBOX,true,4);
			item=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);
			gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
			gtk_widget_set_name(item,"apply");
			g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setKeyCuts),(void*)item);	

			item=createNewStockButton(GTK_STOCK_CANCEL,GTK_STOCK_CANCEL);
			gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
			gtk_widget_set_name(item,"cancel");
			g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setKeyCuts),(void*)item);
			gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);

//show it
			gtk_container_add(GTK_CONTAINER(keysWindow),(GtkWidget*)vbox);
		}

	for(int j=0;j<NUMSHORTCUTS;j++)
		{
			if(gdk_keyval_name(shortCuts[j][0])!=NULL)
				{
					sprintf(&keystring[0],"%s",gdk_keyval_name(shortCuts[j][0]));
					gtk_entry_set_text((GtkEntry*)entries[j],(char*)&keystring);
				}
		}
	gtk_widget_show_all(keysWindow);
	ERRDATA
}

void makePrefsDial(int widgnum,const char *label,const char *name,int value,int minvalue,int maxvalue)
{
	ERRDATA
	GtkWidget	*item;

	prefsIntWidgets[widgnum]=(GObject*)gtk_adjustment_new(value,minvalue,maxvalue,1,1,0);
	item=gtk_spin_button_new((GtkAdjustment*)prefsIntWidgets[widgnum],1,0);
	gtk_widget_set_name(item,name);
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(label));
	gtk_box_pack_start((GtkBox*)prefHBox2,item,true,true,0);
	gtk_box_pack_start((GtkBox*)pagevbox,prefHBox,false,false,0);
	ERRDATA
}

void makePrefsCheck(int widgnum,const char *label,const char *name,bool onoff,bool left,bool autoadd)
{
	ERRDATA
	prefsWidgets[widgnum]=gtk_check_button_new_with_label(label);
	gtk_widget_set_name(prefsWidgets[widgnum],name);
	gtk_toggle_button_set_active((GtkToggleButton*)prefsWidgets[widgnum],onoff);
	if(left==true)
		gtk_container_add(GTK_CONTAINER(prefHBox1),prefsWidgets[widgnum]);
	else
		gtk_container_add(GTK_CONTAINER(prefHBox2),prefsWidgets[widgnum]);
	if(autoadd==true)
		gtk_box_pack_start((GtkBox*)pagevbox,prefHBox,false,false,0);
	ERRDATA
}

void makePrefBox(bool box1,bool box2)
{
	prefHBox=createNewBox(NEWHBOX,true,0);
#ifdef _USEGTK3_
	prefHBox1=createNewBox(NEWHBOX,true,0);
	prefHBox2=createNewBox(NEWHBOX,true,0);
	if(box1==true)
		gtk_widget_set_halign(prefHBox1,GTK_ALIGN_START);
	if(box2==true)
		gtk_widget_set_halign(prefHBox2,GTK_ALIGN_START);
#else
	if(box1==true)
		prefHBox1=(GtkWidget*)gtk_alignment_new(0,0.5,0,0);
	else
		prefHBox1=createNewBox(NEWHBOX,true,0);
	if(box2==true)
		prefHBox2=(GtkWidget*)gtk_alignment_new(0,0.5,0,0);
	else
		prefHBox2=createNewBox(NEWHBOX,true,0);
#endif
	gtk_box_pack_start((GtkBox*)prefHBox,prefHBox1,true,true,4);
	gtk_box_pack_start((GtkBox*)prefHBox,prefHBox2,true,true,4);	
}

VISIBLE void doPrefs(GtkWidget *widget,gpointer data)
{
	ERRDATA
	GtkWidget		*vbox;
	GtkWidget		*hbox;
	GtkWidget		*item;
	GtkWidget		*label;
	GtkNotebook	*prefsnotebook;

	prefswin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)prefswin,"Preferences");
	vbox=createNewBox(NEWVBOX,false,8);
	hbox=createNewBox(NEWHBOX,false,8);

//toolbar dnd
	iconViewBox=createNewBox(NEWHBOX,false,0);
	fromHBox=createNewBox(NEWHBOX,false,0);

	label=gtk_label_new(PREFS_CUSTOMIZE_LABEL);
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),iconViewBox,true,true,2);
	gtk_box_pack_start(GTK_BOX(vbox),fromHBox,true,true,2);

	populateDnD();
	doIconView();

//end customize
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,0);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
#endif

	prefsnotebook=(GtkNotebook*)gtk_notebook_new();

//pages
//page1
	pagevbox=createNewBox(NEWVBOX,false,0);
//appearence 1
//indent
	makePrefBox(true,true);
	makePrefsCheck(AUTOINDENT,PREFS_AUTO_INDENT_LABEL,"indent",indent,true,true);
//linenumbers
	makePrefBox(true,true);
	makePrefsCheck(SHOWNUMS,PREFS_SHOWLINE_NUMBERS_LABEL,"show",lineNumbers,true,true);
//wraplines
	makePrefBox(true,true);
	makePrefsCheck(WRAP,PREFS_WRAP_LINES_LABEL,"wrap",lineWrap,true,true);
//highlite
	makePrefBox(true,true);
	makePrefsCheck(HIGHLIGHT,PREFS_HIGHLIGHT_LINE_LABEL,"high",highLight,true,true);
//no syntax colour
	makePrefBox(true,true);
	makePrefsCheck(NOSYNTAX,PREFS_NO_SYNTAX_LABEL,"nosyntax",noSyntax,true,true);
//show whitespace
	makePrefBox(true,true);
	makePrefsCheck(SHOWWHITESPACE,PREFS_SHOW_WHITESPACE_LABEL,"showspaces",showWhiteSpace,true,true);

//single instance
	makePrefBox(true,true);
	makePrefsCheck(USESINGLE,PREFS_USE_SINGLE_LABEL,"single",singleUse,true,true);

//auto save session
	makePrefBox(true,true);
	makePrefsCheck(AUTOSAVE,PREFS_AUTO_MENU_SAVE_LABEL,"save",onExitSaveSession,true,false);
	g_signal_connect(G_OBJECT(prefsWidgets[AUTOSAVE]),"toggled",G_CALLBACK(setPrefs),(void*)prefsWidgets[AUTOSAVE]);
//auto restore bookmarks
	makePrefsCheck(AUTOBM,PREFS_RESTORE_SESSION_LABEL,"marks",restoreBookmarks,false,true);
	gtk_widget_set_sensitive(prefsWidgets[AUTOBM],onExitSaveSession);

//no duplicates
	makePrefBox(true,true);
	makePrefsCheck(NODUPLICATE,PREFS_NO_DUPLICATE_LABEL,"duplicates",noDuplicates,true,true);
//turn off warnings
	makePrefBox(true,true);
	makePrefsCheck(NOWARN,PREFS_NO_WARN_LABEL,"warning",noWarnings,true,true);
//do readlink
	makePrefBox(true,true);
	makePrefsCheck(READLINK,PREFS_READ_LINK_LABEL,"readlink",readLinkFirst,true,true);
//autoshow completion
	makePrefBox(true,true);
	makePrefsCheck(AUTOSHOW,PREFS_AUTO_COMPLETE_LABEL,"autocomp",autoShowComps,true,true);
//auto search
	makePrefBox(true,true);
	makePrefsCheck(AUTOSEARCH,PREFS_AUTO_SEARCH_LABEL,"autosearchdocs",autoSearchDocs,true,true);

//show menu icons
	makePrefBox(true,true);
	makePrefsCheck(SHOWMENUICONS,PREFS_SHOW_MENU_ICONS_LABEL,"showmenuicons",showMenuIcons,true,true);
#ifdef _USEGTK3_
	gtk_widget_set_sensitive(prefsWidgets[SHOWMENUICONS],true);
#else
	gtk_toggle_button_set_active((GtkToggleButton*)prefsWidgets[SHOWMENUICONS],false);
	gtk_widget_set_sensitive(prefsWidgets[SHOWMENUICONS],false);
#endif

	gtk_notebook_append_page(prefsnotebook,pagevbox,gtk_label_new(PREFS_GEN_APPEARANCE_LABEL));
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)prefsnotebook,true,true,0);

//page2
	pagevbox=createNewBox(NEWVBOX,false,0);
//text appearence
//tabwidth
	makePrefBox(true,false);
	makePrefsDial(TABWIDTH,PREFS_TAB_WIDTH_LABEL,"tabs",tabWidth,2,64);

//style
	int cnt=0;
	int foundname=0;
	const gchar * const * ids=gtk_source_style_scheme_manager_get_scheme_ids(schemeManager);
	makePrefBox(true,false);
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(PREFS_THEME_LABEL));
	item=gtk_combo_box_text_new();
	gtk_widget_set_name(item,"style");
	g_signal_connect(G_OBJECT(item),"changed",G_CALLBACK(setPrefs),(void*)item);
	
	while(ids[cnt]!=NULL)
		{
			gtk_combo_box_text_append_text((GtkComboBoxText*)item,ids[cnt]);
			if(strcmp(ids[cnt],styleName)==0)
				foundname=cnt;
			cnt++;
		}
	gtk_combo_box_set_active((GtkComboBox*)item,foundname);
	gtk_box_pack_start((GtkBox*)prefHBox2,item,true,true,0);
	gtk_box_pack_start((GtkBox*)pagevbox,prefHBox,false,false,0);

//font button
	makePrefBox(true,false);
	fontButton=gtk_font_button_new_with_font(fontAndSize);
	gtk_widget_set_name(fontButton,"fontbutton");
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(PREFS_FONT_LABEL));
	gtk_box_pack_start((GtkBox*)prefHBox2,fontButton,false,true,0);
	gtk_box_pack_start((GtkBox*)pagevbox,prefHBox,false,false,0);

////bm highlight colour
	makePrefBox(true,false);
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(PREFS_BM_COLOUR_LABEL));

#ifdef _USEGTK3_
	GdkRGBA color;
	gdk_rgba_parse(&color,(const gchar *)highlightColour);
	bmHighlightBox=gtk_color_button_new_with_rgba(&color);
#else
	GdkColor color;
	gdk_color_parse((const gchar *)highlightColour,&color);
	bmHighlightBox=gtk_color_button_new_with_color(&color);
#endif
	gtk_box_pack_start((GtkBox*)prefHBox2,bmHighlightBox,false,true,0);
	gtk_box_pack_start((GtkBox*)pagevbox,prefHBox,false,false,0);

//autoshow completion
	makePrefBox(true,false);
	makePrefsDial(COMPLETIONSIZE,PREFS_COMPLETION_SIZE_LABEL,"minautochars",autoShowMinChars,2,20);

//notebook tabs size
	makePrefBox(true,false);
	makePrefsDial(NOTETABSSIZE,PREFS_NBTABS_SIZE_LABEL,"nbtabssize",tabsSize,0,20);

//sort functions
	funcListDrop=gtk_combo_box_text_new();
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,PREFS_FUNC_TYPE_ALPHA_LABEL);
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,PREFS_FUNC_TYPE_FILE_LABEL);
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,PREFS_FUNC_FILE_LABEL);
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,PREFS_FUNC_ALPHA_LABEL);
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,PREFS_FUNC_CAT_LABEL);

	gtk_combo_box_set_active((GtkComboBox*)funcListDrop,listFunction);
	hbox=createNewBox(NEWHBOX,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),(GtkWidget*)funcListDrop,false,false,0);
	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)hbox,false,false,8);

//show keybindings dialog
	hbox=createNewBox(NEWHBOX,true,0);
	item=gtk_button_new_with_label(PREFS_CUSTOM_KBSC_LABEL);
	gtk_widget_set_name(item,"makekeys");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(buildKeys),NULL);	
	gtk_box_pack_start(GTK_BOX(hbox),(GtkWidget*)item,false,false,0);
	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)hbox,false,false,8);

//end style
	gtk_notebook_append_page(prefsnotebook,pagevbox,gtk_label_new(PREFS_TEXT_STYLE_LABEL));

//page 3
	pagevbox=createNewBox(NEWVBOX,false,0);
//admin
//function search depth
	makePrefBox(true,false);
	makePrefsDial(MAXFUNCDEPTH,PREFS_TAG_SEARCH_DEPTH_LABEL,"depth",depth,0,20);

//terminalcommand
	makePrefBox(true,false);
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(PREFS_TERM_COMMAND_LABEL));
	terminalBox=gtk_entry_new();
	if(terminalCommand!=NULL)
		gtk_entry_set_text((GtkEntry*)terminalBox,terminalCommand);
	gtk_container_add(GTK_CONTAINER(prefHBox2),terminalBox);
	gtk_container_add(GTK_CONTAINER(pagevbox),prefHBox);

//root command
	makePrefBox(true,false);
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(PREFS_AS_ROOT_COMMAND_LABEL));
	rootCommandBox=gtk_entry_new();
	if(rootCommand!=NULL)
		gtk_entry_set_text((GtkEntry*)rootCommandBox,rootCommand);
	gtk_container_add(GTK_CONTAINER(prefHBox2),rootCommandBox);
	gtk_container_add(GTK_CONTAINER(pagevbox),prefHBox);

//set default browser
	makePrefBox(true,false);
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(PREFS_BROWSER_LABEL));
	defaultBrowserBox=gtk_entry_new();
	if(browserCommand!=NULL)
		gtk_entry_set_text((GtkEntry*)defaultBrowserBox,browserCommand);
	gtk_container_add(GTK_CONTAINER(prefHBox2),defaultBrowserBox);
	gtk_container_add(GTK_CONTAINER(pagevbox),prefHBox);

//find replace history max
	makePrefBox(true,false);
	makePrefsDial(MAXHISTORY,PREFS_MAX_FIND_HISTORY_LABEL,"maxfrhistory",maxFRHistory,0,MAXTEXTWIDTH);
//max tab label width
	makePrefBox(true,false);
	makePrefsDial(MAXTABCHARS,PREFS_MAX_CHARS_IN_TAB_LABEL,"maxtabchars",maxTabChars,5,MAXTEXTWIDTH);
//max function strings
	makePrefBox(true,false);
	makePrefsDial(MENUWIDTH,PREFS_MAX_CHARS_IN_FUNC_DEFS_LABEL,"maxmenuchars",maxFuncDefs,5,MAXTEXTWIDTH);
//max bookmark strings
	makePrefBox(true,false);
	makePrefsDial(MAXBMWIDTH,PREFS_MAX_CHARS_IN_BMS_LABEL,"maxbmchars",maxBMChars,5,MAXTEXTWIDTH);
//max history
	makePrefBox(true,false);
	makePrefsDial(MAXJUMPHISTORY,PREFS_MAX_JUMP_HISTORY_LABEL,"maxjumphistory",maxJumpHistory,10,MAXBFHISTORY);

//check for update
	makePrefBox(true,true);
	makePrefsCheck(UPDATECHECK,PREFS_UPDATE_CHECK_LABEL,"updatecheck",autoCheck,true,true);
//use global plug menu
	makePrefBox(true,true);
	makePrefsCheck(GLOBALPLUGMENU,PREFS_GLOBAL_PLUG_MENU_LABEL,"useplugmenu",useGlobalPlugMenu,true,true);

	gtk_notebook_append_page(prefsnotebook,pagevbox,gtk_label_new(PREFS_ADMIN_LABEL));
//end admin
//nag
	label=gtk_label_new(PREFS_BE_KIND_LABEL);
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);

	prefsWidgets[BEKIND]=gtk_check_button_new_with_label(PREFS_I_DONATED_LABEL);
	gtk_widget_set_name(prefsWidgets[BEKIND],"useplugmenu");
	gtk_toggle_button_set_active((GtkToggleButton*)prefsWidgets[BEKIND],nagScreen);

	hbox=createNewBox(NEWHBOX,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),(GtkWidget*)prefsWidgets[BEKIND],false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)hbox,false,false,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new(PREFS_KARMA_LABEL),false,false,0);

//buttons
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,0);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
#endif

	hbox=createNewBox(NEWHBOX,true,4);
	item=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);

	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"apply");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);	

	item=createNewStockButton(GTK_STOCK_CANCEL,GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"cancel");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);

//show it
	gtk_container_add(GTK_CONTAINER(prefswin),(GtkWidget*)vbox);
	gtk_widget_show_all(prefswin);
	ERRDATA
}

//TODO//
//not working properly
void addRecentToMenu(GtkRecentChooser *chooser,GtkWidget *menu)
{
	ERRDATA
	GList		*itemlist=NULL;
	GList		*l=NULL;
	GtkWidget	*menuitem;
	const char	*uri=NULL;
	char		*filename=NULL;
	int			i=0;

	itemlist=gtk_recent_chooser_get_items(chooser);

	for(l=itemlist;l !=NULL;l=l->next)
		{
			const gchar *menuname;
			GtkRecentInfo *info =(GtkRecentInfo*)l->data;
			if(i >= MAXRECENT)
				break;
			i++;

			menuname=gtk_recent_info_get_display_name(info);
			uri=gtk_recent_info_get_uri(info);
			menuData[MENUBLANK].cb=(void*)recentFileMenu;
			menuData[MENUBLANK].stockID=NULL;
			menuData[MENUBLANK].key=0;
			if(uri!=NULL)
				{
					filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
					menuData[MENUBLANK].userData=(void*)filename;
					menuData[MENUBLANK].menuLabel=menuname;
					menuData[MENUBLANK].widgetName=menuname;
					menuitem=newMenuItem(MENUBLANK,menu);
				}
		}
	ERRDATA
}

void buildMenus(void)
{
	GtkWidget		*menuitem;
	GtkWidget		*menu;
	GtkWidget		*menurecent;
	GtkWidget		*plugsubmenu=NULL;

	menuBar=gtk_menu_bar_new();

//menus
//file menu
	fileMenu=gtk_menu_item_new_with_label(MENU_FILE_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)fileMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenu),menu);
//new
	menuItemNew=newMenuItem(MENUNEW,menu);
//open
	menuItemOpen=newMenuItem(MENUOPEN,menu);
//open as hexdump
	menuitem=newImageMenuItem(MENUOPENHEX,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//extras
	menuitem=newPixmapMenuItem(MENUNEWADMIN,menu);
	menuitem=newPixmapMenuItem(MENUNEWED,menu);
	if(gotManEditor==0)
		menuitem=newPixmapMenuItem(MENUMANED,menu);

//doxy
	if(gotDoxygen==0)
		menuitem=newImageMenuItem(MENUDOXY,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//recent menu
	menuitem=gtk_menu_item_new_with_mnemonic(gettext(menuData[MENURECENT].menuLabel));
	menurecent=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),menurecent);
	addRecentToMenu((GtkRecentChooser*)recent,menurecent);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_widget_set_name(menuitem,menuData[MENURECENT].widgetName);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save
	saveMenu=newMenuItem(MENUSAVE,menu);
//savas
	saveAsMenu=newMenuItem(MENUSAVEAS,menu);
//save all
	saveAllMenu=newImageMenuItem(MENUSAVEALL,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save session
	menuitem=newImageMenuItem(MENUSAVESESSION,menu);
//restore session
	menuitem=newImageMenuItem(MENURESTORESESSION,menu);
//printfile
	printMenu=newMenuItem(MENUPRINT,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//close
	closeMenu=newMenuItem(MENUCLOSE,menu);
//close-all
	closeAllMenu=newImageMenuItem(MENUCLOSEALL,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//reload file
	revertMenu=newMenuItem(MENUREVERT,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//quit
	menuitem=newMenuItem(MENUQUIT,menu);
//edit menu
	editMenu=gtk_menu_item_new_with_label(MENU_EDIT_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)editMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMenu),menu);
//undo
	undoMenu=newMenuItem(MENUUNDO,menu);
//redo
	redoMenu=newMenuItem(MENUREDO,menu);
//undoall
	undoAllMenu=newImageMenuItem(MENUUNDOALL,menu);
//redoall
	redoAllMenu=newImageMenuItem(MENUREDOALL,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//cut
	cutMenu=newMenuItem(MENUCUT,menu);
//copy
	copyMenu=newMenuItem(MENUCOPY,menu);
//paste
	pasteMenu=newMenuItem(MENUPASTE,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//find
	menuitem=newMenuItem(MENUFIND,menu);
//find next
	menuitem=newImageMenuItem(MENUNEXT,menu);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//sort
	sortTabsMenu=newImageMenuItem(MENUSORTTABS,menu);
//show all tabs
	showAllTabsMenu=newImageMenuItem(MENUSHOWTABS,menu);
//jump to tab
	viewTabMenu=newImageMenuItem(MENUSELECTTAB,menu);
	rebuildTabsMenu();

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//prefs
	menuitem=newImageMenuItem(MENUPREFS,menu);
//plugs
	menuitem=newImageMenuItem(MENUPLUGPREFS,menu);

//view menu
	viewMenu=gtk_menu_item_new_with_label(MENU_VIEW_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)viewMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewMenu),menu);

//show docs
	menuitem=newMenuItem(MENUSHOWDOCS,menu);
//toggle bookmark bar
	if(showBMBar)
		menuitem=newMenuItem(MENUHIDEBMBAR,menu);
	else
		menuitem=newMenuItem(MENUSHOWBMBAR,menu);
//toggle toolbar bar
	if(showToolBar)
		menuitem=newMenuItem(MENUHIDETOOLBAR,menu);
	else
		menuitem=newMenuItem(MENUSHOWTOOLBAR,menu);
//tooloutput
	if(showToolOutWin)
		toolOutMenu=newMenuItem(MENUHIDETOOLOUT,menu);
	else
		toolOutMenu=newMenuItem(MENUSHOWTOOLOUT,menu);
//toggle statusbar
	if(showStatus)
		statusBarMenu=newMenuItem(MENUHIDESTATUS,menu);
	else
		statusBarMenu=newMenuItem(MENUSHOWSTATUS,menu);

#ifdef _BUILDDOCVIEWER_
//toggle docviewer
	showDocViewWidget=newMenuItem(MENUSHOWVIEWER,menu);
#endif

	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
//toggle line nubers
		menuitem=newCheckMenuItem(MENUTOGGLENUMBERS,menu,lineNumbers);
//toggle wrap lines
	menuitem=newCheckMenuItem(MENUWRAPLINES,menu,lineWrap);
//toggle higlight current line
	menuitem=newCheckMenuItem(MENUHILITELINE,menu,highLight);
//toggle syntax highlight
	menuitem=newCheckMenuItem(MENUSYNTAXHILITE,menu,noSyntax);
//toggle show spaces
	menuitem=newCheckMenuItem(MENUSHOWSPACE,menu,showWhiteSpace);
//toggle auto show comps
	menuitem=newCheckMenuItem(MENUSHOWCOMPS,menu,autoShowComps);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
//navigation menu
	navMenu=gtk_menu_item_new_with_label(MENU_NAV_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)navMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(navMenu),menu);

//goto define
	gotoDefMenu=newImageMenuItem(MENUGOTODEF,menu);
//open include
	menuitem=newImageMenuItem(MENUOPENINC,menu);
//goto line number
	menuitem=newImageMenuItem(MENUGOTOLINE,menu);
//find define
	menuitem=newImageMenuItem(MENUSEARCHDEF,menu);
//find gtkdoc
	searchInGtkDocsMenu=newImageMenuItem(MENUSEARCHGTK,menu);
//find qt5
	searchInQTDocsMenu=newImageMenuItem(MENUSEARCHQT,menu);
//goto doxy docs
	if(gotDoxygen==0)
		searchInDocsMenu=newImageMenuItem(MENUSEARCHDOXY,menu);
//go back
	goBackMenu=newMenuItem(MENUBACK,menu);
//go foward
	goForwardMenu=newMenuItem(MENUFORWARD,menu);
//continue
	continueMenu=newImageMenuItem(MENUCONTINUE,menu);
	gtk_widget_hide(continueMenu);

//function menu
	funcMenu=gtk_menu_item_new_with_label(MENU_FUNC_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)funcMenu,true);

//newbookmarks
	bookMarkMenu=gtk_menu_item_new_with_label(MENU_BM_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)bookMarkMenu,true);
	rebuildBookMarkMenu();

//external tools
	toolsMenu=gtk_menu_item_new_with_label(MENU_TOOLS_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)toolsMenu,true);
	buildTools();

//help
	helpMenu=gtk_menu_item_new_with_label(MENU_HELP_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)helpMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMenu),menu);

//about
	menuitem=newMenuItem(MENUABOUT,menu);
//help local
	menuitem=newMenuItem(MENULOCALHELP,menu);
//help online
	menuitem=newImageMenuItem(MENUONLINE,menu);
//get plugins
	menuitem=newPixmapMenuItem(MENUGETPLUGS,menu);

	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),fileMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),editMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),viewMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),navMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),funcMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),bookMarkMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),toolsMenu);

//global plug menu
	if(useGlobalPlugMenu==true)
		{
			globalPlugMenu=gtk_menu_item_new_with_label(MENU_GET_PLUGS_MENU_LABEL);
			gtk_menu_item_set_use_underline((GtkMenuItem*)globalPlugMenu,true);
			plugsubmenu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(globalPlugMenu),plugsubmenu);
			globalPlugins->globalPlugData->mlist.menuBar=plugsubmenu;
			gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),globalPlugMenu);
		}
	else
		{
			globalPlugMenu=NULL;
			globalPlugins->globalPlugData->mlist.menuBar=menuBar;
		}

	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),helpMenu);
}

void buildMainGui(void)
{
	ERRDATA
	GtkWidget		*menuitem;
	GtkWidget		*menu;
	GtkWidget		*menurecent;
	GtkWidget		*plugsubmenu=NULL;

	mainWindowVBox=createNewBox(NEWVBOX,false,0);
	mainTopUserVBox=createNewBox(NEWVBOX,false,0);
	mainLeftUserVBox=createNewBox(NEWVBOX,false,0);
	mainNotebookVBox=createNewBox(NEWVBOX,false,0);
	mainRightUserVBox=createNewBox(NEWVBOX,false,0);
	mainBottomUserVBox=createNewBox(NEWVBOX,false,0);

	mainWindowHBox=createNewBox(NEWHBOX,false,0);

#ifdef _USEGTK3_
	mainWindowHPane=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	secondWindowHPane=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
#else
	mainWindowHPane=gtk_hpaned_new();
	secondWindowHPane=gtk_hpaned_new();
#endif

	mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)mainWindow,windowWidth,windowHeight);
	if(windowX!=-1 && windowY!=-1)
		gtk_window_move((GtkWindow *)mainWindow,windowX,windowY);

//TODO//?
	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(gtk_widget_hide_on_delete),NULL);
	//g_signal_connect_after(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(doShutdown),NULL);
	g_signal_connect_after(G_OBJECT(mainWindow),"unmap",G_CALLBACK(doShutdown),NULL);
	g_signal_connect(G_OBJECT(mainWindow),"key-press-event",G_CALLBACK(keyShortCut),NULL);

	accgroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)mainWindow,accgroup);

	mainNotebook=(GtkNotebook*)gtk_notebook_new();
	gtk_notebook_set_scrollable(mainNotebook,true);
	g_signal_connect(G_OBJECT(mainNotebook),"switch-page",G_CALLBACK(switchPage),NULL);
	g_signal_connect(G_OBJECT(mainNotebook),"page-reordered",G_CALLBACK(switchPage),NULL);
	g_signal_connect(G_OBJECT(mainNotebook),"page-removed",G_CALLBACK(realCloseTab),NULL);

	globalHistory=new HistoryClass(mainNotebook,maxJumpHistory);

#ifdef _USEGTK3_
	char	*notebookcss=NULL;
	GtkStyleProvider	*nbprovider;

	nbprovider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	sinkReturn=asprintf(&notebookcss,"GtkNotebook {\n \
    padding: 0px;\n \
}\n");

	gtk_css_provider_load_from_data((GtkCssProvider*)nbprovider,notebookcss,-1,NULL);
	applyCSS((GtkWidget*)mainNotebook,nbprovider);
	gtk_style_context_reset_widgets(gdk_screen_get_default());
	debugFree(&notebookcss);
#endif

	toolBarBox=createNewBox(NEWHBOX,true,0);
	toolBar=(GtkToolbar*)gtk_toolbar_new();

//dnd
	gtk_drag_dest_set(mainWindowVBox,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(mainWindowVBox);
	g_signal_connect(G_OBJECT(mainWindowVBox),"drag_data_received",G_CALLBACK(dropUri),NULL);

	setUpToolBar();
	gtk_box_pack_start(GTK_BOX(toolBarBox),(GtkWidget*)toolBar,true,true,0);

	buildMenus();

//tooloutputwindow
#ifdef _USEGTK3_
	mainVPane=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
#else
	mainVPane=gtk_vpaned_new();
#endif
	gtk_container_set_border_width(GTK_CONTAINER(mainVPane),0);
	gtk_paned_add1(GTK_PANED(mainVPane),mainNotebookVBox);

	toolOutVBox=createNewBox(NEWVBOX,false,0);

	gtk_paned_add2(GTK_PANED(mainVPane),toolOutVBox);
	mainWindowScrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)mainWindowScrollbox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	toolOutputBuffer=gtk_text_buffer_new(NULL);
	toolOutputView=gtk_text_view_new_with_buffer(toolOutputBuffer);

	gtk_container_add(GTK_CONTAINER(mainWindowScrollbox),(GtkWidget*)toolOutputView);
	gtk_box_pack_start((GtkBox*)toolOutVBox,mainWindowScrollbox,true,true,0);

//add main vbox to mainWindow
	gtk_container_add((GtkContainer*)mainWindow,mainWindowVBox);
//addmenubar
	gtk_box_pack_start((GtkBox*)mainWindowVBox,menuBar,false,false,0);
//add toolbar
	gtk_box_pack_start((GtkBox*)mainWindowVBox,toolBarBox,false,false,0);

//add left user
	gtk_paned_pack1((GtkPaned*)mainWindowHPane,mainLeftUserVBox,false,true);
	gtk_paned_pack2((GtkPaned*)mainWindowHPane,secondWindowHPane,true,true);

//add mainNotebook
	gtk_box_pack_start((GtkBox*)mainNotebookVBox,(GtkWidget*)mainNotebook,true,true,0);
	gtk_box_pack_start((GtkBox*)mainWindowHBox,mainVPane,true,true,0);
	gtk_paned_pack1((GtkPaned*)secondWindowHPane,mainWindowHBox,true,false);
                                                         
//addright user
	gtk_paned_pack2((GtkPaned*)secondWindowHPane,mainRightUserVBox,false,true);

//vpanes top and bottom
#ifdef _USEGTK3_
	mainWindowVPane=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	secondWindowVPane=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
#else
	mainWindowVPane=gtk_vpaned_new();
	secondWindowVPane=gtk_vpaned_new();
#endif

	gtk_paned_pack1((GtkPaned *)mainWindowVPane,mainWindowHPane,false,false);
	gtk_paned_pack2((GtkPaned *)mainWindowVPane,mainBottomUserVBox,false,false);

	gtk_paned_pack1((GtkPaned *)secondWindowVPane,mainTopUserVBox,false,false);
	gtk_paned_pack2((GtkPaned *)secondWindowVPane,mainWindowVPane,false,false);

//add this to vpane and below
//add botom user
	gtk_widget_show_all(secondWindowVPane);
	gtk_box_pack_start((GtkBox*)mainWindowVBox,secondWindowVPane,true,true,0);

//add status bar
	statusWidget=gtk_statusbar_new();
	gtk_widget_show(statusWidget);
#ifdef _USEGTK3_
	char	*statuscss=NULL;
	GtkStyleProvider	*statusprovider;

	statusprovider=GTK_STYLE_PROVIDER(gtk_css_provider_new());
	sinkReturn=asprintf(&statuscss,"* {\n \
	font-size: 8px;\n \
}\n");

	gtk_css_provider_load_from_data((GtkCssProvider*)statusprovider,statuscss,-1,NULL);
	applyCSS((GtkWidget*)statusWidget,statusprovider);
	gtk_style_context_reset_widgets(gdk_screen_get_default());
	debugFree(&statuscss);

	gtk_widget_set_margin_top(GTK_WIDGET(statusWidget),0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(statusWidget),0);

#endif
	gtk_box_pack_end((GtkBox*)mainWindowVBox,statusWidget,false,true,0);

//hide top/bottom user boxes
	gtk_widget_hide(mainTopUserVBox);
	gtk_widget_hide(mainBottomUserVBox);
//hide left/right user boxes
	gtk_widget_hide(mainLeftUserVBox);
	gtk_widget_hide(mainRightUserVBox);

	globalPlugins->globalPlugData->mlist.menuFile=fileMenu;
	globalPlugins->globalPlugData->mlist.menuEdit=editMenu;
	globalPlugins->globalPlugData->mlist.menuFunc=funcMenu;
	globalPlugins->globalPlugData->mlist.menuNav=navMenu;
	globalPlugins->globalPlugData->mlist.menuTools=toolsMenu;
	globalPlugins->globalPlugData->mlist.menuHelp=helpMenu;
	globalPlugins->globalPlugData->mlist.menuBookMark=bookMarkMenu;
	globalPlugins->globalPlugData->mlist.menuView=viewMenu;

	globalPlugins->globalPlugData->topUserBox=mainTopUserVBox;
	globalPlugins->globalPlugData->leftUserBox=mainLeftUserVBox;
	globalPlugins->globalPlugData->rightUserBox=mainRightUserVBox;
	globalPlugins->globalPlugData->bottomUserBox=mainBottomUserVBox;
	globalPlugins->globalPlugData->mainWindow=mainWindow;
	globalPlugins->globalPlugData->notebook=mainNotebook;
	globalPlugins->globalPlugData->mainWindowHPane=mainWindowHPane;

	globalPlugins->globalPlugData->leftShow=0;
	globalPlugins->globalPlugData->rightShow=0;
	globalPlugins->globalPlugData->topShow=0;
	globalPlugins->globalPlugData->bottomShow=0;

	globalPlugins->globalPlugData->toolOutBuffer=toolOutputBuffer;
	globalPlugins->globalPlugData->toolOutWindow=toolOutputView;
	globalPlugins->globalPlugData->tabPopUpMenu=NULL;
	globalPlugins->globalPlugData->contextPopUpMenu=NULL;

//gettext
	globalPlugins->globalPlugData->locale=LOCALEDIR;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"addToGui");

	resetSensitive();
}

void buildFindReplace(void)
{
	ERRDATA
	GtkWidget	*content_area;
	GtkWidget	*replace;
#ifndef _USEGTK3_
	GtkWidget	*image;
#endif
	GtkWidget	*label;
	GtkWidget	*vbox;
	GtkWidget	*hbox;
	GtkWidget	*item;

#ifdef _USEGTK3_
	findReplaceDialog=gtk_dialog_new_with_buttons(FIND_FIND_REPLACE_LABEL,(GtkWindow*)mainWindow, GTK_DIALOG_DESTROY_WITH_PARENT,DIALOG_GO_FORWARD_LABEL,FINDNEXT,MENU_GO_BACK_LABEL,FINDPREV,FIND_REPLACE_LABEL,REPLACENEXT,NULL);
#else
	findReplaceDialog=gtk_dialog_new_with_buttons(FIND_FIND_REPLACE_LABEL,(GtkWindow*)mainWindow, GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_GO_FORWARD,FINDNEXT,GTK_STOCK_GO_BACK,FINDPREV,FIND_REPLACE_LABEL,REPLACENEXT,NULL);
#endif

	g_signal_connect(G_OBJECT(findReplaceDialog),"response",G_CALLBACK(doFindReplace),NULL);
	content_area=gtk_dialog_get_content_area(GTK_DIALOG(findReplaceDialog));
	gtk_dialog_set_default_response((GtkDialog*)findReplaceDialog,FINDNEXT);

	vbox=createNewBox(NEWVBOX,true,0);
	hbox=createNewBox(NEWHBOX,false,0);

	label=gtk_label_new(FIND_FIND_TEXT_LABEL);
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	findDropBox=gtk_combo_box_text_new_with_entry();
	findBox=gtk_bin_get_child((GtkBin*)findDropBox);
	gtk_entry_set_activates_default((GtkEntry*)findBox,true);

	gtk_container_add(GTK_CONTAINER(content_area),findDropBox);
	gtk_widget_show(findDropBox);

	label=gtk_label_new(FIND_REPLACE_TEXT_LABEL);
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	replaceDropBox=gtk_combo_box_text_new_with_entry();
	replaceBox=gtk_bin_get_child((GtkBin*)replaceDropBox);

	g_signal_connect_after(G_OBJECT(replaceDropBox),"changed",G_CALLBACK(pasteFRClip),replaceBox);
	gtk_container_add(GTK_CONTAINER(content_area),replaceDropBox);
	gtk_widget_show(replaceDropBox);

//line 1
	item=gtk_check_button_new_with_label(FIND_INSENSITIVE_LABEL);
	gtk_toggle_button_set_active((GtkToggleButton*)item,insensitiveSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)1);

	item=gtk_check_button_new_with_label(FIND_USE_REGEX_LABEL);
	gtk_toggle_button_set_active((GtkToggleButton*)item,useRegex);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)5);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_widget_show_all(vbox);
//line2
	hbox=createNewBox(NEWHBOX,false,0);

	item=gtk_check_button_new_with_label(FIND_WRAP_LABEL);
	gtk_toggle_button_set_active((GtkToggleButton*)item,wrapSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)2);

	item=gtk_check_button_new_with_label(FIND_ALL_FILES_LABEL);
	gtk_toggle_button_set_active((GtkToggleButton*)item,findInAllFiles);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)4);

	item=gtk_check_button_new_with_label(FIND_HIGHLIGHT_ALL_LABEL);
	gtk_toggle_button_set_active((GtkToggleButton*)item,hightlightAll);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)6);

	item=gtk_check_button_new_with_label(FIND_REPLACE_ALL_LABEL);
	gtk_toggle_button_set_active((GtkToggleButton*)item,replaceAll);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)3);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(content_area),vbox,true,true,0);

	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACENEXT);
#ifndef _USEGTK3_
	image=gtk_image_new_from_icon_name(GTK_STOCK_FIND_AND_REPLACE,GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image((GtkButton*)replace,image);
#endif	
	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACENEXT);
	if(replaceAll==true)
		gtk_button_set_label((GtkButton*)replace,FIND_REPLACE_ALL_MNEMONIC);
	else
		gtk_button_set_label((GtkButton*)replace,FIND_REPLACE_LABEL);

	gtk_widget_show(findBox);
	gtk_widget_show(replaceBox);
	gtk_widget_show(vbox);
	gtk_widget_show(hbox);

	g_signal_connect_object(G_OBJECT(findReplaceDialog),"delete_event",G_CALLBACK(gtk_widget_hide),G_OBJECT(findReplaceDialog),G_CONNECT_SWAPPED);
	g_signal_connect(G_OBJECT(findReplaceDialog),"delete_event",G_CALLBACK(gtk_true),NULL);
	ERRDATA
}

#ifdef _ASPELL_
void buildWordCheck(int documentCheck)
{
	ERRDATA
	GtkWidget	*vbox;
	GtkWidget	*button;
	GtkWidget	*hbox;
	GtkWidget	*label;
	GtkWidget	*image;
	char		*labeltext[512];
	int			docflag=documentCheck;

	spellCheckWord=gtk_dialog_new();
	gtk_window_set_title((GtkWindow*)spellCheckWord,SPELL_CHECK_WORD_LABEL);

	vbox=createNewBox(NEWVBOX,false,0);
	hbox=createNewBox(NEWHBOX,true,0);

	sprintf((char*)&labeltext,SPELL_CHANGE_TO_LABEL,badWord);
	label=gtk_label_new((char*)&labeltext);
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(hbox),label,true,true,0);

	wordListDropbox=gtk_combo_box_text_new_with_entry();

	gtk_box_pack_start(GTK_BOX(hbox),wordListDropbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,8);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,8);
#endif

	hbox=createNewBox(NEWHBOX,true,0);
	button=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doChangeWord),(gpointer)(long)docflag);

	button=gtk_button_new_with_label(SPELL_IGNORE_LABEL);
	image=gtk_image_new_from_icon_name(GTK_STOCK_ADD,GTK_ICON_SIZE_MENU);
	gtk_button_set_image((GtkButton*)button,image);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)1);

	button=createNewStockButton(GTK_STOCK_ADD,SPELL_ADD_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)2);

	button=createNewStockButton(GTK_STOCK_CANCEL,GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doCancelCheck),NULL);

#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,0);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
#endif
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	GtkWidget *content=gtk_dialog_get_content_area((GtkDialog *)spellCheckWord);
	gtk_container_add(GTK_CONTAINER(content),(GtkWidget*)vbox);

	g_signal_connect_object(G_OBJECT(spellCheckWord),"delete_event",G_CALLBACK(gtk_widget_hide),G_OBJECT(spellCheckWord),G_CONNECT_SWAPPED);
	g_signal_connect(G_OBJECT(spellCheckWord),"delete_event",G_CALLBACK(gtk_true),NULL);
	ERRDATA
}
#endif

int showFunctionEntry(void)
{
	ERRDATA
	GtkWidget	*dialog;
	gint		result;
	GtkWidget	*content_area;
	GtkWidget	*entrybox;

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,DIALOG_ENTER_FUNC_NAME_LABEL);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),DIALOG_FIND_FUNC_LABEL);

	content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));	
	entrybox=gtk_entry_new();
	gtk_entry_set_activates_default((GtkEntry*)entrybox,true);
	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_YES);
	gtk_container_add(GTK_CONTAINER(content_area),entrybox);
	gtk_widget_show_all(content_area);
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	if(functionSearchText!=NULL)
		{
			ERRDATA debugFree(&functionSearchText);
		}
	functionSearchText=strdup(gtk_entry_get_text((GtkEntry*)entrybox));
	gtk_widget_destroy(dialog);

	ERRDATA return(result);
}

#ifdef _BUILDDOCVIEWER_
void buildGtkDocViewer(void)
{
	ERRDATA
	GtkWidget	*vbox;
	GtkWidget	*hbox;
	GtkWidget	*button;
	GtkWidget	*scrolledWindow;
	GtkWidget	*entry;
	GtkWidget	*findbutton;
	GtkWidget	*findnextinpage;
	WebKitWebSettings	*settings;

	docView=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)docView,DOCVIEW_DOCVIEWER_LABEL);

	gtk_window_set_default_size((GtkWindow*)docView,docWindowWidth,docWindowHeight);
	if(docWindowX!=-1 && docWindowY!=-1)
		gtk_window_move((GtkWindow *)docView,docWindowX,docWindowY);

	vbox=createNewBox(NEWVBOX,false,0);
	hbox=createNewBox(NEWHBOX,false,4);

	webView=WEBKIT_WEB_VIEW(webkit_web_view_new());
	g_signal_connect(G_OBJECT(webView),"navigation-policy-decision-requested",G_CALLBACK(docLinkTrap),NULL);

	settings=webkit_web_view_get_settings(webView);
	g_object_set((gpointer)settings,"enable-file-access-from-file-uris",true,NULL);
	g_object_set((gpointer)settings,"enable-page-cache",true,NULL);
	g_object_set((gpointer)settings,"enable-plugins",false,NULL);
	g_object_set((gpointer)settings,"enable-caret-browsing",true,NULL);
	g_object_set((gpointer)settings,"enable-private-browsing",true,NULL);
	g_object_set((gpointer)settings,"enable-java-applet",false,NULL);

	scrolledWindow=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow),GTK_WIDGET(webView));

	gtk_box_pack_start((GtkBox*)vbox,scrolledWindow,true,true,0);

	button=createNewStockButton(GTK_STOCK_GO_BACK,MENU_GO_BACK_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoBack),(void*)webView);	

//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	button=createNewStockButton(GTK_STOCK_HOME,DOCVIEW_HOME_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoHome),(void*)webView);	
 
//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	entry=gtk_entry_new();
	findbutton=createNewStockButton(GTK_STOCK_FIND,MENU_FIND_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),findbutton,false,false,0);
	g_signal_connect(G_OBJECT(findbutton),"clicked",G_CALLBACK(docSearchFromBar),(void*)entry);	

	gtk_box_pack_start(GTK_BOX(hbox),entry,false,true,0);
	g_signal_connect_after(G_OBJECT(entry),"activate",G_CALLBACK(docSearchFromBar),(void*)entry);

	findnextinpage=createNewStockButton(GTK_STOCK_GO_DOWN,DOCVIEW_GO_DOWN_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),findnextinpage,false,false,0);
	g_signal_connect(G_OBJECT(findnextinpage),"clicked",G_CALLBACK(docSearchInPageFoward),(void*)entry);

	findbutton=createNewStockButton(GTK_STOCK_GO_UP,DOCVIEW_GO_UP_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),findbutton,false,false,0);
	g_signal_connect(G_OBJECT(findbutton),"clicked",G_CALLBACK(docSearchInPageBack),(void*)entry);

//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	button=createNewStockButton(GTK_STOCK_GO_FORWARD,DIALOG_GO_FORWARD_LABEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoForward),(void*)webView);	

	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,4);

	gtk_container_add(GTK_CONTAINER(docView),vbox);
	gtk_widget_grab_focus(GTK_WIDGET(webView));

#ifdef _BUILDDOCVIEWER_
	g_signal_connect_object(G_OBJECT(docView),"delete-event",G_CALLBACK(toggleDocviewer),G_OBJECT(docView),G_CONNECT_SWAPPED);
#endif
	const char *lang;

	if(strncmp(localeLang,"en",2)==0)
		lang="en";
	else
		lang="fr";

	sinkReturn=asprintf(&thePage,"file://%s/help.%s.html",GHELPFOLDER,lang);
	webkit_web_view_load_uri(webView,thePage);
	ERRDATA freeAndNull(&thePage);
	ERRDATA
}
#endif




