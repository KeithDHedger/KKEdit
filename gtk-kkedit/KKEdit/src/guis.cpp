/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:50:07 GMT 2015 kdhedger68713@gmail.com

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

GtkWidget*		recent;
GtkToolItem*	tool[18]={NULL,};
GtkIconView*	iconView=NULL;
GtkListStore*	listStore=NULL;
//TODO//
#ifdef _USEGTK3_
GtkWidget*		table;
#else
GtkTable*		table;
#endif

GtkWidget*		entries[NUMSHORTCUTS];
const char* 	shortcuttext[NUMSHORTCUTS]={gettext("Delete Current Line"),gettext("Delete To End Of Line"),gettext("Delete To Beginning Of Line"),gettext("Select Word Under Cursor"),gettext("Delete Word Under Cursor"),gettext("Duplicate Current Line"),gettext("Select Current Line"),gettext("Move Current Line Up"),gettext("Move Current Line Down"),gettext("Select From Cursor To End Of Line"),gettext("Select From Beginning Of Line To Cursor"),gettext("Move Selection Up"),gettext("Move Selection Down"),gettext("Show Completion")};

GtkWidget*	prefsWidgets[MAXPREFSWIDGETS];
//GtkObject*	prefsIntWidgets[MAXPREFSINTWIDGETS];
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

			if(selectedToolFromDrop->keyCode!=0)
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

			if((gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false) || (gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget)==true))
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
	GtkToolItem	*toolbutton;
	GtkWidget	*image=NULL;

//#ifndef _USEGTK3_
	GtkRecentFilter*	filter;

	recent=gtk_recent_chooser_menu_new();
	gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recent),false);
	gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent),GTK_RECENT_SORT_MRU);
	gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent),MAXRECENT);

	filter=gtk_recent_filter_new();
	gtk_recent_filter_add_application(filter,"kkedit");
	gtk_recent_chooser_set_filter(GTK_RECENT_CHOOSER(recent),filter);
	g_signal_connect(recent,"item_activated",G_CALLBACK(recentFileMenu),NULL);
//#endif
	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						//newButton=gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
						newButton=createNewToolItem(GTK_STOCK_NEW,gettext("New"));
						gtk_toolbar_insert(toolBar,newButton,-1);
						g_signal_connect(G_OBJECT(newButton),"clicked",G_CALLBACK(newFile),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)newButton,gettext("New File"));
						break;
					case 'O':
//open+recent
#ifdef _USEGTK3_
						//openButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_OPEN);
					//	openButton=createNewToolItem(GTK_STOCK_OPEN,GTK_STOCK_OPEN3);
						image=gtk_image_new_from_icon_name(GTK_STOCK_OPEN,GTK_ICON_SIZE_LARGE_TOOLBAR);

						openButton=gtk_menu_tool_button_new (image,GTK_STOCK_OPEN3);
						gtk_menu_tool_button_set_menu((GtkMenuToolButton*)openButton,recent);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(openButton),gettext("Open Recent File"));
						gtk_toolbar_insert(toolBar,openButton,-1);
						
#else
						//openButton=createNewToolItem(GTK_STOCK_OPEN,GTK_STOCK_OPEN);
						openButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_OPEN);
//TODO//
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(openButton),recent);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(openButton),gettext("Open Recent File"));
						g_signal_connect(G_OBJECT(openButton),"clicked",G_CALLBACK(doOpenFile),NULL);
						gtk_toolbar_insert(toolBar,openButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)openButton,gettext("Open File"));
#endif

						break;
					case 'S':
//save
						//saveButton=gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
						saveButton=createNewToolItem(GTK_STOCK_SAVE,gettext("Save"));
						g_signal_connect(G_OBJECT(saveButton),"clicked",G_CALLBACK(saveFile),NULL);
						gtk_toolbar_insert(toolBar,saveButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)saveButton,gettext("Save File"));
						break;
					case 's':
						gtk_toolbar_insert(toolBar,gtk_separator_tool_item_new(),-1);
						break;
					case 'X':
//cut
						//cutButton=gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
						cutButton=createNewToolItem(GTK_STOCK_CUT,gettext("Cut"));
						g_signal_connect(G_OBJECT(cutButton),"clicked",G_CALLBACK(cutToClip),NULL);
						gtk_toolbar_insert(toolBar,cutButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)cutButton,gettext("Cut"));
						break;
					case 'C':
//copy
						//copyButton=gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
						copyButton=createNewToolItem(GTK_STOCK_COPY,gettext("Copy"));
						gtk_toolbar_insert(toolBar,copyButton,-1);
						g_signal_connect(G_OBJECT(copyButton),"clicked",G_CALLBACK(copyToClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)copyButton,gettext("Copy"));
						break;
					case 'P':
//paste
						//pasteButton=gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);
						pasteButton=createNewToolItem(GTK_STOCK_PASTE,gettext("Paste"));

						gtk_toolbar_insert(toolBar,pasteButton,-1);
						g_signal_connect(G_OBJECT(pasteButton),"clicked",G_CALLBACK(pasteFromClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)pasteButton,gettext("Paste"));
						break;
					case 'U':
//undo
						//undoButton=gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
						undoButton=createNewToolItem(GTK_STOCK_UNDO,gettext("Undo"));

						gtk_toolbar_insert(toolBar,undoButton,-1);
						g_signal_connect(G_OBJECT(undoButton),"clicked",G_CALLBACK(undo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)undoButton,gettext("Undo"));
						break;
					case 'R':
//redo
						//redoButton=gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
						redoButton=createNewToolItem(GTK_STOCK_REDO,gettext("Redo"));

						gtk_toolbar_insert(toolBar,redoButton,-1);
						g_signal_connect(G_OBJECT(redoButton),"clicked",G_CALLBACK(redo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)redoButton,gettext("Redo"));
						break;
					case 'F':
//find
						//findButton=gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
						findButton=createNewToolItem(GTK_STOCK_FIND,gettext("Find"));

						gtk_toolbar_insert(toolBar,findButton,-1);
						g_signal_connect(G_OBJECT(findButton),"clicked",G_CALLBACK(find),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)findButton,gettext("Find"));
						break;
					case 'G':
//navigation
						//gotoDefButton=gtk_tool_button_new_from_stock(GTK_STOCK_DIALOG_QUESTION);
						gotoDefButton=createNewToolItem(GTK_STOCK_DIALOG_QUESTION,gettext("Go To Definition"));

						gtk_toolbar_insert(toolBar,gotoDefButton,-1);
						g_signal_connect(G_OBJECT(gotoDefButton),"clicked",G_CALLBACK(goToDefinition),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoDefButton,gettext("Go To Definition"));
						break;
//go back
					case 'B':
						//backButton=gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
						backButton=createNewToolItem(GTK_STOCK_GO_BACK,gettext("Back"));

						gtk_toolbar_insert(toolBar,backButton,-1);
						g_signal_connect(G_OBJECT(backButton),"clicked",G_CALLBACK(goBack),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)backButton,gettext("Go Back"));
						break;

					case '9':
						lineNumberWidget=gtk_entry_new();
						gotoLineButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)gotoLineButton,lineNumberWidget);
						gtk_toolbar_insert(toolBar,gotoLineButton,-1);
						g_signal_connect_after(G_OBJECT(lineNumberWidget),"key-release-event",G_CALLBACK(jumpToLineFromBar),NULL);
						gtk_widget_set_size_request((GtkWidget*)gotoLineButton,48,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoLineButton,gettext("Go To Line"));
						break;
					case 'A':
//find in gtkdoc
						findApiWidget=gtk_entry_new();
						findApiButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findApiButton,findApiWidget);
						gtk_toolbar_insert(toolBar,findApiButton,-1);
						g_signal_connect_after(G_OBJECT(findApiWidget),"activate",G_CALLBACK(docSearchFromBar),(void*)findApiWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findApiButton,gettext("Find API In Gtk Docs"));
						break;

					case 'Q':
//find in qt5doc
						findQtApiWidget=gtk_entry_new();
						findQtApiButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findQtApiButton,findQtApiWidget);
						gtk_toolbar_insert(toolBar,findQtApiButton,-1);
						g_signal_connect_after(G_OBJECT(findQtApiWidget),"activate",G_CALLBACK(qt5DocSearchFromBar),(void*)findQtApiWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findQtApiButton,gettext("Find API In Qt5 Docs"));
						break;
					case 'D':
//find in function def
						findDefWidget=gtk_entry_new();
						findFuncDefButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findFuncDefButton,findDefWidget);
						gtk_toolbar_insert(toolBar,findFuncDefButton,-1);
						g_signal_connect_after(G_OBJECT(findDefWidget),"activate",G_CALLBACK(defSearchFromBar),(void*)findDefWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findFuncDefButton,gettext("Search For Define"));
						break;
					case 'L':
//livesearch
						liveSearchWidget=gtk_entry_new();
						liveSearchButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)liveSearchButton,liveSearchWidget);
						gtk_toolbar_insert(toolBar,liveSearchButton,-1);
						g_signal_connect_after(G_OBJECT(liveSearchWidget),"key-release-event",G_CALLBACK(doLiveSearch),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)liveSearchButton,gettext("Live Search"));
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
	if((event->type==GDK_KEY_PRESS) && (event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			ERRDATA return(true);
		}

	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
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

	asprintf(&placeholderinfo,"%s",gettext("PLACEHOLDERS:\n%t - Currently selected text. Passed to command as $KKEDIT_SELECTION\n%f - Filepath of the current document. Passed to command as $KKEDIT_CURRENTFILE\n%d - Directory of the current document or $HOME. Passed to command as $KKEDIT_CURRENTDIR\n%i - The location of the globally installed tools. Passed to command as $KKEDIT_DATADIR\n%h - Tempory file for displaying html in doc viewer. Passed to command as $KKEDIT_HTMLFILE\n%l - Highlihting language. Passed to command as $KKEDIT_SOURCE_LANG"));

	toolwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)toolwin,gettext("Edit External Tools"));
	//vbox=gtk_vbox_new(false,8);
	vbox=creatNewBox(NEWVBOX,false,8);

//select tool
	toolSelect=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(vbox),toolSelect,false,true,0);
	g_signal_connect(G_OBJECT(toolSelect),"changed",G_CALLBACK(selectToolOptions),NULL);
	fillCombo((GtkComboBoxText*) toolSelect);

//name
	toolNameWidget=gtk_entry_new();
	//hbox=gtk_hbox_new(false,0);
	hbox=creatNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Tool Name:\t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),toolNameWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(toolNameWidget);
	gtk_entry_set_text((GtkEntry*)toolNameWidget,"New Tool");

//command
	commandLineWidget=gtk_entry_new();
	//hbox=gtk_hbox_new(false,0);
	hbox=creatNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Command: \t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commandLineWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(commandLineWidget);
	gtk_entry_set_text((GtkEntry*)commandLineWidget,"");
//key
	keyWidget=gtk_entry_new();
	gtk_widget_show(keyWidget);
	//hbox=gtk_hbox_new(false,0);
	hbox=creatNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Shortcut:\t\t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),keyWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)keyWidget,"");
	g_signal_connect(G_OBJECT(keyWidget),"key-press-event",G_CALLBACK(getToolKey),NULL);

//comment
	commentWidget=gtk_entry_new();
	gtk_widget_show(commentWidget);
	//hbox=gtk_hbox_new(false,0);
	hbox=creatNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Comment:  \t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commentWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)commentWidget,"");

//info
	infolabel=gtk_label_new(placeholderinfo);
	gtk_label_set_selectable((GtkLabel*)infolabel,true);
	gtk_box_pack_start(GTK_BOX(vbox),infolabel,false,false,0);
	ERRDATA debugFree(&placeholderinfo);
//in terminal
	inTermWidget=gtk_check_button_new_with_label(gettext("Run Tool In Terminal"));
	gtk_widget_set_name(inTermWidget,"interm");
	gtk_box_pack_start(GTK_BOX(vbox),inTermWidget,false,true,0);
	g_signal_connect(G_OBJECT(inTermWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//show in popup menu
	inPopupWidget=gtk_check_button_new_with_label(gettext("Show Tool In Pop-Up Menu"));
	gtk_widget_set_name(inPopupWidget,"inpopup");
	gtk_box_pack_start(GTK_BOX(vbox),inPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(inPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//always show in popup
	alwaysPopupWidget=gtk_check_button_new_with_label(gettext("Always Show Tool In Pop-Up Menu"));
	gtk_widget_set_name(alwaysPopupWidget,"alwayspopup");
	gtk_box_pack_start(GTK_BOX(vbox),alwaysPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(alwaysPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags
//snch/async
	syncWidget=gtk_check_button_new_with_label(gettext("Run Tool Synchronously"));
	gtk_widget_set_name(syncWidget,"sync");
	gtk_box_pack_start(GTK_BOX(vbox),syncWidget,false,true,0);
	g_signal_connect(G_OBJECT(syncWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - show html doc
	showDocWidget=gtk_check_button_new_with_label(gettext("Show HTML Doc"));
	gtk_widget_set_name(showDocWidget,"showdoc");
	gtk_box_pack_start(GTK_BOX(vbox),showDocWidget,false,true,0);
	g_signal_connect(G_OBJECT(showDocWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - clear view first
	clearViewWidget=gtk_check_button_new_with_label(gettext("Clear Tool Output First"));
	gtk_widget_set_name(clearViewWidget,"clearview");
	gtk_box_pack_start(GTK_BOX(vbox),clearViewWidget,false,true,0);
	g_signal_connect(G_OBJECT(clearViewWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - run as root
	runAsRootWidget=gtk_check_button_new_with_label(gettext("Run Tool As Root"));
	gtk_widget_set_name(runAsRootWidget,"runasroot");
	gtk_box_pack_start(GTK_BOX(vbox),runAsRootWidget,false,true,0);
	g_signal_connect(G_OBJECT(runAsRootWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - use progressbar
	useBarWidget=gtk_check_button_new_with_label(gettext("Use Progress Bar"));
	gtk_widget_set_name(useBarWidget,"usebar");
	gtk_box_pack_start(GTK_BOX(vbox),useBarWidget,false,true,0);
	g_signal_connect(G_OBJECT(useBarWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - ignore
	ignoreWidget=gtk_radio_button_new_with_label(NULL,gettext("Ignore Output"));
	gtk_widget_set_name(ignoreWidget,"ignore");
	gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
	gtk_box_pack_start(GTK_BOX(vbox),ignoreWidget,false,true,0);

//flags - paste
	pasteWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,gettext("Paste Output"));
	gtk_widget_set_name(pasteWidget,"paste");
	gtk_box_pack_start(GTK_BOX(vbox),pasteWidget,false,true,0);

//flags - replace all
	replaceWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,gettext("Replace All Contents"));
	gtk_widget_set_name(replaceWidget,"replace");
	gtk_box_pack_start(GTK_BOX(vbox),replaceWidget,false,true,0);

//flags - view
	outputWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,gettext("View Output"));
	gtk_widget_set_name(outputWidget,"outtoview");
	gtk_box_pack_start(GTK_BOX(vbox),outputWidget,false,true,0);

//buttons
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),false,false,0);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);
#endif

	//hbox=gtk_hbox_new(false,0);
	hbox=creatNewBox(NEWHBOX,false,0);
	//button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	button=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);

	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"apply");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	//button=gtk_button_new_from_stock(GTK_STOCK_DELETE);
	button=createNewStockButton(GTK_STOCK_DELETE,gettext("Delete"));
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"delete");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	//button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
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
	GtkWidget*		image;
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
	//menuitem=gtk_image_menu_item_new_with_label(gettext("Manage External Tools"));
	menuitem=createNewImageMenuItem(GTK_STOCK_EDIT,gettext("Manage External Tools"));

	//image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
	//gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doMakeTool),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	ptr=toolsList;
	while(ptr!=NULL)
		{
			if( ((toolStruct*)ptr->data)->global==true)
				{
					gotglobal=true;
					//menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
					menuitem=createNewImageMenuItem(NULL,((toolStruct*)ptr->data)->menuName);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);

					if(((toolStruct*)ptr->data)->keyCode>0)
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
			if( ((toolStruct*)ptr->data)->global==false)
				{
					//menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
					menuitem=createNewImageMenuItem(NULL,((toolStruct*)ptr->data)->menuName);

					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);

					if(((toolStruct*)ptr->data)->keyCode>0)
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

void populateStore(void)
{
	ERRDATA
	GdkPixbuf*	pbuf;
	GtkWidget*	image;
	GtkTreeIter iter;
	char*		type;

	image=(GtkWidget*)menuItemOpen;

	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			type=strndup((char*)&toolBarLayout[j],1);
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_NEW,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_NEW,image);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,0,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[0],false);
						break;
					case 'O':
//open+recent
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_OPEN,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_OPEN,image);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,1,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[1],false);
						break;
					case 'S':
//save
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_SAVE,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_SAVE,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,2,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[2],false);
						break;

					case 'X':
//cut
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_CUT,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_CUT,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,3,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[3],false);
						break;
					case 'C':
//copy
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_COPY,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_COPY,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,4,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[4],false);
						break;
					case 'P':
//paste
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_PASTE,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_PASTE,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,5,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[5],false);
						break;
					case 'U':
//undo
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_UNDO,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_UNDO,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,6,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[6],false);
						break;
					case 'R':
//redo
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_REDO,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_REDO,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,7,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[7],false);
						break;
					case 'F':
//find
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_FIND,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_FIND,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,8,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[8],false);
						break;
					case 'G':
//navigation
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_DIALOG_QUESTION,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,9,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[9],false);
						break;
//go back
					case 'B':
						gtk_list_store_append (listStore, &iter);
						//pbuf=gtk_widget_render_icon(image,GTK_STOCK_GO_BACK,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
						pbuf=createNewIcon(GTK_STOCK_GO_BACK,image);

						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,17,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[17],false);
						break;

					case '9':
						gtk_list_store_append (listStore, &iter);
						pbuf=gdk_pixbuf_new_from_file(DATADIR"/pixmaps/num.png",NULL);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,10,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[10],false);
						break;

					case 'A':
						gtk_list_store_append (listStore, &iter);
						pbuf=gdk_pixbuf_new_from_file(DATADIR"/pixmaps/api.png",NULL);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,11,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[11],false);
						break;
					case 'Q':
						gtk_list_store_append (listStore, &iter);
						pbuf=gdk_pixbuf_new_from_file(DATADIR"/pixmaps/qtapi.png",NULL);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,16,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[16],false);
						break;
					case 'D':
						gtk_list_store_append (listStore, &iter);
						pbuf=gdk_pixbuf_new_from_file(DATADIR"/pixmaps/finddef.png",NULL);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,12,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[12],false);
						break;
					case 'L':
						gtk_list_store_append (listStore, &iter);
						pbuf=gdk_pixbuf_new_from_file(DATADIR"/pixmaps/live.png",NULL);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,13,-1);
								g_object_unref(pbuf);
							}
						gtk_widget_set_sensitive((GtkWidget*)tool[13],false);
						break;

					case 's':
						gtk_list_store_append (listStore, &iter);
						pbuf=gdk_pixbuf_new_from_file(DATADIR"/pixmaps/sep.png",NULL);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,14,-1);
								g_object_unref(pbuf);
							}
					break;

					case 'E':
//expander
						gtk_list_store_append (listStore, &iter);
						pbuf=gdk_pixbuf_new_from_file(DATADIR"/pixmaps/expand.png",NULL);
						if(pbuf!=NULL)
							{
								gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,type,BUTTON_NUM,15,-1);
								g_object_unref(pbuf);
							}
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

void addIcon(const char* icon,const char* data,int toolnumber,const char* tooltip)
{
	ERRDATA
	//tool[toolnumber]=gtk_tool_button_new_from_stock(icon);
	tool[toolnumber]=createNewToolItem(icon,NULL);

	gtk_box_pack_start(GTK_BOX(fromHBox),(GtkWidget*)tool[toolnumber],false,false,2);
	g_signal_connect(G_OBJECT(tool[toolnumber]),"clicked",G_CALLBACK(addToToolBar),(void*)data);
	ERRDATA gtk_widget_set_tooltip_text((GtkWidget*)tool[toolnumber],tooltip);
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
	addIcon(GTK_STOCK_NEW,"N",0,gettext("New File"));
	addIcon(GTK_STOCK_OPEN,"O",1,gettext("Open File"));
	addIcon(GTK_STOCK_SAVE,"S",2,gettext("Save File"));
	addIcon(GTK_STOCK_CUT,"X",3,gettext("Cut"));
	addIcon(GTK_STOCK_COPY,"C",4,gettext("Copy"));
	addIcon(GTK_STOCK_PASTE,"P",5,gettext("Paste"));
	addIcon(GTK_STOCK_UNDO,"U",6,gettext("Undo"));
	addIcon(GTK_STOCK_REDO,"R",7,gettext("Redo"));
	addIcon(GTK_STOCK_FIND,"F",8,gettext("Find"));
	addIcon(GTK_STOCK_DIALOG_QUESTION,"G",9,gettext("Go To Definition"));
	addIcon(GTK_STOCK_GO_BACK,"B",17,gettext("Go Back"));

	addPixbuf(DATADIR"/pixmaps/num.png","9",10,gettext("Go To Line"));
	addPixbuf(DATADIR"/pixmaps/api.png","A",11,gettext("Find API In Gtk Docs"));
	addPixbuf(DATADIR"/pixmaps/qtapi.png","Q",16,gettext("Find API In Qt5 Docs"));
	addPixbuf(DATADIR"/pixmaps/finddef.png","D",12,gettext("Search For Define"));
	addPixbuf(DATADIR"/pixmaps/live.png","L",13,gettext("Live Search"));
	addPixbuf(DATADIR"/pixmaps/sep.png","s",14,gettext("Separator"));
	addPixbuf(DATADIR"/pixmaps/expand.png","E",15,gettext("Expander"));
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
// 			str_data=(gchar*)12345;
//			printf("XXXXXXXXXX\n");
//			try{
//free(str_data);
// ERRDATA debugFree(&str_data);  //int oops = *pointer;
//}/catch(...){
//			printf("zzzzzzzzzzzzzzz\n");
//    printf("No problem ;-)\n");
//}
			ERRDATA debugFree(&str_data);
		}
	ERRDATA return(g_string_free(str,false));
}


bool clickIt(GtkWidget* widget,GdkEvent* event,gpointer data)
{
	ERRDATA
	GtkTreePath*	path=NULL;
	GdkModifierType	mask;
	GtkTreeIter		iter;
	int				button;

	gdk_window_get_pointer(NULL,NULL,NULL,&mask);
	path=gtk_icon_view_get_path_at_pos((GtkIconView *)widget,event->button.x,event->button.y);

	if ((GDK_CONTROL_MASK & mask) && (path!=NULL))
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

	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (iconView),GTK_SELECTION_SINGLE);
//	gtk_icon_view_set_orientation (GTK_ICON_VIEW (iconView),GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_item_orientation (GTK_ICON_VIEW (iconView),GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_columns (GTK_ICON_VIEW(iconView),24);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(iconView),TRUE);

	renderer=gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconView),renderer,false);

	renderer=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(iconView),renderer,false);

	gtk_box_pack_start(GTK_BOX(iconViewBox),(GtkWidget*)iconView,false,false,2);
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
			asprintf(&filename,"%s/.KKEdit/keybindings.rc",getenv("HOME"));
			saveVarsToFile(filename,keybindings_rc);
			gtk_widget_hide(keysWindow);
		}
	ERRDATA
}

gboolean setKeyInEntry(GtkEntry* widget,GdkEventKey* event,gpointer data)
{
	ERRDATA
	if((event->type==GDK_KEY_PRESS) && (event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			ERRDATA return(true);
		}

	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
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
			gtk_window_set_title((GtkWindow*)keysWindow,gettext("Define Keyboard Shortcuts"));
			//vbox=gtk_vbox_new(false,8);
			vbox=creatNewBox(NEWVBOX,false,8);

			asprintf(&keycutsinfo,"%s",gettext("To set a custom shortcut:\nClick in the appropriate box and press CONTROL ( and optionally SHIFT ) plus your custom key.\nJust press 'Delete' to remove the shortcut\nClick 'Apply' to keep changes or 'Cancel' to discard any changes."));
			item=gtk_label_new(keycutsinfo);
			gtk_label_set_justify((GtkLabel*)item,GTK_JUSTIFY_CENTER);
			gtk_label_set_line_wrap((GtkLabel*)item,true);
			gtk_box_pack_start(GTK_BOX(vbox),item,false,false,0);
			ERRDATA debugFree(&keycutsinfo);
//functions
			for(loop=0;loop<NUMSHORTCUTS;loop++)
				{
					//hbox=gtk_hbox_new(true,0);
					hbox=creatNewBox(NEWHBOX,true,0);
					shortcuttext[loop]=gettext(shortcuttext[loop]);
					gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(shortcuttext[loop]),true,true,0);
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

			//hbox=gtk_hbox_new(true,4);
			hbox=creatNewBox(NEWHBOX,true,4);
			//item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
			item=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);

			gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
			gtk_widget_set_name(item,"apply");
			g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setKeyCuts),(void*)item);	

			//item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
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
	GtkWidget*	image;

	switch(setimage)
		{
			case STOCKMENU:
				//TODO//
				//widg=gtk_image_menu_item_new_from_stock(stocklabel,NULL);
				widg=createNewImageMenuItem(stocklabel,menulabel);
				break;

			case IMAGEMENU:
				//widg=gtk_image_menu_item_new_with_label(menulabel);
				widg=createNewImageMenuItem(stocklabel,menulabel);

				//image=gtk_image_new_from_stock(stocklabel,GTK_ICON_SIZE_MENU);
				//gtk_image_menu_item_set_image((GtkImageMenuItem *)widg,image);
				break;

			case PIXMAPMENU:
				//widg=gtk_image_menu_item_new_with_label(menulabel);
				widg=createNewImageMenuItem(NULL,menulabel);

				//image=gtk_image_new_from_file(stocklabel);
				//gtk_image_menu_item_set_image((GtkImageMenuItem *)widg,image);
				break;

			case CHECKMENU:
				widg=gtk_check_menu_item_new_with_label(stocklabel);
				//widg=createNewImageMenuItem(NULL,stocklabel);
				gtk_check_menu_item_set_active((GtkCheckMenuItem*)widg,toggle);
				break;

			default:
				widg=createNewImageMenuItem(NULL,stocklabel);
//				widg=gtk_menu_item_new_with_label(stocklabel);
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

void makePrefsDial(int widgnum,const char* label,const char* name,int value,int minvalue,int maxvalue,int posy)
{
#if 0
	ERRDATA
////////////TODO////////////////
//	GtkAlignment*	align;
	GtkWidget*	align;
	GtkWidget*		item;

	//align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	align=gtk_alignment_new(0,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(label));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,posy,posy+1);

	prefsIntWidgets[widgnum]=(GObject*)gtk_adjustment_new(value,minvalue,maxvalue,1,1,0);
	item=gtk_spin_button_new((GtkAdjustment*)prefsIntWidgets[widgnum],1,0);
	gtk_widget_set_name(item,name);
	gtk_table_attach_defaults(table,item,1,2,posy,posy+1);
	ERRDATA
#endif
}

void makePrefsCheck(int widgnum,const char* label,const char* name,bool onoff,int posx,int posy)
{
#if 0
/////////////TODO////////////
	ERRDATA
	prefsWidgets[widgnum]=gtk_check_button_new_with_label(label);
	gtk_widget_set_name(prefsWidgets[widgnum],name);
	gtk_toggle_button_set_active((GtkToggleButton*)prefsWidgets[widgnum],onoff);
	if(posx!=-1)
		gtk_table_attach_defaults(table,prefsWidgets[widgnum],posx,posx+1,posy,posy+1);
	ERRDATA
#endif
}

VISIBLE void doPrefs(GtkWidget* widget,gpointer data)
{
#if 0
/////////////////TODO/////////////
	ERRDATA
	GtkWidget*		vbox;
	GtkWidget*		hbox;
	GtkWidget*		pagevbox;
	GtkWidget*		item;
	GtkWidget*		label;
	GtkNotebook*	prefsnotebook;
//TODO//	GtkAlignment*	align;

	prefswin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)prefswin,"Preferences");
	//vbox=gtk_vbox_new(false,8);
	vbox=creatNewBox(NEWVBOX,false,8);
	//hbox=gtk_hbox_new(false,8);
	hbox=creatNewBox(NEWHBOX,false,8);

//toolbar dnd
	//iconViewBox=gtk_hbox_new(false,0);
	//fromHBox=gtk_hbox_new(false,0);
	iconViewBox=creatNewBox(NEWHBOX,false,0);
	fromHBox=creatNewBox(NEWHBOX,false,0);

	label=gtk_label_new(gettext("<b>Customize Tool Bar</b>"));
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),iconViewBox,true,false,2);
	gtk_box_pack_start(GTK_BOX(vbox),fromHBox,true,false,2);

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
	//pagevbox=gtk_vbox_new(false,0);
	pagevbox=creatNewBox(NEWVBOX,false,0);
//appearence 1
	table=(GtkTable*)gtk_table_new(11,TABLECOLS,true);
//indent
	makePrefsCheck(AUTOINDENT,gettext("Auto Indent Lines"),"indent",indent,0,0);
//linenumbers
	makePrefsCheck(SHOWNUMS,gettext("Show Line Numbers"),"show",lineNumbers,0,1);
//wraplines
	makePrefsCheck(WRAP,gettext("Wrap Lines"),"wrap",lineWrap,0,2);
//highlite
	makePrefsCheck(HIGHLIGHT,gettext("Highlight Current Line"),"high",highLight,0,3);
//no syntax colour
	makePrefsCheck(NOSYNTAX,gettext("No Syntax Highlighting"),"nosyntax",noSyntax,0,4);
//single instance
	makePrefsCheck(USESINGLE,gettext("Use Single Instance"),"single",singleUse,0,5);

//auto save session
	makePrefsCheck(AUTOSAVE,gettext("Auto Save/Restore Session"),"save",onExitSaveSession,0,6);
	g_signal_connect(G_OBJECT(prefsWidgets[AUTOSAVE]),"toggled",G_CALLBACK(setPrefs),(void*)prefsWidgets[AUTOSAVE]);
//auto restore bookmarks
	makePrefsCheck(AUTOBM,gettext("Restore Session Bookmarks"),"marks",restoreBookmarks,1,6);
	gtk_widget_set_sensitive(prefsWidgets[AUTOBM],onExitSaveSession);

//no duplicates
	makePrefsCheck(NODUPLICATE,gettext("Don't Open Duplicate File"),"duplicates",noDuplicates,0,7);
//turn off warnings
	makePrefsCheck(NOWARN,gettext("Don't Warn On File Change"),"warning",noWarnings,0,8);
//do readlink
	makePrefsCheck(READLINK,gettext("Read Link Before Opening File"),"readlink",readLinkFirst,0,9);
//autoshow completion
	makePrefsCheck(AUTOSHOW,gettext("Auto Show Completions"),"autocomp",autoShowComps,0,10);

	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)table,false,false,0);
	gtk_notebook_append_page(prefsnotebook,pagevbox,gtk_label_new(gettext("General Appearance")));

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)prefsnotebook,true,true,0);

//page2
	//pagevbox=gtk_vbox_new(false,0);
	pagevbox=creatNewBox(NEWVBOX,false,0);
//text appearence
//TODO//
//	table=(GtkTable*)gtk_table_new(5,TABLECOLS,true);
//	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);

//tabwidth
	makePrefsDial(TABWIDTH,gettext("Tab width:"),"tabs",tabWidth,2,64,0);

//style
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	int cnt=0;
	int foundname=0;
	const gchar * const * ids=gtk_source_style_scheme_manager_get_scheme_ids(schemeManager);

	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(gettext("Theme:")));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,1,2);

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
	gtk_table_attach_defaults(table,item,1,2,1,2);

//font button
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(gettext("Font:")));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,2,3);

	fontButton=gtk_font_button_new_with_font(fontAndSize);
	gtk_widget_set_name(fontButton,"fontbutton");
	gtk_table_attach_defaults(table,fontButton,1,2,2,3);

//bm highlight colour
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(gettext("Bookmark Highlight Colour:")));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,3,4);

	GdkColor color;
	gdk_color_parse ((const gchar *)highlightColour,&color);
	bmHighlightBox=gtk_color_button_new_with_color(&color);

	gtk_table_attach_defaults(table,bmHighlightBox,1,2,3,4);

//autoshow completion
	makePrefsDial(COMPLETIONSIZE,gettext("Completion Minimum Word Size:"),"minautochars",autoShowMinChars,2,20,4);

	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)table,false,false,0);

//sort functions
	align=(GtkAlignment*)gtk_alignment_new(0.5,1,0,0);

	funcListDrop=gtk_combo_box_text_new();
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,gettext("Display functions etc in menu by type and alphabetically"));
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,gettext("Display functions etc in menu by type and file position"));
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,gettext("Display functions etc in menu by file position"));
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,gettext("Display functions etc in menu alphabetically"));
	gtk_combo_box_text_append_text((GtkComboBoxText*)funcListDrop,gettext("Display functions etc in menu in categorised format"));

	gtk_combo_box_set_active((GtkComboBox*)funcListDrop,listFunction);
	gtk_container_add(GTK_CONTAINER(align),(GtkWidget*)funcListDrop);
	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)align,false,false,16);

	gtk_notebook_append_page(prefsnotebook,pagevbox,gtk_label_new(gettext("Text Style")));

//show keybindings dialog
	align=(GtkAlignment*)gtk_alignment_new(0.5,1,0,0);
	item=gtk_button_new_with_label(gettext("Customize Keyboard Shortcuts"));
	gtk_widget_set_name(item,"makekeys");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(buildKeys),NULL);	
	gtk_container_add(GTK_CONTAINER(align),item);
	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)align,false,false,0);
//end style

//page 3
	//pagevbox=gtk_vbox_new(false,0);
	pagevbox=creatNewBox(NEWVBOX,false,0);
//admin
	table=(GtkTable*)gtk_table_new(10,TABLECOLS,true);
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);

//function search depth
	makePrefsDial(MAXFUNCDEPTH,gettext("Tag File Search Depth:"),"depth",depth,0,20,0);

//terminalcommand
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(gettext("Terminal Command:")));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,1,2);

	terminalBox=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)terminalBox,terminalCommand);
	gtk_table_attach_defaults(table,terminalBox,1,2,1,2);

//root command
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(gettext("Run As Root Command:")));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,2,3);

	rootCommandBox=gtk_entry_new();
	if(rootCommand!=NULL)
		gtk_entry_set_text((GtkEntry*)rootCommandBox,rootCommand);
	gtk_table_attach_defaults(table,rootCommandBox,1,2,2,3);

//set default browser
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(gettext("Default Browser:")));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,3,4);

	defaultBrowserBox=gtk_entry_new();
	gtk_widget_set_name(defaultBrowserBox,"defaultbrowser");
	if(browserCommand!=NULL)
		gtk_entry_set_text((GtkEntry*)defaultBrowserBox,browserCommand);
	gtk_table_attach_defaults(table,defaultBrowserBox,1,2,3,4);

//find replace history max
	makePrefsDial(MAXHISTORY,gettext("Max Find/Replace History:"),"maxfrhistory",maxFRHistory,0,MAXTEXTWIDTH,4);
//max tab label width
	makePrefsDial(MAXTABCHARS,gettext("Max Characters In Tab:"),"maxtabchars",maxTabChars,5,MAXTEXTWIDTH,5);
//max function strings
	makePrefsDial(MENUWIDTH,gettext("Max Characters In Function Defs:"),"maxmenuchars",maxFuncDefs,5,MAXTEXTWIDTH,6);
//max bookmark strings
	makePrefsDial(MAXBMWIDTH,gettext("Max Characters In Bookmarks:"),"maxbmchars",maxBMChars,5,MAXTEXTWIDTH,7);

//check for update
	makePrefsCheck(UPDATECHECK,gettext("Check For Updates"),"updatecheck",autoCheck,0,8);
//use global plug menu
	makePrefsCheck(GLOBALPLUGMENU,gettext("Use Global Plugins Menu ( Requires Restart )"),"useplugmenu",useGlobalPlugMenu,0,9);

	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)table,false,false,0);
	gtk_notebook_append_page(prefsnotebook,pagevbox,gtk_label_new(gettext("Administration")));

//end admin

//nag
	label=gtk_label_new(gettext("<b>Be Kind To Poor Programmers</b>"));
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);
	//hbox=gtk_hbox_new(true,0);
	hbox=creatNewBox(NEWHBOX,true,0);

	makePrefsCheck(BEKIND,gettext("I have donated"),"useplugmenu",nagScreen,-1,-1);

	gtk_box_pack_start(GTK_BOX(hbox),gtk_alignment_new(0,0,1,0),true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),prefsWidgets[BEKIND],true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_alignment_new(0,0,1,0),true,true,0);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new(gettext("I have really donated some some money to the author.\nMy conscience is clear and my Karma is squeaky clean :)")),false,false,0);

//buttons
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,0);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
#endif

	//hbox=gtk_hbox_new(true,4);
	hbox=creatNewBox(NEWHBOX,true,4);
	//item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	item=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);

	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"apply");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);	

	//item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	item=createNewStockButton(GTK_STOCK_CANCEL,GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"cancel");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);

//show it
	gtk_container_add(GTK_CONTAINER(prefswin),(GtkWidget*)vbox);
	gtk_widget_show_all(prefswin);
	ERRDATA
#endif
}

void addRecentToMenu(GtkRecentChooser* chooser,GtkWidget* menu)
{
	ERRDATA
	GList*		itemlist=NULL;
	GList*		l=NULL;
	GtkWidget*	menuitem;
	char*		uri=NULL;
	char*		filename=NULL;
	int			i=0;

	itemlist=gtk_recent_chooser_get_items(chooser);

	for (l=itemlist;l !=NULL;l=l->next)
		{
			const gchar *menuname;
			GtkRecentInfo *info = (GtkRecentInfo*)l->data;
			if (i >= MAXRECENT)
				break;
			i++;

			menuname=gtk_recent_info_get_display_name(info);
			uri=(char*)gtk_recent_info_get_uri(info);
			if (uri!=NULL)
				{
					filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
					//menuitem=gtk_image_menu_item_new_with_label(menuname);
					menuitem=createNewImageMenuItem(NULL,menuname);

					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(recentFileMenu),(void*)filename);
					g_free (uri);
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

//	mainWindowVBox=gtk_vbox_new(false,0);
//	mainTopUserVBox=gtk_vbox_new(false,0);
//	mainLeftUserVBox=gtk_vbox_new(false,0);
//	mainNotebookVBox=gtk_vbox_new(false,0);
//	mainRightUserVBox=gtk_vbox_new(false,0);
//	mainBottomUserVBox=gtk_vbox_new(false,0);

	mainWindowVBox=creatNewBox(NEWVBOX,false,0);
	mainTopUserVBox=creatNewBox(NEWVBOX,false,0);
	mainLeftUserVBox=creatNewBox(NEWVBOX,false,0);
	mainNotebookVBox=creatNewBox(NEWVBOX,false,0);
	mainRightUserVBox=creatNewBox(NEWVBOX,false,0);
	mainBottomUserVBox=creatNewBox(NEWVBOX,false,0);

//	mainWindowHBox=gtk_hbox_new(false,0);
	mainWindowHBox=creatNewBox(NEWHBOX,false,0);

//	mainWindowHPane=gtk_hpaned_new();
//	secondWindowHPane=gtk_hpaned_new();
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

	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(doShutdown),NULL);
	g_signal_connect(G_OBJECT(mainWindow),"key-press-event",G_CALLBACK(keyShortCut),NULL);

	accgroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)mainWindow,accgroup);

	mainNotebook=(GtkNotebook*)gtk_notebook_new();
	gtk_notebook_set_scrollable(mainNotebook,true);
	g_signal_connect(G_OBJECT(mainNotebook),"switch-page",G_CALLBACK(switchPage),NULL);
	g_signal_connect(G_OBJECT(mainNotebook),"page-reordered",G_CALLBACK(switchPage),NULL);

	menuBar=gtk_menu_bar_new();
	//toolBarBox=gtk_hbox_new(true,0);
	toolBarBox=creatNewBox(NEWHBOX,true,0);
	toolBar=(GtkToolbar*)gtk_toolbar_new();

//dnd
	gtk_drag_dest_set(mainWindowVBox,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(mainWindowVBox);
	g_signal_connect(G_OBJECT(mainWindowVBox),"drag_data_received",G_CALLBACK(dropUri),NULL);

	setUpToolBar();
	gtk_box_pack_start(GTK_BOX(toolBarBox),(GtkWidget*)toolBar,true,true,0);

//menus
//file menu
	fileMenu=gtk_menu_item_new_with_label(gettext("_File"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)fileMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenu),menu);

//new
	menuitem=makeMenuItem(GTK_STOCK_NEW,menu,(void*)newFile,'N',NEWMENUNAME,STOCKMENU,GTK_STOCK_NEW3,NULL,false);
//open
	menuItemOpen=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)doOpenFile,'O',OPENMENUNAME,STOCKMENU,GTK_STOCK_OPEN3,NULL,false);
//open as hexdump
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)openAsHexDump,0,HEXDUMPMENUNAME,IMAGEMENU,gettext("Open As Hexdump"),NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//extras
	menuitem=makeMenuItem(DATADIR"/pixmaps/ROOTKKEdit.png",menu,(void*)newEditor,0,NEWADMINMENUNAME,PIXMAPMENU,gettext("New Admin Editor"),(void*)1,false);
	menuitem=makeMenuItem(DATADIR"/pixmaps/MenuKKEdit.png",menu,(void*)newEditor,0,NEWEDITORMENUNAME,PIXMAPMENU,gettext("New Editor"),(void*)2,false);

	if(gotManEditor==0)
		menuitem=makeMenuItem(DATADIR"/pixmaps/ManPageEditor.png",menu,(void*)newEditor,0,MANEDITORMENUNAME,PIXMAPMENU,gettext("Manpage Editor"),(void*)3,false);

//doxy
	if(gotDoxygen==0)
		menuitem=makeMenuItem(GTK_STOCK_COPY,menu,(void*)doDoxy,0,DOXYBUILDMENUNAME,IMAGEMENU,gettext("Build Documentation"),(void*)1,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//recent menu
	//menuitem=gtk_image_menu_item_new_with_label(gettext("Recent Files"));
	menuitem=createNewImageMenuItem(NULL,gettext("Recent Files"));

	menurecent=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),menurecent);
	addRecentToMenu((GtkRecentChooser*)recent,menurecent);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_widget_set_name(menuitem,RECENTMENUNAME);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save
	saveMenu=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)saveFile,'S',SAVEMENUNAME,STOCKMENU,GTK_STOCK_SAVE3,NULL,false);
//savas
	saveAsMenu=makeMenuItem(GTK_STOCK_SAVE_AS,menu,(void*)saveFile,-'S',SAVEASMENUNAME,STOCKMENU,GTK_STOCK_SAVE_AS3,(void*)1,false);
//save all
	saveAllMenu=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)doSaveAll,0,SAVEALLMENUNAME,IMAGEMENU,gettext("Save All"),NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save session
	menuitem=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)saveSession,0,SAVESESSIONMENUNAME,IMAGEMENU,gettext("Save Session"),(void*)false,false);
//restore session
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)restoreSession,0,RESTORESESSIONMENUNAME,IMAGEMENU,gettext("Restore Session"),NULL,false);

//printfile
	printMenu=makeMenuItem(GTK_STOCK_PRINT,menu,(void*)printFile,0,PRINTMENUNAME,STOCKMENU,GTK_STOCK_PRINT3,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//close
	closeMenu=makeMenuItem(GTK_STOCK_CLOSE,menu,(void*)closeTab,'W',CLOSEMENUNAME,STOCKMENU,GTK_STOCK_CLOSE3,NULL,false);
//close-all
	closeAllMenu=makeMenuItem(GTK_STOCK_CLOSE,menu,(void*)closeAllTabs,0,CLOSEALLMENUNAME,IMAGEMENU,gettext("Close All Tabs"),NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//reload file
	revertMenu=makeMenuItem(GTK_STOCK_REVERT_TO_SAVED,menu,(void*)reloadFile,0,REVERTMENUNAME,STOCKMENU,GTK_STOCK_REVERT_TO_SAVED3,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//quit
	menuitem=makeMenuItem(GTK_STOCK_QUIT,menu,(void*)doShutdown,'Q',QUITMENUNAME,STOCKMENU,GTK_STOCK_QUIT3,NULL,false);

//edit menu
	editMenu=gtk_menu_item_new_with_label(gettext("_Edit"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)editMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMenu),menu);
//undo
	undoMenu=makeMenuItem(GTK_STOCK_UNDO,menu,(void*)undo,'Z',UNDOMENUNAME,STOCKMENU,GTK_STOCK_UNDO3,NULL,false);
//redo
	redoMenu=makeMenuItem(GTK_STOCK_REDO,menu,(void*)redo,-'Z',REDOMENUNAME,STOCKMENU,GTK_STOCK_REDO3,NULL,false);
//undoall
	undoAllMenu=makeMenuItem(GTK_STOCK_UNDO,menu,(void*)unRedoAll,0,UNDOALLMENUNAME,IMAGEMENU,gettext("Undo All"),(void*)0,false);
//redoall
	redoAllMenu=makeMenuItem(GTK_STOCK_REDO,menu,(void*)unRedoAll,0,REDOALLMENUNAME,IMAGEMENU,gettext("Redo All"),(void*)1,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//cut
	cutMenu=makeMenuItem(GTK_STOCK_CUT,menu,(void*)cutToClip,'X',CUTMENUNAME,STOCKMENU,GTK_STOCK_CUT3,NULL,false);
//copy
	copyMenu=makeMenuItem(GTK_STOCK_COPY,menu,(void*)copyToClip,'C',COPYMENUNAME,STOCKMENU,GTK_STOCK_COPY3,NULL,false);
//paste
	pasteMenu=makeMenuItem(GTK_STOCK_PASTE,menu,(void*)pasteFromClip,'V',PASTEMENUNAME,STOCKMENU,GTK_STOCK_PASTE3,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//find
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)find,'F',FINDMENUNAME,STOCKMENU,GTK_STOCK_FIND3,NULL,false);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//sort
	sortTabsMenu=makeMenuItem(GTK_STOCK_SORT_ASCENDING,menu,(void*)sortTabs,0,SORTTABSMENUNAME,IMAGEMENU,gettext("Sort Tabs"),NULL,false);
//jump to tab
	GtkWidget*	image;
	//viewTabMenu=gtk_image_menu_item_new_with_label(gettext("Select Tab"));
	viewTabMenu=createNewImageMenuItem(GTK_STOCK_EDIT,gettext("Select Tab"));

	//image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
	//gtk_image_menu_item_set_image((GtkImageMenuItem *)viewTabMenu,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),viewTabMenu);
	rebuildTabsMenu();

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//prefs
	menuitem=makeMenuItem(GTK_STOCK_PREFERENCES,menu,(void*)doPrefs,0,PREFSMENUNAME,STOCKMENU,GTK_STOCK_PREFERENCES3,NULL,false);
//plugs
	menuitem=makeMenuItem(GTK_STOCK_PREFERENCES,menu,(void*)doPlugPrefs,0,PLUGPREFSMENUNAME,IMAGEMENU,gettext("Plugin Prefs"),NULL,false);

//view menu
	viewMenu=gtk_menu_item_new_with_label(gettext("_View"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)viewMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewMenu),menu);

//show docs
	menuitem=makeMenuItem(gettext("Show Documentation"),menu,(void*)doDoxy,0,SHOWDOCSMENUNAME,NORMALMENU,NULL,(void*)2,false);

//toggle bookmark bar
	if(showBMBar)
		menuitem=makeMenuItem(gettext("Hide Bookmarks Bar"),menu,(void*)toggleBookMarkBar,0,SHOWBMBARMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		menuitem=makeMenuItem(gettext("Show Bookmarks Bar"),menu,(void*)toggleBookMarkBar,0,SHOWBMBARMENUNAME,NORMALMENU,NULL,NULL,false);

//toggle toolbar bar
	if(showToolBar)
		menuitem=makeMenuItem(gettext("Hide Tool Bar"),menu,(void*)toggleToolBar,0,SHOWTOOLBARMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		menuitem=makeMenuItem(gettext("Show Tool Bar"),menu,(void*)toggleToolBar,0,SHOWTOOLBARMENUNAME,NORMALMENU,NULL,NULL,false);

//tooloutput
	if(showToolOutWin)
		toolOutMenu=makeMenuItem(gettext("Hide Tool Output"),menu,(void*)toggleToolOutput,0,SHOWTOOLOUTMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		toolOutMenu=makeMenuItem(gettext("Show Tool Output"),menu,(void*)toggleToolOutput,0,SHOWTOOLOUTMENUNAME,NORMALMENU,NULL,NULL,false);
//toggle statusbar
	if(showStatus)
		statusBarMenu=makeMenuItem(gettext("Hide Status Bar"),menu,(void*)toggleStatusBar,0,SHOWSTATUSMENUNAME,NORMALMENU,NULL,NULL,false);
	else
		statusBarMenu=makeMenuItem(gettext("Show Status Bar"),menu,(void*)toggleStatusBar,0,SHOWSTATUSMENUNAME,NORMALMENU,NULL,NULL,false);

#ifdef _BUILDDOCVIEWER_
//toggle docviewer
	showDocViewWidget=makeMenuItem(gettext("Show Docviewer"),menu,(void*)toggleDocviewer,0,SHOWDOCVIEWERMENUNAME,NORMALMENU,NULL,NULL,false);
#endif

	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
//toggle line nubers
		menuitem=makeMenuItem(gettext("Show Line Numbers"),menu,(void*)toggleLineNumbers,0,VIEWSHOWLINENUMERS,CHECKMENU,NULL,NULL,lineNumbers);
//toggle wrap lines
	menuitem=makeMenuItem(gettext("Wrap Lines"),menu,(void*)toggleWrapLines,0,VIEWWRAPLINES,CHECKMENU,NULL,NULL,lineWrap);

//toggle higlight current line
	menuitem=makeMenuItem(gettext("Hightlght Current Line"),menu,(void*)toggleHighlightCurrent,0,VIEWHIGHLIGHT,CHECKMENU,NULL,NULL,highLight);

//toggle syntax highlight
	menuitem=makeMenuItem(gettext("No Syntax Highlighting"),menu,(void*)toggleSyntax,0,VIEWNOSYNTAX,CHECKMENU,NULL,NULL,noSyntax);

//toggle auto show comps
	menuitem=makeMenuItem(gettext("Auto Show Completions"),menu,(void*)toggleAutoComplete,0,VIEWAUTOCOMPLETE,CHECKMENU,NULL,NULL,autoShowComps);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());

//navigation menu
	navMenu=gtk_menu_item_new_with_label(gettext("_Navigation"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)navMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(navMenu),menu);

//goto define
	menuitem=makeMenuItem(GTK_STOCK_DIALOG_QUESTION,menu,(void*)goToDefinition,'D',GOTODEFMENUNAME,IMAGEMENU,gettext("Go To Definition"),NULL,false);
//open include
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)findFile,'I',OPENINCLUDEMENUNAME,IMAGEMENU,gettext("Open Include File"),NULL,false);
//goto line number
	menuitem=makeMenuItem(GTK_STOCK_GO_DOWN,menu,(void*)jumpToLine,0,GOTOLINEMENUNAME,IMAGEMENU,gettext("Go To Line"),NULL,false);
//find define
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)functionSearch,0,SEARCHFORDEFMENUNAME,IMAGEMENU,gettext("Search For Define"),NULL,false);
//find gtkdoc
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)searchGtkDocs,0,SEARCHGTKMENUNAME,IMAGEMENU,gettext("Search In Gtk Docs"),NULL,false);
//find qt5
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)searchQT5Docs,0,SEARCHQT5MENUNAME,IMAGEMENU,gettext("Search In Qt5 Docs"),NULL,false);

//goto doxy docs
	if(gotDoxygen==0)
		menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)doxyDocs,0,SEARCHDOXYMENUNAME,IMAGEMENU,gettext("Find In Documentation"),NULL,false);

//go back
	goBackMenu=makeMenuItem(GTK_STOCK_GO_BACK,menu,(void*)goBack,0,GOBACKMENUNAME,STOCKMENU,GTK_STOCK_GO_BACK3,NULL,false);

//function menu
	funcMenu=gtk_menu_item_new_with_label(gettext("Fun_ctions"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)funcMenu,true);

//newbookmarks
	bookMarkMenu=gtk_menu_item_new_with_label(gettext("_Bookmarks"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)bookMarkMenu,true);
	rebuildBookMarkMenu();

//external tools
	toolsMenu=gtk_menu_item_new_with_label(gettext("_Tools"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)toolsMenu,true);
	buildTools();

//help
	helpMenu=gtk_menu_item_new_with_label(gettext("_Help"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)helpMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMenu),menu);

//about
	menuitem=makeMenuItem(GTK_STOCK_ABOUT,menu,(void*)doAbout,0,ABOUTMENUNAME,STOCKMENU,NULL,NULL,false);
//help
	menuitem=makeMenuItem(GTK_STOCK_HELP,menu,(void*)openHelp,0,HELPMENUNAME,STOCKMENU,NULL,NULL,false);
//get plugins
	menuitem=makeMenuItem(DATADIR"/pixmaps/KKEditPlugMenu.png",menu,(void*)getPlugins,0,GETPLUGSMENUNAME,PIXMAPMENU,gettext("Get Plugins"),NULL,false);

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
			globalPlugMenu=gtk_menu_item_new_with_label(gettext("Plu_gins"));
			gtk_menu_item_set_use_underline((GtkMenuItem*)globalPlugMenu,true);
			plugsubmenu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(globalPlugMenu),plugsubmenu);
			gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),globalPlugMenu);
		}
	else
		globalPlugMenu=NULL;

	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),helpMenu);

//tooloutputwindow
	//mainVPane=gtk_vpaned_new();
#ifdef _USEGTK3_
	mainVPane=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
#else
	mainVPane=gtk_vpaned_new();
#endif
	gtk_container_set_border_width(GTK_CONTAINER(mainVPane),0);
	gtk_paned_add1(GTK_PANED(mainVPane),mainNotebookVBox);

	//toolOutVBox=gtk_vbox_new(false,0);
	toolOutVBox=creatNewBox(NEWVBOX,false,0);

	gtk_paned_add2(GTK_PANED(mainVPane),toolOutVBox);
	mainWindowScrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)mainWindowScrollbox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	toolOutputBuffer=gtk_text_buffer_new(NULL);
	toolOutputView=gtk_text_view_new_with_buffer(toolOutputBuffer);
	gtk_container_add(GTK_CONTAINER(mainWindowScrollbox),(GtkWidget*)toolOutputView);
	gtk_container_add(GTK_CONTAINER(toolOutVBox),(GtkWidget*)mainWindowScrollbox);

//add main vbox to mainWindow
	gtk_container_add((GtkContainer*)mainWindow,mainWindowVBox);
//addmenubar
	gtk_box_pack_start((GtkBox*)mainWindowVBox,menuBar,false,false,0);
//add toolbar
	gtk_box_pack_start((GtkBox*)mainWindowVBox,toolBarBox,false,false,0);

//add left user
	gtk_paned_pack1((GtkPaned*)mainWindowHPane,mainLeftUserVBox,false,true);
	gtk_paned_pack2((GtkPaned*)mainWindowHPane,secondWindowHPane,true,false);

//add mainNotebook
	gtk_box_pack_start((GtkBox*)mainNotebookVBox,(GtkWidget*)mainNotebook,true,true,0);
	gtk_box_pack_start((GtkBox*)mainWindowHBox,mainVPane,true,true,0);
	gtk_paned_pack1((GtkPaned*)secondWindowHPane,mainWindowHBox,true,false);
                                                         
//addright user
	gtk_paned_pack2((GtkPaned*)secondWindowHPane,mainRightUserVBox,false,true);

//vpanes top and bottom
	//mainWindowVPane=gtk_vpaned_new();
	//secondWindowVPane=gtk_vpaned_new();
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

	gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);

	
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
}

void buildFindReplace(void)
{
	ERRDATA
	GtkWidget*	content_area;
	GtkWidget*	replace;
	GtkWidget*	image;
	GtkWidget*	label;
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	item;

	findReplaceDialog=gtk_dialog_new_with_buttons(gettext("Find/Replace"),(GtkWindow*)mainWindow, GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_GO_FORWARD,FINDNEXT,GTK_STOCK_GO_BACK,FINDPREV,gettext("Replace"),REPLACE,NULL);

	g_signal_connect(G_OBJECT(findReplaceDialog),"response",G_CALLBACK(doFindReplace),NULL);
	content_area=gtk_dialog_get_content_area(GTK_DIALOG(findReplaceDialog));
	gtk_dialog_set_default_response((GtkDialog*)findReplaceDialog,FINDNEXT);

	//vbox=gtk_vbox_new(true,0);
	vbox=creatNewBox(NEWVBOX,true,0);
	//hbox=gtk_hbox_new(false,0);
	hbox=creatNewBox(NEWHBOX,false,0);

	label=gtk_label_new(gettext("Find"));
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);


	findDropBox=gtk_combo_box_text_new_with_entry();
	findBox=gtk_bin_get_child((GtkBin*)findDropBox);
	gtk_entry_set_activates_default((GtkEntry*)findBox,true);

	gtk_container_add(GTK_CONTAINER(content_area),findDropBox);
	gtk_widget_show(findDropBox);

	label=gtk_label_new(gettext("Replace With"));
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	replaceDropBox=gtk_combo_box_text_new_with_entry();
	replaceBox=gtk_bin_get_child((GtkBin*)replaceDropBox);

	g_signal_connect_after(G_OBJECT(replaceDropBox),"changed",G_CALLBACK(pasteFRClip),replaceBox);
	gtk_container_add(GTK_CONTAINER(content_area),replaceDropBox);
	gtk_widget_show(replaceDropBox);

//line 1
	item=gtk_check_button_new_with_label(gettext("Case insensitive"));
	gtk_toggle_button_set_active((GtkToggleButton*)item,insensitiveSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)1);

	item=gtk_check_button_new_with_label(gettext("Use Regex"));
	gtk_toggle_button_set_active((GtkToggleButton*)item,useRegex);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)5);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_widget_show_all(vbox);
//line2
	//hbox=gtk_hbox_new(false,0);
	hbox=creatNewBox(NEWHBOX,false,0);

	item=gtk_check_button_new_with_label(gettext("Wrap"));
	gtk_toggle_button_set_active((GtkToggleButton*)item,wrapSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)2);

	item=gtk_check_button_new_with_label(gettext("All Files"));
	gtk_toggle_button_set_active((GtkToggleButton*)item,findInAllFiles);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)4);

	item=gtk_check_button_new_with_label(gettext("Highlight All"));
	gtk_toggle_button_set_active((GtkToggleButton*)item,hightlightAll);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)6);

	item=gtk_check_button_new_with_label(gettext("Replace All"));
	gtk_toggle_button_set_active((GtkToggleButton*)item,replaceAll);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)3);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(content_area),vbox,true,true,0);

	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
	image=gtk_image_new_from_icon_name(GTK_STOCK_FIND_AND_REPLACE,GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image((GtkButton*)replace,image);
	
	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
	if(replaceAll==true)
		gtk_button_set_label((GtkButton*)replace,gettext("Replace All"));

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
	gtk_window_set_title((GtkWindow*)spellCheckWord,gettext("Spell check word"));

	//vbox=gtk_vbox_new(false,0);
	vbox=creatNewBox(NEWVBOX,false,0);
	//hbox=gtk_hbox_new(true,0);
	hbox=creatNewBox(NEWHBOX,true,0);

	sprintf((char*)&labeltext,gettext("Change <i><b>%s</b></i> to: "),badWord);
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

	//hbox=gtk_hbox_new(true,0);
	hbox=creatNewBox(NEWHBOX,true,0);
	//button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	button=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doChangeWord),(gpointer)(long)docflag);

	button=gtk_button_new_with_label(gettext("Ignore"));
	image=gtk_image_new_from_icon_name(GTK_STOCK_ADD,GTK_ICON_SIZE_MENU);
	gtk_button_set_image((GtkButton*)button,image);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)1);

	//button=gtk_button_new_from_stock(GTK_STOCK_ADD);
	button=createNewStockButton(GTK_STOCK_ADD,gettext("_Add"));
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)2);

	//button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
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

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,gettext("Enter Function Name"));

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),gettext("Find Function"));

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
	gtk_window_set_title((GtkWindow*)docView,gettext("Doc Viewer"));

	gtk_window_set_default_size((GtkWindow*)docView,docWindowWidth,docWindowHeight);
	if(docWindowX!=-1 && docWindowY!=-1)
		gtk_window_move((GtkWindow *)docView,docWindowX,docWindowY);

	//vbox=gtk_vbox_new(false,0);
	vbox=creatNewBox(NEWVBOX,false,0);
	//hbox=gtk_hbox_new(false,4);
	hbox=creatNewBox(NEWHBOX,false,4);

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

	gtk_container_add(GTK_CONTAINER(vbox),scrolledWindow);

	//button=gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	button=createNewStockButton(GTK_STOCK_GO_BACK,gettext("_Back"));
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoBack),(void*)webView);	

//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	//button=gtk_button_new_from_stock(GTK_STOCK_HOME);
	button=createNewStockButton(GTK_STOCK_HOME,gettext("_Home"));
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoHome),(void*)webView);	
 
//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	entry=gtk_entry_new();
	//findbutton=gtk_button_new_from_stock(GTK_STOCK_FIND);
	findbutton=createNewStockButton(GTK_STOCK_FIND,gettext("_Find"));
	gtk_box_pack_start(GTK_BOX(hbox),findbutton,false,false,0);
	g_signal_connect(G_OBJECT(findbutton),"clicked",G_CALLBACK(docSearchFromBar),(void*)entry);	

	gtk_box_pack_start(GTK_BOX(hbox),entry,false,true,0);
	g_signal_connect_after(G_OBJECT(entry),"activate",G_CALLBACK(docSearchFromBar),(void*)entry);

	findnextinpage=gtk_button_new_from_stock(GTK_STOCK_GO_DOWN);
	gtk_box_pack_start(GTK_BOX(hbox),findnextinpage,false,false,0);
	g_signal_connect(G_OBJECT(findnextinpage),"clicked",G_CALLBACK(docSearchInPageFoward),(void*)entry);

	findbutton=gtk_button_new_from_stock(GTK_STOCK_GO_UP);
	gtk_box_pack_start(GTK_BOX(hbox),findbutton,false,false,0);
	g_signal_connect(G_OBJECT(findbutton),"clicked",G_CALLBACK(docSearchInPageBack),(void*)entry);

//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	button=gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
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




