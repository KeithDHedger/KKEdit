/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:50:07 GMT 2015 kdhedger68713@gmail.com

 * This file(guis.cpp) is part of KKEdit.

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

GtkWidget*		recent;
GtkToolItem*	tool[18]={NULL,};
GtkIconView*	iconView=NULL;
GtkListStore*	listStore=NULL;

GtkWidget*		prefHBox;
GtkWidget*		prefHBox1;
GtkWidget*		prefHBox2;
GtkWidget*		pagevbox;

GtkWidget*		entries[NUMSHORTCUTS];

const char		*shortcuttext[NUMSHORTCUTS]={DELETE_LINE_LABEL,DELETE_TO_EOL_LABEL,DELETE_TO_BOL_LABEL,SELECT_WORD_LABEL,DELETE_WORD_LABEL,DUPLICATE_LINE_LABEL,SELECT_LINE_LABEL,LINE_UP_LABEL,LINE_DOWN_LABEL,SELECT_TO_EOL_LABEL,SELECT_TO_BOL_LABEL,SELECTION_UP_LABEL,SELECTION_DOWN_LABEL,SHOW_COMPLETION_LABEL};

GtkWidget*	prefsWidgets[MAXPREFSWIDGETS];
GObject*	prefsIntWidgets[MAXPREFSINTWIDGETS];

void findTool(toolStruct* data,char* toolname)
{
	ERRDATA
	if(strcmp(toolname,data->menuName)==0)
		selectedToolFromDrop=data;
	ERRDATA
}

void selectToolOptions(GtkWidget* widget,gpointer data)
{
	ERRDATA
	char*	text=gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
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
						gotoDefButton=createNewToolItem(GTK_STOCK_DIALOG_QUESTION,GOTO_DEFINE_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,gotoDefButton,-1);
						g_signal_connect(G_OBJECT(gotoDefButton),"clicked",G_CALLBACK(goToDefinition),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoDefButton,GOTO_DEFINE_TT_LABEL);
						break;
//go back
					case 'B':
						backButton=createNewToolItem(GTK_STOCK_GO_BACK,BACK_TOOLBAR_LABEL);
						gtk_toolbar_insert(toolBar,backButton,-1);
						g_signal_connect(G_OBJECT(backButton),"clicked",G_CALLBACK(goBack),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)backButton,BACK_TT_LABEL);
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

void fillCombo(GtkComboBoxText* combo)
{
	ERRDATA
	g_list_foreach(toolsList,addToolToDrop,NULL);
	ERRDATA
}

gboolean getToolKey(GtkEntry* widget,GdkEventKey* event,gpointer data)
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
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;
	GtkWidget*	toolwin;
	GtkWidget*	infolabel;

	char*		placeholderinfo;

	asprintf(&placeholderinfo,"%s",TOOLS_PLACEHOLDER_LABEL);

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
	//hbox=gtk_hbox_new(false,0);
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
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GList*			ptr;
	bool			gotglobal=false;
	int				keyflags=0;

	buildToolsList();

	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)toolsMenu);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)toolsMenu,NULL);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(toolsMenu),menu);

//addtool
	menuitem=createNewImageMenuItem(GTK_STOCK_EDIT,MENU_EXT_TOOLS_LABEL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doMakeTool),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	ptr=toolsList;
	while(ptr!=NULL)
		{
			if(((toolStruct*)ptr->data)->global==true)
				{
					gotglobal=true;
					menuitem=createNewImageMenuItem(NULL,((toolStruct*)ptr->data)->menuName);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);

					if(((toolStruct*)ptr->data)->keyCode!=GDK_KEY_VoidSymbol)
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
	while(ptr!=NULL)
		{
			if(((toolStruct*)ptr->data)->global==false)
				{
					menuitem=createNewImageMenuItem(NULL,((toolStruct*)ptr->data)->menuName);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);

					if(((toolStruct*)ptr->data)->keyCode!=GDK_KEY_VoidSymbol)
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
	char*		type;

	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			type=strndup((char*)&toolBarLayout[j],1);
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						setNewPixbuf(PREFSNEW,0,type);
						break;
					case 'O':
//open+recent
						setNewPixbuf(PREFSOPEN,1,type);
						break;
					case 'S':
//save
						setNewPixbuf(PREFSSAVE,2,type);
						break;

					case 'X':
//cut
						setNewPixbuf(PREFSCUT,3,type);
						break;
					case 'C':
//copy
						setNewPixbuf(PREFSCOPY,4,type);
						break;
					case 'P':
//paste
						setNewPixbuf(PREFSPASTE,5,type);
						break;
					case 'U':
//undo
						setNewPixbuf(PREFSUNDO,6,type);
						break;
					case 'R':
//redo
						setNewPixbuf(PREFSREDO,7,type);
						break;
					case 'F':
//find
						setNewPixbuf(PREFSFIND,8,type);
						break;
					case 'G':
//navigation
						setNewPixbuf(PREFSQUESTION,9,type);
						break;
//go back
					case 'B':
						setNewPixbuf(PREFSBACK,17,type);
						break;

					case '9':
//goto line num
						setNewPixbuf(PREFSLINENUM,10,type);
						break;

					case 'A':
//find api
						setNewPixbuf(PREFSAPI,11,type);
						break;
					case 'Q':
//find qt api
						setNewPixbuf(PREFSQTAPI,16,type);
						break;
					case 'D':
//find define
						setNewPixbuf(PREFSFINDDEF,12,type);
						break;
					case 'L':
//live search
						setNewPixbuf(PREFSLIVE,13,type);
						break;

					case 's':
//seperator
						setNewPixbuf(PREFSSEPERATOR,14,type);
						gtk_widget_set_sensitive((GtkWidget*)tool[14],true);
					break;

					case 'E':
//expander
						setNewPixbuf(PREFSEXPAND,15,type);
						gtk_widget_set_sensitive((GtkWidget*)tool[15],true);
						break;
				}
			ERRDATA debugFree(&type);
		}
	ERRDATA
}

void addToToolBar(GtkWidget* widget,gpointer ptr)
{
	ERRDATA
	char*	holddata=toolBarLayout;
	char*	type;

	type=strndup((char*)ptr,1);
	toolBarLayout=type;
	populateStore();
	toolBarLayout=holddata;
	ERRDATA debugFree(&type);
}

void addPixbuf(const char* pixbuf,const char* data,int toolnumber,const char* tooltip)
{
	ERRDATA
	GtkWidget*		image;
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
	addPixbuf(PREFSNEW,"N",0,NEW_TT_LABEL);
	addPixbuf(PREFSOPEN,"O",1,OPEN_TT_LABEL);
	addPixbuf(PREFSSAVE,"S",2,SAVE_TT_LABEL);
	addPixbuf(PREFSCUT,"X",3,CUT_TT_LABEL);
	addPixbuf(PREFSCOPY,"C",4,COPY_TT_LABEL);
	addPixbuf(PREFSPASTE,"P",5,PASTE_TT_LABEL);
	addPixbuf(PREFSUNDO,"U",6,UNDO_TT_LABEL);
	addPixbuf(PREFSREDO,"R",7,REDO_TT_LABEL);
	addPixbuf(PREFSFIND,"F",8,FIND_TT_LABEL);
	addPixbuf(PREFSQUESTION,"G",9,GOTO_DEFINE_TT_LABEL);
	addPixbuf(PREFSBACK,"B",17,BACK_TT_LABEL);
	addPixbuf(PREFSLINENUM,"9",10,TO_LINE_TT_LABEL);
	addPixbuf(PREFSAPI,"A",11,TO_API_TT_LABEL);
	addPixbuf(PREFSQTAPI,"Q",16,TO_QTAPI_TT_LABEL);
	addPixbuf(PREFSFINDDEF,"D",12,TO_DEFINE_TT_LABEL);
	addPixbuf(PREFSLIVE,"L",13,LIVE_TT_LABEL);
	addPixbuf(PREFSSEPERATOR,"s",14,SEPERATOR_TT_LABEL);
	addPixbuf(PREFSEXPAND,"E",15,EXPANDER_TT_LABEL);
	ERRDATA
}

char* makeToolBarList(void)
{
	ERRDATA
	GtkTreeIter iter;
	gboolean	valid;
	gchar*		str_data=NULL;
	GString*	str=g_string_new("");

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


bool clickIt(GtkWidget* widget,GdkEvent* event,gpointer data)
{
	ERRDATA
	GtkTreePath*	path=NULL;
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

void setKeyCuts(GtkWidget* widget,gpointer data)
{
	ERRDATA

	char*	filename;
	const char*		text;

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
					asprintf(&shortCutStrings[j],"%i %i - ^%c %s",shortCuts[j][0],shortCuts[j][1],shortCuts[j][0],shortcuttext[j]);
				}
			asprintf(&filename,"%s/." KKEDITVERS "/keybindings.rc",getenv("HOME"));
			saveVarsToFile(filename,keybindings_rc);
			gtk_widget_hide(keysWindow);
		}
	ERRDATA
}

gboolean setKeyInEntry(GtkEntry* widget,GdkEventKey* event,gpointer data)
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
	GtkWidget*	vbox;
	GtkWidget*	item;
	GtkWidget*	hbox;
	char		keystring[32];
	int			loop;
	char*		keycutsinfo;

	if(keysWindow==NULL)
		{
			keysWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
			gtk_window_set_title((GtkWindow*)keysWindow,KBSC_DEFINE_KB_LABEL);
			vbox=createNewBox(NEWVBOX,false,8);

			asprintf(&keycutsinfo,"%s",KBSC_INFO_LABEL);
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

GtkWidget*	makeMenuItem(const char* stocklabel,GtkWidget* parent,void* function,char hotkey,const char* name,int setimage,const char* menulabel,void* userdata,bool toggle)
{
	ERRDATA
	GtkWidget*	widg;
#ifndef _USEGTK3_
	GtkWidget*	image;
#endif

	switch(setimage)
		{
			case STOCKMENU:
				widg=createNewStockMenuItem(stocklabel,menulabel);
				break;

			case IMAGEMENU:
				widg=createNewImageMenuItem(stocklabel,menulabel);
				break;

			case PIXMAPMENU:
#ifdef _USEGTK3_
				widg=gtk_menu_item_new_with_mnemonic(menulabel);
#else
				widg=gtk_image_menu_item_new_with_mnemonic(menulabel);
				image=gtk_image_new_from_file(stocklabel);
				gtk_image_menu_item_set_image((GtkImageMenuItem *)widg,image);
#endif
				break;

			case CHECKMENU:
				widg=gtk_check_menu_item_new_with_label(stocklabel);
				gtk_check_menu_item_set_active((GtkCheckMenuItem*)widg,toggle);
				break;

			default:
				widg=createNewImageMenuItem(NULL,stocklabel);
		}

	gtk_menu_shell_append(GTK_MENU_SHELL(parent),widg);
	g_signal_connect(G_OBJECT(widg),"activate",G_CALLBACK(function),userdata);

	if(hotkey>0)
		gtk_widget_add_accelerator((GtkWidget *)widg,"activate",accgroup,hotkey,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	if(hotkey<0)
		gtk_widget_add_accelerator((GtkWidget *)widg,"activate",accgroup,hotkey*-1,(GdkModifierType)(GDK_SHIFT_MASK|GDK_CONTROL_MASK),GTK_ACCEL_VISIBLE);

	gtk_widget_set_name(widg,name);
	ERRDATA return(widg);
}

void makePrefsDial(int widgnum,const char* label,const char* name,int value,int minvalue,int maxvalue)
{
	ERRDATA
	GtkWidget*	item;

	prefsIntWidgets[widgnum]=(GObject*)gtk_adjustment_new(value,minvalue,maxvalue,1,1,0);
	item=gtk_spin_button_new((GtkAdjustment*)prefsIntWidgets[widgnum],1,0);
	gtk_widget_set_name(item,name);
	gtk_container_add(GTK_CONTAINER(prefHBox1),gtk_label_new(label));
	gtk_box_pack_start((GtkBox*)prefHBox2,item,true,true,0);
	gtk_box_pack_start((GtkBox*)pagevbox,prefHBox,false,false,0);
	ERRDATA
}

void makePrefsCheck(int widgnum,const char* label,const char* name,bool onoff,bool left,bool autoadd)
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

VISIBLE void doPrefs(GtkWidget* widget,gpointer data)
{
	ERRDATA
	GtkWidget*		vbox;
	GtkWidget*		hbox;
	GtkWidget*		item;
	GtkWidget*		label;
	GtkNotebook*	prefsnotebook;

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

void addRecentToMenu(GtkRecentChooser* chooser,GtkWidget* menu)
{
	ERRDATA
	GList*		itemlist=NULL;
	GList*		l=NULL;
	GtkWidget*	menuitem;
	const char	*uri=NULL;
	char*		filename=NULL;
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
			if(uri!=NULL)
				{
					filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
					menuitem=createNewImageMenuItem(NULL,menuname);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(recentFileMenu),(void*)filename);
				}
		}
	ERRDATA
}

void buildMainGui(void)
{
	ERRDATA
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GtkWidget*		menurecent;
	GtkWidget*		plugsubmenu=NULL;

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

	menuBar=gtk_menu_bar_new();
	toolBarBox=createNewBox(NEWHBOX,true,0);
	toolBar=(GtkToolbar*)gtk_toolbar_new();

//dnd
	gtk_drag_dest_set(mainWindowVBox,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(mainWindowVBox);
	g_signal_connect(G_OBJECT(mainWindowVBox),"drag_data_received",G_CALLBACK(dropUri),NULL);

	setUpToolBar();
	gtk_box_pack_start(GTK_BOX(toolBarBox),(GtkWidget*)toolBar,true,true,0);

//menus
//file menu
	fileMenu=gtk_menu_item_new_with_label(MENU_FILE_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)fileMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenu),menu);

//new
	menuitem=makeMenuItem(GTK_STOCK_NEW,menu,(void*)newFile,'N',NEWMENUNAME,STOCKMENU,MENU_NEW_LABEL,NULL,false);
//open
	menuItemOpen=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)doOpenFile,'O',OPENMENUNAME,STOCKMENU,MENU_OPEN_LABEL,NULL,false);
//open as hexdump
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)openAsHexDump,0,HEXDUMPMENUNAME,IMAGEMENU,MENU_OPEN_AS_HEXDUMP_LABEL,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//extras
	menuitem=makeMenuItem(DATADIR"/pixmaps/ROOTKKEdit.png",menu,(void*)newEditor,0,NEWADMINMENUNAME,PIXMAPMENU,MENU_NEW_ADMIN_LABEL,(void*)1,false);
	menuitem=makeMenuItem(DATADIR"/pixmaps/MenuKKEdit.png",menu,(void*)newEditor,0,NEWEDITORMENUNAME,PIXMAPMENU, MENU_NEW_EDITOR_LABEL,(void*)2,false);

	if(gotManEditor==0)
		menuitem=makeMenuItem(DATADIR"/pixmaps/ManPageEditor.png",menu,(void*)newEditor,0,MANEDITORMENUNAME,PIXMAPMENU,MENU_MANPAGE_EDITOR_LABEL,(void*)3,false);

//doxy
	if(gotDoxygen==0)
		menuitem=makeMenuItem(GTK_STOCK_COPY,menu,(void*)doDoxy,0,DOXYBUILDMENUNAME,IMAGEMENU,MENU_BUILD_DOCS_LABEL,(void*)1,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//recent menu
	menuitem=createNewImageMenuItem(NULL,MENU_RECENT_FILES_LABEL);
	menurecent=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),menurecent);
	addRecentToMenu((GtkRecentChooser*)recent,menurecent);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_widget_set_name(menuitem,RECENTMENUNAME);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save
	saveMenu=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)saveFile,'S',SAVEMENUNAME,STOCKMENU,MENU_SAVE_LABEL,NULL,false);
//savas
	saveAsMenu=makeMenuItem(GTK_STOCK_SAVE_AS,menu,(void*)saveFile,-'S',SAVEASMENUNAME,STOCKMENU,MENU_SAVE_AS_LABEL,(void*)1,false);
//save all
	saveAllMenu=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)doSaveAll,0,SAVEALLMENUNAME,IMAGEMENU,MENU_SAVE_ALL_LABEL,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save session
	menuitem=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)saveSession,0,SAVESESSIONMENUNAME,IMAGEMENU,MENU_SAVE_SESSION_LABEL,(void*)false,false);
//restore session
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)restoreSession,0,RESTORESESSIONMENUNAME,IMAGEMENU,MENU_RESTORE_SESSION_LABEL,NULL,false);

//printfile
	printMenu=makeMenuItem(GTK_STOCK_PRINT,menu,(void*)printFile,'P',PRINTMENUNAME,STOCKMENU,PRINT_LABEL,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//close
	closeMenu=makeMenuItem(GTK_STOCK_CLOSE,menu,(void*)closeTab,'W',CLOSEMENUNAME,STOCKMENU,CLOSE_LABEL,NULL,false);
//close-all
	closeAllMenu=makeMenuItem(GTK_STOCK_CLOSE,menu,(void*)closeAllTabs,0,CLOSEALLMENUNAME,IMAGEMENU,MENU_CLOSE_ALL_LABEL,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//reload file
	revertMenu=makeMenuItem(GTK_STOCK_REVERT_TO_SAVED,menu,(void*)reloadFile,0,REVERTMENUNAME,STOCKMENU,REVERT_TO_SAVED_LABEL,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//quit
	menuitem=makeMenuItem(GTK_STOCK_QUIT,menu,(void*)doShutdown,'Q',QUITMENUNAME,STOCKMENU,QUIT_LABEL,NULL,false);

//edit menu
	editMenu=gtk_menu_item_new_with_label(MENU_EDIT_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)editMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMenu),menu);
//undo
	undoMenu=makeMenuItem(GTK_STOCK_UNDO,menu,(void*)undo,'Z',UNDOMENUNAME,STOCKMENU,MENU_UNDO_LABEL,NULL,false);
//redo
	redoMenu=makeMenuItem(GTK_STOCK_REDO,menu,(void*)redo,-'Z',REDOMENUNAME,STOCKMENU,MENU_REDO_LABEL,NULL,false);
//undoall
	undoAllMenu=makeMenuItem(GTK_STOCK_UNDO,menu,(void*)unRedoAll,0,UNDOALLMENUNAME,IMAGEMENU,MENU_UNDO_ALL_LABEL,(void*)0,false);
//redoall
	redoAllMenu=makeMenuItem(GTK_STOCK_REDO,menu,(void*)unRedoAll,0,REDOALLMENUNAME,IMAGEMENU,MENU_REDO_ALL_LABEL,(void*)1,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//cut
	cutMenu=makeMenuItem(GTK_STOCK_CUT,menu,(void*)cutToClip,'X',CUTMENUNAME,STOCKMENU,MENU_CUT_LABEL,NULL,false);
//copy
	copyMenu=makeMenuItem(GTK_STOCK_COPY,menu,(void*)copyToClip,'C',COPYMENUNAME,STOCKMENU,MENU_COPY_LABEL,NULL,false);
//paste
	pasteMenu=makeMenuItem(GTK_STOCK_PASTE,menu,(void*)pasteFromClip,'V',PASTEMENUNAME,STOCKMENU,MENU_PASTE_LABEL,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//find
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)find,'F',FINDMENUNAME,STOCKMENU,MENU_FIND_LABEL,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//sort
	sortTabsMenu=makeMenuItem(GTK_STOCK_SORT_ASCENDING,menu,(void*)sortTabs,0,SORTTABSMENUNAME,IMAGEMENU,MENU_SORT_TABS_LABEL,NULL,false);
//jump to tab
	viewTabMenu=createNewImageMenuItem(GTK_STOCK_EDIT,MENU_SELECT_TAB_LABEL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),viewTabMenu);
	rebuildTabsMenu();

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//prefs
	menuitem=makeMenuItem(GTK_STOCK_PREFERENCES,menu,(void*)doPrefs,0,PREFSMENUNAME,STOCKMENU,MENU_PREFERENCES_LABEL,NULL,false);
//plugs
	menuitem=makeMenuItem(GTK_STOCK_PREFERENCES,menu,(void*)doPlugPrefs,0,PLUGPREFSMENUNAME,IMAGEMENU,MENU_PLUG_PREFS_LABEL,NULL,false);

//view menu
	viewMenu=gtk_menu_item_new_with_label(MENU_VIEW_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)viewMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewMenu),menu);

//show docs
	menuitem=makeMenuItem(MENU_SHOW_DOCS_LABEL,menu,(void*)doDoxy,0,SHOWDOCSMENUNAME,NORMALMENU,NULL,(void*)2,false);

//toggle bookmark bar
	if(showBMBar)
		menuitem=makeMenuItem(MENU_HIDE_BM_BAR_LABEL,menu,(void*)toggleBookMarkBar,0,SHOWBMBARMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		menuitem=makeMenuItem(MENU_SHOW_BM_BAR_LABEL,menu,(void*)toggleBookMarkBar,0,SHOWBMBARMENUNAME,NORMALMENU,NULL,NULL,false);

//toggle toolbar bar
	if(showToolBar)
		menuitem=makeMenuItem(MENU_HIDE_TOOLBAR_LABEL,menu,(void*)toggleToolBar,0,SHOWTOOLBARMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		menuitem=makeMenuItem(MENU_SHOW_TOOLBAR_LABEL,menu,(void*)toggleToolBar,0,SHOWTOOLBARMENUNAME,NORMALMENU,NULL,NULL,false);

//tooloutput
	if(showToolOutWin)
		toolOutMenu=makeMenuItem(MENU_HIDE_TOOL_OP_LABEL,menu,(void*)toggleToolOutput,0,SHOWTOOLOUTMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		toolOutMenu=makeMenuItem(MENU_SHOW_TOOL_OP_LABEL,menu,(void*)toggleToolOutput,0,SHOWTOOLOUTMENUNAME,NORMALMENU,NULL,NULL,false);
//toggle statusbar
	if(showStatus)
		statusBarMenu=makeMenuItem(MENU_HIDE_STATUS_LABEL,menu,(void*)toggleStatusBar,0,SHOWSTATUSMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		statusBarMenu=makeMenuItem(MENU_SHOW_STATUS_LABEL,menu,(void*)toggleStatusBar,0,SHOWSTATUSMENUNAME,NORMALMENU,NULL,NULL,false);

#ifdef _BUILDDOCVIEWER_
//toggle docviewer
	showDocViewWidget=makeMenuItem(MENU_SHOW_DOCVIEWER_LABEL,menu,(void*)toggleDocviewer,0,SHOWDOCVIEWERMENUNAME,NORMALMENU,NULL,NULL,false);
#endif

	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
//toggle line nubers
		menuitem=makeMenuItem(MENU_SHOW_LINE_NUMBERS_LABEL,menu,(void*)toggleLineNumbers,0,VIEWSHOWLINENUMERS,CHECKMENU,NULL,NULL,lineNumbers);
//toggle wrap lines
	menuitem=makeMenuItem( MENU_WRAP_LINES_LABEL,menu,(void*)toggleWrapLines,0,VIEWWRAPLINES,CHECKMENU,NULL,NULL,lineWrap);

//toggle higlight current line
	menuitem=makeMenuItem(MENU_HIGHLIGHT_LINE_LABEL,menu,(void*)toggleHighlightCurrent,0,VIEWHIGHLIGHT,CHECKMENU,NULL,NULL,highLight);

//toggle syntax highlight
	menuitem=makeMenuItem(MENU_NO_SYNTAX_LABEL,menu,(void*)toggleSyntax,0,VIEWNOSYNTAX,CHECKMENU,NULL,NULL,noSyntax);

//toggle auto show comps
	menuitem=makeMenuItem(MENU_SHOW_AUTO_COMPLETE,menu,(void*)toggleAutoComplete,0,VIEWAUTOCOMPLETE,CHECKMENU,NULL,NULL,autoShowComps);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());

//navigation menu
	navMenu=gtk_menu_item_new_with_label(MENU_NAV_MENU_LABEL);
	gtk_menu_item_set_use_underline((GtkMenuItem*)navMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(navMenu),menu);

//goto define
	gotoDefMenu=makeMenuItem(GTK_STOCK_DIALOG_QUESTION,menu,(void*)goToDefinition,'D',GOTODEFMENUNAME,IMAGEMENU,MENU_GOTO_DEFINE_LABEL,NULL,false);
//open include
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)findFile,'I',OPENINCLUDEMENUNAME,IMAGEMENU,MENU_OPEN_INCLUDE_LABEL,NULL,false);
//goto line number
	menuitem=makeMenuItem(GTK_STOCK_GO_DOWN,menu,(void*)jumpToLine,0,GOTOLINEMENUNAME,IMAGEMENU,MENU_GOTO_LINE_LABEL,NULL,false);
//find define
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)functionSearch,0,SEARCHFORDEFMENUNAME,IMAGEMENU,MENU_FIND_DEFINE_LABEL,NULL,false);
//find gtkdoc
	searchInGtkDocsMenu=makeMenuItem(GTK_STOCK_FIND,menu,(void*)searchGtkDocs,0,SEARCHGTKMENUNAME,IMAGEMENU,MENU_FIND_IN_GTKAPI_LABEL,NULL,false);
//find qt5
	searchInQTDocsMenu=makeMenuItem(GTK_STOCK_FIND,menu,(void*)searchQT5Docs,0,SEARCHQT5MENUNAME,IMAGEMENU,MENU_FIND_IN_QTAPI_LABEL,NULL,false);

//goto doxy docs
	if(gotDoxygen==0)
		searchInDocsMenu=makeMenuItem(GTK_STOCK_FIND,menu,(void*)doxyDocs,0,SEARCHDOXYMENUNAME,IMAGEMENU,MENU_FIND_IN_DOCS_LABEL,NULL,false);

//go back
	goBackMenu=makeMenuItem(GTK_STOCK_GO_BACK,menu,(void*)goBack,0,GOBACKMENUNAME,STOCKMENU,MENU_GO_BACK_LABEL,NULL,false);

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
	menuitem=makeMenuItem(GTK_STOCK_ABOUT,menu,(void*)doAbout,0,ABOUTMENUNAME,STOCKMENU,MENU_ABOUT_LABEL,NULL,false);
//help
	menuitem=makeMenuItem(GTK_STOCK_HELP,menu,(void*)openHelp,0,HELPMENUNAME,STOCKMENU,MENU_HELP_LABEL,NULL,false);
//get plugins
	menuitem=makeMenuItem(DATADIR"/pixmaps/KKEditPlugMenu.png",menu,(void*)getPlugins,0,GETPLUGSMENUNAME,PIXMAPMENU,MENU_GET_PLUGS_LABEL,NULL,false);

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
			gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),globalPlugMenu);
		}
	else
		globalPlugMenu=NULL;

	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),helpMenu);

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

#if 0
	gtk_container_add((GtkContainer*)mainWindow,mainWindowVBox);
//	gtk_container_add((GtkContainer*)mainWindow,whbox);
//	gtk_widget_show_all(whbox);
//addmenubar
	gtk_box_pack_start((GtkBox*)mainWindowVBox,menuBar,false,false,0);
//add toolbar
	gtk_box_pack_start((GtkBox*)mainWindowVBox,toolBarBox,false,false,0);

//add left user
	gtk_paned_pack1((GtkPaned*)mainWindowHPane,mainLeftUserVBox,true,true);
//	gtk_paned_pack2((GtkPaned*)mainWindowHPane,secondWindowHPane,true,true);

//add mainNotebook
//	gtk_box_pack_start((GtkBox*)mainNotebookVBox,(GtkWidget*)mainNotebook,true,true,0);
//gtk_paned_pack1((GtkPaned*)secondWindowHPane,mainWindowHBox,true,true);
//gtk_box_pack_start((GtkBox*)secondWindowHPane,(GtkWidget*)mainNotebookVBox,true,true,0);
//gtk_widget_show_all(mainWindowHPane);
gtk_paned_pack2((GtkPaned*)mainWindowHPane,(GtkWidget*)mainNotebook,true,true);
//	gtk_box_pack_start((GtkBox*)mainWindowHBox,mainVPane,true,true,0);
//	gtk_paned_pack1((GtkPaned*)secondWindowHPane,mainWindowHBox,true,false);
 //                                                        
////addright user
//	gtk_paned_pack2((GtkPaned*)secondWindowHPane,mainRightUserVBox,false,true);

#endif

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
	if(useGlobalPlugMenu==true)
		globalPlugins->globalPlugData->mlist.menuBar=plugsubmenu;
	else
		globalPlugins->globalPlugData->mlist.menuBar=menuBar;

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
	ERRDATA
//	doBusy(false,NULL);
	globalHistory=new HistoryClass(mainNotebook);
	resetSensitive();
}

void buildFindReplace(void)
{
	ERRDATA
	GtkWidget*	content_area;
	GtkWidget*	replace;
#ifndef _USEGTK3_
	GtkWidget*	image;
#endif
	GtkWidget*	label;
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	item;

#ifdef _USEGTK3_
	findReplaceDialog=gtk_dialog_new_with_buttons(FIND_FIND_REPLACE_LABEL,(GtkWindow*)mainWindow, GTK_DIALOG_DESTROY_WITH_PARENT,DIALOG_GO_FORWARD_LABEL,FINDNEXT,MENU_GO_BACK_LABEL,FINDPREV,FIND_REPLACE_LABEL,REPLACE,NULL);
#else
	findReplaceDialog=gtk_dialog_new_with_buttons(FIND_FIND_REPLACE_LABEL,(GtkWindow*)mainWindow, GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_GO_FORWARD,FINDNEXT,GTK_STOCK_GO_BACK,FINDPREV,FIND_REPLACE_LABEL,REPLACE,NULL);
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

	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
#ifndef _USEGTK3_
	image=gtk_image_new_from_icon_name(GTK_STOCK_FIND_AND_REPLACE,GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image((GtkButton*)replace,image);
#endif	
	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
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
	GtkWidget*	vbox;
	GtkWidget*	button;
	GtkWidget*	hbox;
	GtkWidget*	label;
	GtkWidget*	image;
	char*		labeltext[512];
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

	GtkWidget* content=gtk_dialog_get_content_area((GtkDialog *)spellCheckWord);
	gtk_container_add(GTK_CONTAINER(content),(GtkWidget*)vbox);

	g_signal_connect_object(G_OBJECT(spellCheckWord),"delete_event",G_CALLBACK(gtk_widget_hide),G_OBJECT(spellCheckWord),G_CONNECT_SWAPPED);
	g_signal_connect(G_OBJECT(spellCheckWord),"delete_event",G_CALLBACK(gtk_true),NULL);
	ERRDATA
}
#endif

int showFunctionEntry(void)
{
	ERRDATA
	GtkWidget*	dialog;
	gint		result;
	GtkWidget*	content_area;
	GtkWidget*	entrybox;

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
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;
	GtkWidget*	scrolledWindow;
	GtkWidget*	entry;
	GtkWidget*	findbutton;
	GtkWidget*	findnextinpage;
	WebKitWebSettings*	settings;

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

	g_signal_connect_object(G_OBJECT(docView),"delete-event",G_CALLBACK(toggleDocviewer),G_OBJECT(docView),G_CONNECT_SWAPPED);

	const char* lang;

	if(strncmp(localeLang,"en",2)==0)
		lang="en";
	else
		lang="fr";

	asprintf(&thePage,"file://%s/help/help.%s.html",DATADIR,lang);
	webkit_web_view_load_uri(webView,thePage);
	ERRDATA freeAndNull(&thePage);
	ERRDATA
}
#endif




