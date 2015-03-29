/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include "kkedit-includes.h"

#define TABLECOLS 2

#ifndef _USEQT5_

GtkWidget*		recent;
GtkToolItem*	tool[18]={NULL,};
GtkIconView*	iconView=NULL;
GtkListStore*	listStore=NULL;
GtkWidget*		entries[NUMSHORTCUTS];
GtkTable*		table;

GtkWidget*		prefsWidgets[MAXPREFSWIDGETS];
GtkObject*		prefsIntWidgets[MAXPREFSINTWIDGETS];

GtkWidget		*fromHBox;

#else
QGridLayout		*table;
QWidget			*prefsWidgets[MAXPREFSWIDGETS];
QWidget			*prefsIntWidgets[MAXPREFSINTWIDGETS];
QWidget			*prefsOtherWidgets[MAXPREFSOTHERWIDGETS];
QListWidget		*listWidget;
QToolBar		*fromHBox;
QAction			*tool[18];
//TODO//
#endif

const char* 	shortcuttext[NUMSHORTCUTS]={gettext("Delete Current Line"),gettext("Delete To End Of Line"),gettext("Delete To Beginning Of Line"),gettext("Select Word Under Cursor"),gettext("Delete Word Under Cursor"),gettext("Duplicate Current Line"),gettext("Select Current Line"),gettext("Move Current Line Up"),gettext("Move Current Line Down"),gettext("Select From Cursor To End Of Line"),gettext("Select From Beginning Of Line To Cursor"),gettext("Move Selection Up"),gettext("Move Selection Down"),gettext("Show Completion")};

void findTool(toolStruct* data,char* toolname)
{
#ifndef _USEQT5_
	if(strcmp(toolname,data->menuName)==0)
		selectedToolFromDrop=data;
#endif
}

#ifndef _USEQT5_
void selectToolOptions(GtkWidget* widget,gpointer data)
#else
//TODO//
void selectToolOptions(void)
#endif
{
#ifndef _USEQT5_
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
#endif
}

#ifdef _USEQT5_
QAction* addToolButton(const char* icon,int toolnumber,const char* tooltip,menuCallbackVoid fv,menuCallbackBool fb)
{
	QIcon qicon;
	MenuItemClass* menuitem=new MenuItemClass(icon);
	qicon=QIcon::fromTheme(icon,QIcon(icon));
	menuitem->setIcon(qicon);
	if(fv!=NULL)
		menuitem->setCallBackVoid(fv);
	else
		menuitem->setCallBackBool(fb);
	menuitem->setMenuID(toolnumber);
	menuitem->setToolTip(tooltip);

	return(menuitem);
}
#endif

void setUpToolBar(void)
{
#ifdef _USEQT5_
	toolBar->clear();
	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			switch(toolBarLayout[j])
				{
//new
					case 'N':
						toolBar->addAction(qobject_cast<QAction*>(newMenu));
						break;
//open+recent
					case 'O':
						toolBar->addAction(qobject_cast<QAction*>(openMenu));
						break;
//save
					case 'S':
						toolBar->addAction(qobject_cast<QAction*>(saveMenu));
						break;

					case 's':
						toolBar->addSeparator();
						break;
//cut
					case 'X':
						toolBar->addAction(qobject_cast<QAction*>(cutMenu));
						break;
//copy
					case 'C':
						toolBar->addAction(qobject_cast<QAction*>(copyMenu));
						break;
//paste
					case 'P':
						toolBar->addAction(qobject_cast<QAction*>(pasteMenu));
						break;
//undo
					case 'U':
						toolBar->addAction(qobject_cast<QAction*>(undoMenu));
						break;
//redo
					case 'R':
						toolBar->addAction(qobject_cast<QAction*>(redoMenu));
						break;
//find
					case 'F':
						toolBar->addAction(qobject_cast<QAction*>(findMenu));
						break;
//navigation
					case 'G':
						toolBar->addAction(qobject_cast<QAction*>(goToDefineMenu));
						break;
//go back
					case 'B':
						toolBar->addAction(qobject_cast<QAction*>(goBackMenu));
						break;

					case '9':
						lineNumberWidget=new QLineEdit;
						reinterpret_cast<QLineEdit*>(lineNumberWidget)->setValidator(new QIntValidator);
						lineNumberWidget->setToolTip(gettext("Go To Line"));
						lineNumberWidget->setMaximumWidth(48);
						QObject::connect(((QLineEdit*)lineNumberWidget),&QLineEdit::textChanged,jumpToLineFromBar);
						toolBar->addWidget(lineNumberWidget);
						break;
//find in gtkdoc
					case 'A':
						findApiWidget=new QLineEdit;
						findApiWidget->setToolTip(gettext("Find API In Gtk Docs"));
						QObject::connect(((QLineEdit*)findApiWidget),&QLineEdit::returnPressed,docSearchFromBar);
						toolBar->addWidget(findApiWidget);
						break;

//find in qt5doc
					case 'Q':
						findQtApiWidget=new QLineEdit;
						findQtApiWidget->setToolTip(gettext("Find API In Qt5 Docs"));
						QObject::connect(((QLineEdit*)findQtApiWidget),&QLineEdit::returnPressed,qt5DocSearchFromBar);
						toolBar->addWidget(findQtApiWidget);
						break;
//find in function def
					case 'D':
						findDefWidget=new QLineEdit;
						findDefWidget->setToolTip(gettext("Search For Define"));
						QObject::connect(((QLineEdit*)findDefWidget),&QLineEdit::returnPressed,defSearchFromBar);
						toolBar->addWidget(findDefWidget);
						break;
//livesearch
					case 'L':
						liveSearchWidget=new QLineEdit;
						liveSearchWidget->setToolTip(gettext("Live Search"));
						QObject::connect(((QLineEdit*)liveSearchWidget),&QLineEdit::textChanged,doLiveSearch);
						toolBar->addWidget(liveSearchWidget);
						break;
//expander
					case 'E':
						{
							QHBoxLayout *hbox=new QHBoxLayout;
							QWidget		*widg=new QWidget;
    						hbox->addStretch(1);
    						widg->setLayout(hbox);
    						toolBar->addWidget(widg);
    					}
						break;
				}
		}
#else
	GtkToolItem*		toolbutton;
	GtkRecentFilter*	filter;

	recent=gtk_recent_chooser_menu_new();
	gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recent),false);
	gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent),GTK_RECENT_SORT_MRU);
	gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent),MAXRECENT);

	filter=gtk_recent_filter_new();
	gtk_recent_filter_add_application(filter,"kkedit");
	gtk_recent_chooser_set_filter(GTK_RECENT_CHOOSER(recent),filter);
	g_signal_connect(recent,"item_activated",G_CALLBACK(recentFileMenu),NULL);

	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						newButton=gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
						gtk_toolbar_insert(toolBar,newButton,-1);
						g_signal_connect(G_OBJECT(newButton),"clicked",G_CALLBACK(newFile),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)newButton,gettext("New File"));
						break;
					case 'O':
//open+recent
						openButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_OPEN);
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(openButton),recent);
						g_signal_connect(G_OBJECT(openButton),"clicked",G_CALLBACK(doOpenFile),NULL);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(openButton),gettext("Open Recent File"));
						gtk_toolbar_insert(toolBar,openButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)openButton,gettext("Open File"));
						break;
					case 'S':
//save
						saveButton=gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
						g_signal_connect(G_OBJECT(saveButton),"clicked",G_CALLBACK(saveFile),NULL);
						gtk_toolbar_insert(toolBar,saveButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)saveButton,gettext("Save File"));
						break;
					case 's':
						gtk_toolbar_insert(toolBar,gtk_separator_tool_item_new(),-1);
						break;
					case 'X':
//cut
						cutButton=gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
						g_signal_connect(G_OBJECT(cutButton),"clicked",G_CALLBACK(cutToClip),NULL);
						gtk_toolbar_insert(toolBar,cutButton,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)cutButton,gettext("Cut"));
						break;
					case 'C':
//copy
						copyButton=gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
						gtk_toolbar_insert(toolBar,copyButton,-1);
						g_signal_connect(G_OBJECT(copyButton),"clicked",G_CALLBACK(copyToClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)copyButton,gettext("Copy"));
						break;
					case 'P':
//paste
						pasteButton=gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);
						gtk_toolbar_insert(toolBar,pasteButton,-1);
						g_signal_connect(G_OBJECT(pasteButton),"clicked",G_CALLBACK(pasteFromClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)pasteButton,gettext("Paste"));
						break;
					case 'U':
//undo
						undoButton=gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
						gtk_toolbar_insert(toolBar,undoButton,-1);
						g_signal_connect(G_OBJECT(undoButton),"clicked",G_CALLBACK(undo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)undoButton,gettext("Undo"));
						break;
					case 'R':
//redo
						redoButton=gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
						gtk_toolbar_insert(toolBar,redoButton,-1);
						g_signal_connect(G_OBJECT(redoButton),"clicked",G_CALLBACK(redo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)redoButton,gettext("Redo"));
						break;
					case 'F':
//find
						findButton=gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
						gtk_toolbar_insert(toolBar,findButton,-1);
						g_signal_connect(G_OBJECT(findButton),"clicked",G_CALLBACK(find),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)findButton,gettext("Find/Replace"));
						break;
					case 'G':
//navigation
						gotoDefButton=gtk_tool_button_new_from_stock(GTK_STOCK_DIALOG_QUESTION);
						gtk_toolbar_insert(toolBar,gotoDefButton,-1);
						g_signal_connect(G_OBJECT(gotoDefButton),"clicked",G_CALLBACK(goToDefinition),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoDefButton,gettext("Go To Definition"));
						break;
//go back
					case 'B':
						backButton=gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
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
#endif
}

void addToolToDrop(gpointer data,gpointer user_data)
{
#ifndef _USEQT5_
	if(((toolStruct*)data)->global==false)
		gtk_combo_box_text_append_text((GtkComboBoxText*)toolSelect,((toolStruct*)data)->menuName);
#endif
}

#ifndef _USEQT5_
void fillCombo(GtkComboBoxText* combo)
#else
//TODO//
void fillCombo(void)
#endif
{
#ifndef _USEQT5_
	g_list_foreach(toolsList,addToolToDrop,NULL);
#endif
}

#ifndef _USEQT5_
gboolean getToolKey(GtkEntry* widget,GdkEventKey* event,gpointer data)
#else
//TODO//
gboolean getToolKey(void)
#endif
{
#ifndef _USEQT5_
	if((event->type==GDK_KEY_PRESS) && (event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			return(true);
		}

	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
		gtk_entry_set_text(widget,gdk_keyval_name(event->keyval));

#endif
	return(true);
}

void doMakeTool(void)
{
#ifndef _USEQT5_
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;
	GtkWidget*	toolwin;
	GtkWidget*	infolabel;

	char*		placeholderinfo;

	asprintf(&placeholderinfo,"%s",gettext("PLACEHOLDERS:\n%t - Currently selected text. Passed to command as $KKEDIT_SELECTION\n%f - Filepath of the current document. Passed to command as $KKEDIT_CURRENTFILE\n%d - Directory of the current document or $HOME. Passed to command as $KKEDIT_CURRENTDIR\n%i - The location of the globally installed tools. Passed to command as $KKEDIT_DATADIR\n%h - Tempory file for displaying html in doc viewer. Passed to command as $KKEDIT_HTMLFILE\n%l - Highlihting language. Passed to command as $KKEDIT_SOURCE_LANG"));

	toolwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)toolwin,gettext("Edit External Tools"));
	vbox=gtk_vbox_new(false,8);

//select tool
	toolSelect=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(vbox),toolSelect,false,true,0);
	g_signal_connect(G_OBJECT(toolSelect),"changed",G_CALLBACK(selectToolOptions),NULL);
	fillCombo((GtkComboBoxText*) toolSelect);

//name
	toolNameWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Tool Name:\t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),toolNameWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(toolNameWidget);
	gtk_entry_set_text((GtkEntry*)toolNameWidget,"New Tool");

//command
	commandLineWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Command: \t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commandLineWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(commandLineWidget);
	gtk_entry_set_text((GtkEntry*)commandLineWidget,"");
//key
	keyWidget=gtk_entry_new();
	gtk_widget_show(keyWidget);
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Shortcut:\t\t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),keyWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)keyWidget,"");
	g_signal_connect(G_OBJECT(keyWidget),"key-press-event",G_CALLBACK(getToolKey),NULL);

//comment
	commentWidget=gtk_entry_new();
	gtk_widget_show(commentWidget);
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(gettext("Comment:  \t")),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commentWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)commentWidget,"");

//info
	infolabel=gtk_label_new(placeholderinfo);
	gtk_label_set_selectable((GtkLabel*)infolabel,true);
	gtk_box_pack_start(GTK_BOX(vbox),infolabel,false,false,0);
	free(placeholderinfo);
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
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);

	hbox=gtk_hbox_new(false,0);
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"apply");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"delete");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"cancel");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,false,2);

//show it
	gtk_container_add(GTK_CONTAINER(toolwin),(GtkWidget*)vbox);
	gtk_widget_show_all(toolwin);
#endif
}

void buildTools(void)
{
//TODO//
printf("buildTools\n");
#ifndef _USEQT5_
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
	menuitem=gtk_image_menu_item_new_with_label(gettext("Manage External Tools"));
	image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
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
					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
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
					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
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
#endif
}

#ifdef _USEQT5_
void addIconToList(const char* name,int type)
{
	QIcon icon;
	QListWidgetItem *iconw;

	icon=QIcon::fromTheme(name,QIcon(name));
	iconw=new QListWidgetItem(icon,"",0,type);
	listWidget->addItem(iconw);
}
#endif

void populateStore(void)
{
#ifdef _USEQT5_

	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			switch(toolBarLayout[j])
				{
//new
					case 'N':
						addIconToList("document-new",'N');
						tool[0]->setEnabled(false);
						break;

//open+recent
					case 'O':
						addIconToList("document-open",'O');
						tool[1]->setEnabled(false);
						break;
//save
					case 'S':
						addIconToList("document-save",'S');
						tool[2]->setEnabled(false);
						break;
//cut
					case 'X':
						addIconToList("edit-cut",'X');
						tool[3]->setEnabled(false);
						break;
//copy
					case 'C':
						addIconToList("edit-copy",'C');
						tool[4]->setEnabled(false);
						break;
//paste
					case 'P':
						addIconToList("edit-paste",'P');
						tool[5]->setEnabled(false);
						break;
//undo
					case 'U':
						addIconToList("edit-undo",'U');
						tool[6]->setEnabled(false);
						break;
//redo
					case 'R':
						addIconToList("edit-redo",'R');
						tool[7]->setEnabled(false);
						break;
//find
					case 'F':
						addIconToList("edit-find",'F');
						tool[8]->setEnabled(false);
						break;
//navigation
					case 'G':
						addIconToList("dialog-question",'G');
						tool[9]->setEnabled(false);
						break;
//go back
					case 'B':
						addIconToList("go-previous",'B');
						tool[17]->setEnabled(false);
						break;
//go to line
					case '9':
						addIconToList(DATADIR"/pixmaps/num.png",'9');
						tool[10]->setEnabled(false);
						break;
//find api gtk
					case 'A':
						addIconToList(DATADIR"/pixmaps/api.png",'A');
						tool[11]->setEnabled(false);
						break;
//find api qt5
					case 'Q':
						addIconToList(DATADIR"/pixmaps/qtapi.png",'Q');
						tool[16]->setEnabled(false);
						break;
//find def
					case 'D':
						addIconToList(DATADIR"/pixmaps/finddef.png",'D');
						tool[12]->setEnabled(false);
						break;
//live search
					case 'L':
						addIconToList(DATADIR"/pixmaps/live.png",'L');
						tool[13]->setEnabled(false);
						break;
//seperator
					case 's':
						addIconToList(DATADIR"/pixmaps/sep.png",'s');
						break;
//expander
					case 'E':
						addIconToList(DATADIR"/pixmaps/expand.png",'E');
						break;
				}
		}
#else
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_NEW,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_OPEN,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_SAVE,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_CUT,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_COPY,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_PASTE,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_UNDO,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_REDO,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_FIND,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
						pbuf=gtk_widget_render_icon(image,GTK_STOCK_GO_BACK,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
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
			debugFree(&type,"populateStore type");
		}
#endif
}

void addToToolBar(Widget* widget,uPtr data)
{
	char	*holddata=toolBarLayout;
#ifdef _USEQT5_
	toolBarLayout=(char*)qobject_cast<MenuItemClass*>(widget)->objectName().constData();
	populateStore();
	toolBarLayout=holddata;
#else
	char	*type;

	type=strndup((char*)data,1);
	toolBarLayout=type;
	populateStore();
	toolBarLayout=holddata;
	debugFree(&type,"addToToolBar type");
#endif
}

void addIcon(const char* icon,const char* data,int toolnumber,const char* tooltip)
{
#ifdef _USEQT5_
	QIcon qicon;
	MenuItemClass* menuitem=new MenuItemClass(icon);
	qicon=QIcon::fromTheme(icon,QIcon(icon));
	menuitem->setObjectName(data);
	menuitem->setIcon(qicon);
	menuitem->setCallBackVoid(&addToToolBar);
	menuitem->setMenuID(toolnumber);
	menuitem->setToolTip(tooltip);

	fromHBox->addAction(menuitem);
	tool[toolnumber]=menuitem;
#else
	tool[toolnumber]=gtk_tool_button_new_from_stock(icon);
	gtk_box_pack_start(GTK_BOX(fromHBox),(GtkWidget*)tool[toolnumber],false,false,2);
	g_signal_connect(G_OBJECT(tool[toolnumber]),"clicked",G_CALLBACK(addToToolBar),(void*)data);
	gtk_widget_set_tooltip_text((GtkWidget*)tool[toolnumber],tooltip);
#endif
}

#ifndef _USEQT5_
void addPixbuf(const char* pixbuf,const char* data,int toolnumber,const char* tooltip)
{
	GtkWidget*		image;
	image=gtk_image_new_from_file(pixbuf);

	tool[toolnumber]=gtk_tool_button_new(image,NULL);
	gtk_box_pack_start(GTK_BOX(fromHBox),(GtkWidget*)tool[toolnumber],false,false,2);
	g_signal_connect(G_OBJECT(tool[toolnumber]),"clicked",G_CALLBACK(addToToolBar),(void*)data);
	gtk_widget_set_tooltip_text((GtkWidget*)tool[toolnumber],tooltip);
}
#endif

void populateDnD(void)
{
#ifdef _USEQT5_
	addIcon("document-new","N",0,gettext("New File"));
	addIcon("document-open","O",1,gettext("Open File"));
	addIcon("document-save","S",2,gettext("Save File"));
	addIcon("edit-cut","X",3,gettext("Cut"));
	addIcon("edit-copy","C",4,gettext("Copy"));
	addIcon("edit-paste","P",5,gettext("Paste"));
	addIcon("edit-undo","U",6,gettext("Undo"));
	addIcon("edit-redo","R",7,gettext("Redo"));
	addIcon("edit-find","F",8,gettext("Find"));
	addIcon("dialog-question","G",9,gettext("Go To Definition"));
	addIcon("go-previous","B",17,gettext("Go Back"));
	addIcon(DATADIR"/pixmaps/num.png","9",10,gettext("Go To Line"));
	addIcon(DATADIR"/pixmaps/api.png","A",11,gettext("Find API In Gtk Docs"));
	addIcon(DATADIR"/pixmaps/qtapi.png","Q",16,gettext("Find API In Qt5 Docs"));
	addIcon(DATADIR"/pixmaps/finddef.png","D",12,gettext("Search For Define"));
	addIcon(DATADIR"/pixmaps/live.png","L",13,gettext("Live Search"));
	addIcon(DATADIR"/pixmaps/sep.png","s",14,gettext("Separator"));
	addIcon(DATADIR"/pixmaps/expand.png","E",15,gettext("Expander"));
#else
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
#endif
}

char* makeToolBarList(void)
{
#ifdef _USEQT5_
	GString		*str=g_string_new("");

	for (int j=0;j<listWidget->count();j++)
		g_string_append_c(str,listWidget->item(j)->type());
#else
	GtkTreeIter iter;
	gboolean	valid;
	gchar		*str_data;
	gint		row_count=0;
	GString		*str=g_string_new("");

	valid=gtk_tree_model_get_iter_first((GtkTreeModel *)listStore,&iter);
	while(valid)
		{
			gtk_tree_model_get((GtkTreeModel *)listStore,&iter,TEXT_COLUMN,&str_data,-1);
			g_string_append_c(str,str_data[0]);
			row_count++;
			valid=gtk_tree_model_iter_next((GtkTreeModel *)listStore,&iter);
			free(str_data);
		}
#endif
	return(g_string_free(str,false));
}

#ifndef _USEQT5_
void clickIt(GtkWidget* widget,GdkEvent* event,gpointer data)
#else
void clickIt(QListWidgetItem* item)
#endif
{
#ifdef _USEQT5_
	const char		*data="NOSXCPURFG9ADLsEQB";
	int				row=listWidget->currentRow();
	QListWidgetItem	*titem=listWidget->takeItem(row);

	for(unsigned int j=0;j<strlen(data);j++)
		{
			if(item->type()==data[j])
				{
					tool[j]->setEnabled(true);
					break;
				}
		}
	delete titem;
#else
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
#endif
}

void doIconView(void)
{
#ifndef _USEQT5_
	GtkCellRenderer *renderer;

	listStore=gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_INT);
	iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(iconView),GTK_TREE_MODEL(listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(iconView),0);
	
	populateStore();

	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (iconView),GTK_SELECTION_SINGLE);
	gtk_icon_view_set_orientation (GTK_ICON_VIEW (iconView),GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_columns (GTK_ICON_VIEW (iconView),24);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(iconView),TRUE);

	renderer=gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconView),renderer,false);

	renderer=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(iconView),renderer,false);

	gtk_box_pack_start(GTK_BOX(iconViewBox),(GtkWidget*)iconView,false,false,2);
	g_signal_connect(G_OBJECT(iconView),"button-press-event",G_CALLBACK(clickIt),NULL);
#endif
}

#ifndef _USEQT5_
void setKeyCuts(GtkWidget* widget,gpointer data)
#else
//TODO//
void setKeyCuts(void)
#endif
{
#ifndef _USEQT5_
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
						debugFree(&shortCutStrings[j],"setKeyCuts shortCutStrings");
					asprintf(&shortCutStrings[j],"%i %i - ^%c %s",shortCuts[j][0],shortCuts[j][1],shortCuts[j][0],shortcuttext[j]);
				}
			asprintf(&filename,"%s/" KKEDITFOLDER "/keybindings.rc",getenv("HOME"));
			saveVarsToFile(filename,keybindings_rc);
			gtk_widget_hide(keysWindow);
		}
#endif
}

#ifndef _USEQT5_
gboolean setKeyInEntry(GtkEntry* widget,GdkEventKey* event,gpointer data)
#else
//TODO//
gboolean setKeyInEntry(void)
#endif
{
#ifndef _USEQT5_
	if((event->type==GDK_KEY_PRESS) && (event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			return(true);
		}

	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
		gtk_entry_set_text(widget,gdk_keyval_name(event->keyval));

#endif
	return(true);
}

void buildKeys()
{
#ifndef _USEQT5_
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
			vbox=gtk_vbox_new(false,8);

			asprintf(&keycutsinfo,"%s",gettext("To set a custom shortcut:\nClick in the appropriate box and press CONTROL ( and optionally SHIFT ) plus your custom key.\nJust press 'Delete' to remove the shortcut\nClick 'Apply' to keep changes or 'Cancel' to discard any changes."));
			item=gtk_label_new(keycutsinfo);
			gtk_label_set_justify((GtkLabel*)item,GTK_JUSTIFY_CENTER);
			gtk_label_set_line_wrap((GtkLabel*)item,true);
			gtk_box_pack_start(GTK_BOX(vbox),item,false,false,0);
			free(keycutsinfo);
//functions
			for(loop=0;loop<NUMSHORTCUTS;loop++)
				{
					hbox=gtk_hbox_new(true,0);
					shortcuttext[loop]=gettext(shortcuttext[loop]);
					gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(shortcuttext[loop]),true,true,0);
					entries[loop]=gtk_entry_new();
					g_signal_connect(G_OBJECT(entries[loop]),"key-press-event",G_CALLBACK(setKeyInEntry),NULL);
					gtk_box_pack_start(GTK_BOX(hbox),entries[loop],true,true,0);
					gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
				}
//buttons
			gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

			hbox=gtk_hbox_new(true,4);
			item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
			gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
			gtk_widget_set_name(item,"apply");
			g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setKeyCuts),(void*)item);	

			item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
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
#endif
}

#ifndef _USEQT5_
GtkWidget*	makeMenuItem(const char* stocklabel,GtkWidget* parent,void* function,char hotkey,const char* name,int setimage,const char* menulabel,void* userdata)
{
	GtkWidget*	widg;
	GtkWidget*	image;

	switch(setimage)
		{
			case STOCKMENU:
				widg=gtk_image_menu_item_new_from_stock(stocklabel,NULL);
				break;

			case IMAGEMENU:
				widg=gtk_image_menu_item_new_with_label(menulabel);
				image=gtk_image_new_from_stock(stocklabel,GTK_ICON_SIZE_MENU);
				gtk_image_menu_item_set_image((GtkImageMenuItem *)widg,image);
				break;

			case PIXMAPMENU:
				widg=gtk_image_menu_item_new_with_label(menulabel);
				image=gtk_image_new_from_file(stocklabel);
				gtk_image_menu_item_set_image((GtkImageMenuItem *)widg,image);
				break;

			default:
				widg=gtk_menu_item_new_with_label(stocklabel);
		}

	gtk_menu_shell_append(GTK_MENU_SHELL(parent),widg);
	g_signal_connect(G_OBJECT(widg),"activate",G_CALLBACK(function),userdata);
	
	if(hotkey>0)
		gtk_widget_add_accelerator((GtkWidget *)widg,"activate",accgroup,hotkey,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	if(hotkey<0)
		gtk_widget_add_accelerator((GtkWidget *)widg,"activate",accgroup,hotkey*-1,(GdkModifierType)(GDK_SHIFT_MASK|GDK_CONTROL_MASK),GTK_ACCEL_VISIBLE);

	gtk_widget_set_name(widg,name);
	return(widg);

}
#else
QAction* makeMenuItem(Widget* menu,const char* name,const QKeySequence key,const char* iconname,const char* widgetname,menuCallbackVoid ptrvoid,menuCallbackBool ptrbool,int data)
{
	MenuItemClass* menuitem=new MenuItemClass(name);

	QIcon icon=QIcon::fromTheme(iconname,QIcon(iconname));
	 
	menuitem->setShortcut(key);
	menuitem->setIcon(icon);
	menuitem->setObjectName(widgetname);
	if(ptrvoid!=NULL)
		menuitem->setCallBackVoid(ptrvoid);
	else
		menuitem->setCallBackBool(ptrbool);
	menuitem->setMenuID(data);
	qobject_cast<QMenu*>(menu)->addAction(menuitem);
	
	return(menuitem);
}
#endif

void makePrefsDial(int widgnum,const char* label,const char* name,int value,int minvalue,int maxvalue,int posy)
{
//TODO//
#ifndef _USEQT5_
	GtkAlignment*	align;
	GtkWidget*		item;

	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);
	gtk_container_add(GTK_CONTAINER(align),gtk_label_new(label));
	gtk_table_attach_defaults(table,(GtkWidget*)align,0,1,posy,posy+1);

	prefsIntWidgets[widgnum]=gtk_adjustment_new(value,minvalue,maxvalue,1,1,0);
	item=gtk_spin_button_new((GtkAdjustment*)prefsIntWidgets[widgnum],1,0);
	gtk_widget_set_name(item,name);
	gtk_table_attach_defaults(table,item,1,2,posy,posy+1);
#else
	QLabel*	widgetlabel;
	prefsIntWidgets[widgnum]=new QSpinBox;
	prefsIntWidgets[widgnum]->setProperty("maximum",maxvalue);
	prefsIntWidgets[widgnum]->setProperty("minimum",minvalue);
	prefsIntWidgets[widgnum]->setProperty("value",value);
	widgetlabel=new QLabel(label);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsIntWidgets[widgnum],posy,1,1,-1,Qt::AlignVCenter);
#endif
}

void makePrefsCheck(int widgnum,const char* label,const char* name,bool onoff,int posx,int posy)
{
//TODO//
#ifndef _USEQT5_
	prefsWidgets[widgnum]=gtk_check_button_new_with_label(label);
	gtk_widget_set_name(prefsWidgets[widgnum],name);
	gtk_toggle_button_set_active((GtkToggleButton*)prefsWidgets[widgnum],onoff);
	if(posx!=-1)
		gtk_table_attach_defaults(table,prefsWidgets[widgnum],posx,posx+1,posy,posy+1);
#else
	prefsWidgets[widgnum]=new QCheckBox(label);
	prefsWidgets[widgnum]->setProperty("checked", onoff);
	if(posx!=-1)
		table->addWidget(prefsWidgets[widgnum],posy,posx,Qt::AlignTop);
#endif
}

#ifdef _USEQT5_
void setColor()
{
	const QColor colour=QColorDialog::getColor(QColor(highlightColour),0,"Select Color",0);
	if(colour.isValid())
		{
			prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("palette",QPalette(colour));
			prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("autoFillBackground",true);
			prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("text",colour.name());
		}
}
#endif

#ifdef _USEQT5_
void cancelPrefs(void)
{
	prefsWindow->hide();
	delete prefsWindow;
}
#endif

VISIBLE void doPrefs(Widget* widget,uPtr data)
{
#ifdef _USEQT5_
	QVBoxLayout			*mainvbox=new QVBoxLayout();
	QHBoxLayout			*hbox=new QHBoxLayout;
	QTabWidget			*prefsnotebook=new QTabWidget;
	QWidget				*button;
	QWidget				*tab;
	QLabel				*widgetlabel;
	int					posy;

	listWidget=new QListWidget;
	fromHBox=new QToolBar;
	prefsWindow=new QDialog(mainWindow);

	prefsWindow->setWindowTitle("Preferences");

	populateDnD();
	populateStore();

	QObject::connect(((QListWidget*)listWidget),&QListWidget::itemDoubleClicked,clickIt);

	listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	listWidget->setDragEnabled(true);
	listWidget->setDragDropMode(QAbstractItemView::InternalMove);
	listWidget->viewport()->setAcceptDrops(true);
	listWidget->setFlow(QListView::LeftToRight);
	listWidget->setGridSize(QSize(32,32));
	listWidget->setSizePolicy (QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
	listWidget->setMinimumHeight(48);
	listWidget->setMinimumWidth(32*(strlen(toolBarLayout))+4);

	mainvbox->addWidget(listWidget,2);
	mainvbox->addWidget(fromHBox);
//pages
//page1
	table=new QGridLayout;
	tab=new QWidget();

//appearence 1
//indent
	posy=0;
	makePrefsCheck(AUTOINDENT,gettext("Auto Indent Lines"),"indent",indent,0,posy);
//linenumbers
	posy++;
	makePrefsCheck(SHOWNUMS,gettext("Show Line Numbers"),"show",lineNumbers,0,posy);
//wraplines
	posy++;
	makePrefsCheck(WRAP,gettext("Wrap Lines"),"wrap",lineWrap,0,posy);
//highlite
	posy++;
	makePrefsCheck(HIGHLIGHT,gettext("Highlight Current Line"),"high",highLight,0,posy);
//no syntax colour
	posy++;
	makePrefsCheck(NOSYNTAX,gettext("No Syntax Highlighting"),"nosyntax",noSyntax,0,posy);
//single instance
	posy++;
	makePrefsCheck(USESINGLE,gettext("Use Single Instance"),"single",singleUse,0,posy);

//auto save session
	posy++;
	makePrefsCheck(AUTOSAVE,gettext("Auto Save/Restore Session"),"save",onExitSaveSession,0,posy);
//	g_signal_connect(G_OBJECT(prefsWidgets[AUTOSAVE]),"toggled",G_CALLBACK(setPrefs),(void*)prefsWidgets[AUTOSAVE]);
//auto restore bookmarks
	makePrefsCheck(AUTOBM,gettext("Restore Session Bookmarks"),"marks",restoreBookmarks,1,posy);
//	gtk_widget_set_sensitive(prefsWidgets[AUTOBM],onExitSaveSession);

//no duplicates
	posy++;
	makePrefsCheck(NODUPLICATE,gettext("Don't Open Duplicate File"),"duplicates",noDuplicates,0,posy);
//turn off warnings
	posy++;
	makePrefsCheck(NOWARN,gettext("Don't Warn On File Change"),"warning",noWarnings,0,posy);
//do readlink
	posy++;
	makePrefsCheck(READLINK,gettext("Read Link Before Opening File"),"readlink",readLinkFirst,0,posy);
//autoshow completion
	posy++;
	makePrefsCheck(AUTOSHOW,gettext("Auto show Completions"),"autocomp",autoShowComps,0,posy);

	tab->setLayout(table);
	prefsnotebook->addTab(tab,gettext("General Appearance"));

//page2
	table=new QGridLayout;
	tab=new QWidget();

//tabwidth
	posy=0;
	makePrefsDial(TABWIDTH,gettext("Tab width:"),"tabs",tabWidth,2,64,posy);

//TODO//
//style
	posy++;
	prefsOtherWidgets[THEMECOMBO]=new QComboBox;
	widgetlabel=new QLabel(gettext("Theme:"));
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[THEMECOMBO],posy,1,posy,-1,Qt::AlignVCenter);

//font button
	posy++;
	prefsOtherWidgets[FONTNAMECOMBO]=new QFontComboBox;
	widgetlabel=new QLabel(gettext("Font:"));
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[FONTNAMECOMBO],posy,1,Qt::AlignVCenter);

	int				comboindex=0;
	int				foundsize=0;
	char			fontname[256];
	int				fontsize;
	const char*		stripped;

	sscanf(fontAndSize,"%[^0-9]s",fontname);
	fontsize=atoi(strrchr(fontAndSize,' '));
	stripped=g_strstrip(fontname);
	foundsize=qobject_cast<QComboBox*>(prefsOtherWidgets[FONTNAMECOMBO])->findText(QString(stripped),0);
	prefsOtherWidgets[FONTNAMECOMBO]->setProperty("currentIndex",foundsize);

	prefsOtherWidgets[FONTSIZECOMBO]=new QComboBox;
    prefsOtherWidgets[FONTSIZECOMBO]->setProperty("editable",true);

    QFontDatabase db;
	comboindex=0;
	foreach(int size, db.standardSizes())
		{
			if(size==fontsize)
				foundsize=comboindex;
			qobject_cast<QComboBox*>(prefsOtherWidgets[FONTSIZECOMBO])->addItem(QString::number(size));
			comboindex++;
		}

	prefsOtherWidgets[FONTSIZECOMBO]->setProperty("currentIndex",foundsize);
	table->addWidget(prefsOtherWidgets[FONTSIZECOMBO],posy,2,Qt::AlignVCenter);

//bm highlight colour
	posy++;
    widgetlabel = new QLabel(gettext("Bookmark Highlight Colour:"));
	prefsOtherWidgets[BMHIGHLIGHTCOLOUR]=new QLabel(highlightColour);
	int frameStyle = QFrame::Raised | QFrame::Panel;
    qobject_cast<QLabel*>(prefsOtherWidgets[BMHIGHLIGHTCOLOUR])->setFrameStyle(frameStyle);
    QPushButton *colorButton = new QPushButton("Set Colour");
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[BMHIGHLIGHTCOLOUR],posy,1,Qt::AlignVCenter);
	table->addWidget(colorButton,posy,2,Qt::AlignVCenter);

	QColor colour=QColor(highlightColour);
	prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("palette",QPalette(colour));
	prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("autoFillBackground",true);
	QObject::connect(((QPushButton*)colorButton),&QPushButton::clicked,setColor);

//autoshow completion
	posy++;
	makePrefsDial(COMPLETIONSIZE,gettext("Completion Minimum Word Size:"),"minautochars",autoShowMinChars,2,20,posy);

//sort functions
	posy++;
	prefsOtherWidgets[FUNCTIONCOMBO]=new QComboBox;
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem(gettext("Display functions etc in menu by type and alphabetically"));
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem(gettext("Display functions etc in menu by type and file position"));
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem(gettext("Display functions etc in menu by file position"));
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem(gettext("Display functions etc in menu alphabetically"));
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem(gettext("Display functions etc in menu in categorised format"));
	prefsOtherWidgets[FUNCTIONCOMBO]->setProperty("currentIndex",listFunction);

	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(prefsOtherWidgets[FUNCTIONCOMBO]);
	hbox->setStretch(1,2);
	hbox->addStretch(1);
	table->addLayout(hbox,posy,0,1,-1);

//show keybindings dialog
//TODO//
	posy++;
	hbox=new QHBoxLayout;
    button = new QPushButton(gettext("Customize Keyboard Shortcuts"));
	hbox->addStretch(1);
	hbox->addWidget(button);
	hbox->setStretch(1,2);
	hbox->addStretch(1);
	table->addLayout(hbox,posy,0,1,-1,Qt::AlignVCenter);
	
	QSpacerItem *space=new QSpacerItem(0,0,QSizePolicy::Maximum,QSizePolicy::Maximum);
	posy++;
	table->addItem(space,posy,0,100,-1);

	table->setColumnStretch(1,1);
	tab->setLayout(table);

	prefsnotebook->addTab(tab,gettext("Text Style"));

//page 3
//admin
	table=new QGridLayout;
	tab=new QWidget();
//function search depth
	posy=0;
	makePrefsDial(MAXFUNCDEPTH,gettext("Tag File Search Depth:"),"depth",depth,0,20,posy);

//terminalcommand
	posy++;
    widgetlabel=new QLabel(gettext("Terminal Command:"));
	prefsOtherWidgets[PREFSTERMCOMMAND]=new QLineEdit(terminalCommand);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSTERMCOMMAND],posy,1,1,-1,Qt::AlignVCenter);

//root command
	posy++;
    widgetlabel=new QLabel(gettext("Run As Root Command:"));
	prefsOtherWidgets[PREFSROOTCOMMAND]=new QLineEdit(rootCommand);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSROOTCOMMAND],posy,1,1,-1,Qt::AlignVCenter);

//set default browser
	posy++;
    widgetlabel=new QLabel(gettext("Default Browser:"));
	prefsOtherWidgets[PREFSBROWSERCOMMAND]=new QLineEdit(browserCommand);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSBROWSERCOMMAND],posy,1,1,-1,Qt::AlignVCenter);
	space=new QSpacerItem(0,0,QSizePolicy::Maximum,QSizePolicy::Maximum);

//find replace history max
	posy++;
	makePrefsDial(MAXHISTORY,gettext("Max Find/Replace History:"),"maxfrhistory",maxFRHistory,0,MAXTEXTWIDTH,posy);
//max tab label width
	posy++;
	makePrefsDial(MAXTABCHARS,gettext("Max Characters In Tab:"),"maxtabchars",maxTabChars,5,MAXTEXTWIDTH,posy);
//max function strings
	posy++;
	makePrefsDial(MENUWIDTH,gettext("Max Characters In Function Defs:"),"maxmenuchars",maxFuncDefs,5,MAXTEXTWIDTH,posy);
//max bookmark strings
	posy++;
	makePrefsDial(MAXBMWIDTH,gettext("Max Characters In Bookmarks:"),"maxbmchars",maxBMChars,5,MAXTEXTWIDTH,posy);

//check for update
	posy++;
	makePrefsCheck(UPDATECHECK,gettext("Check For Updates"),"updatecheck",autoCheck,0,posy);
//use global plug menu
	posy++;
	makePrefsCheck(GLOBALPLUGMENU,gettext("Use Global Plugins Menu ( Requires Restart )"),"useplugmenu",useGlobalPlugMenu,0,posy);
//end admin

	posy++;
	table->addItem(space,posy,0,100,-1);

	table->setColumnStretch(1,1);

	tab->setLayout(table);
	prefsnotebook->addTab(tab,gettext("Administration"));

	mainvbox->addWidget(prefsnotebook);
//nag
	posy++;
    widgetlabel=new QLabel(gettext("<b>Be Kind To Poor Programmers</b>"));

	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(widgetlabel);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox,0);

	makePrefsCheck(BEKIND,gettext("I have donated"),"useplugmenu",nagScreen,-1,-1);
	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(prefsWidgets[BEKIND]);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox,0);

    widgetlabel=new QLabel(gettext("I have really donated some some money to the author.\nMy conscience is clear and my Karma is squeaky clean :)"));
	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(widgetlabel);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox,0);
 
    widgetlabel=new QLabel;
    frameStyle=QFrame::Sunken | QFrame::HLine;
	widgetlabel->setFrameStyle(frameStyle);
	mainvbox->addWidget(widgetlabel);

//buttons
	hbox=new QHBoxLayout;
	hbox->addStretch(1);
	button=new QPushButton(gettext("Apply"));
	QObject::connect(((QPushButton*)button),&QPushButton::clicked,setPrefs);
	hbox->addWidget(button);
	hbox->addStretch(1);
	button=new QPushButton(gettext("Cancel"));
	QObject::connect(((QPushButton*)button),&QPushButton::clicked,cancelPrefs);
	hbox->addWidget(button);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox);

	prefsWindow->setLayout(mainvbox);

	prefsWindow->setWindowModality(Qt::WindowModal);
	prefsWindow->show();

#else
	GtkWidget*		vbox;
	GtkWidget*		hbox;
	GtkWidget*		pagevbox;
	GtkWidget*		item;
	GtkWidget*		label;
	GtkNotebook*	prefsnotebook;
	GtkAlignment*	align;

	prefsWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)prefsWindow,"Preferences");
	vbox=gtk_vbox_new(false,8);
	hbox=gtk_hbox_new(false,8);

//toolbar dnd
	iconViewBox=gtk_hbox_new(false,0);
	fromHBox=gtk_hbox_new(false,0);

	label=gtk_label_new(gettext("<b>Customize Tool Bar</b>"));
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),iconViewBox,true,false,2);
	gtk_box_pack_start(GTK_BOX(vbox),fromHBox,true,false,2);

	populateDnD();
	doIconView();
	
//end customize
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

	prefsnotebook=(GtkNotebook*)gtk_notebook_new();

//pages
//page1
	pagevbox=gtk_vbox_new(false,0);
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
	makePrefsCheck(AUTOSHOW,gettext("Auto show Completions"),"autocomp",autoShowComps,0,10);

	gtk_box_pack_start(GTK_BOX(pagevbox),(GtkWidget*)table,false,false,0);
	gtk_notebook_append_page(prefsnotebook,pagevbox,gtk_label_new(gettext("General Appearance")));

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)prefsnotebook,true,true,0);

//page2
	pagevbox=gtk_vbox_new(false,0);
//text appearence
	table=(GtkTable*)gtk_table_new(5,TABLECOLS,true);
	align=(GtkAlignment*)gtk_alignment_new(0,0.5,0,0);

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
	pagevbox=gtk_vbox_new(false,0);
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
	hbox=gtk_hbox_new(true,0);

	makePrefsCheck(BEKIND,gettext("I have donated"),"useplugmenu",nagScreen,-1,-1);

	gtk_box_pack_start(GTK_BOX(hbox),gtk_alignment_new(0,0,1,0),true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),prefsWidgets[BEKIND],true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_alignment_new(0,0,1,0),true,true,0);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new(gettext("I have really donated some some money to the author.\nMy conscience is clear and my Karma is squeaky clean :)")),false,false,0);

//buttons
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

	hbox=gtk_hbox_new(true,4);
	item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"apply");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);	

	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"cancel");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);

//show it
	gtk_container_add(GTK_CONTAINER(prefsWindow),(GtkWidget*)vbox);
	gtk_widget_show_all(prefsWindow);
#endif
}

#ifndef _USEQT5_
void addRecentToMenu(GtkRecentChooser* chooser,GtkWidget* menu)
#else
//TODO//
void addRecentToMenu(void)
#endif
{
#ifndef _USEQT5_
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
					menuitem=gtk_image_menu_item_new_with_label(menuname);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(recentFileMenu),(void*)filename);
					g_free (uri);
				}
		}
#endif
}
#ifdef _USEQT5_
void keyp(QKeyEvent * event)
{
//	closingAll=false;
//	closeTab(NULL,tabnum);
printf("qqqqqqqqqqqqqqq\n");
}
#endif

#ifdef _USEQT5_
void buildMainGuiQT(void)
{
	mainWindow=new QWidget;

	mainWindowVBox=new QVBoxLayout();
	mainTopUserVBox=new QVBoxLayout;
	mainLeftUserVBox=new QVBoxLayout;
	mainNotebookVBox=new QVBoxLayout;
	mainRightUserVBox=new QVBoxLayout;
	mainBottomUserVBox=new QVBoxLayout;
	mainWindowHBox=new QHBoxLayout;

	mainWindow->setGeometry(windowX,windowY,windowWidth,windowHeight);

	mainWindow->setLayout(mainWindowVBox);

	mainNotebook=new QTabWidget;
	qobject_cast<QTabWidget*>(mainNotebook)->setDocumentMode(true);
	qobject_cast<QTabWidget*>(mainNotebook)->setTabsClosable(true);
	qobject_cast<QTabWidget*>(mainNotebook)->setMovable(true);
	
	QObject::connect(qobject_cast<QTabWidget*>(mainNotebook),&QTabWidget::currentChanged,switchPage);
	QObject::connect(qobject_cast<QTabWidget*>(mainNotebook),&QTabWidget::tabCloseRequested,closeTabQT);
//	QObject::connect(qobject_cast<QWidget*>(mainNotebook),&QWidget::keyPressEvent,keyp);

	toolBar=new QToolBar;
//	setUpToolBar();

	menuBar=new QMenuBar;
//file menu
	fileMenu=new QMenu("&File");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(fileMenu));

//new
	newMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("New"),QKeySequence::New,"document-new",NEWMENUNAME,&newFile,NULL,0));
//open
	//openMenu=(Widget*)(makeMenuItem(fileMenu,gettext("Open"),QKeySequence::Open,"document-open",OPENMENUNAME,&doOpenFile,NULL,1));
	openMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Open"),QKeySequence::Open,"document-open",OPENMENUNAME,&doOpenFile,NULL,1));
//open as hexdump
	makeMenuItem(fileMenu,gettext("Open As HeXdump"),0,"document-open",HEXDUMPMENUNAME,&openAsHexDump,NULL,2);

	qobject_cast<QMenu*>(fileMenu)->addSeparator();

//extras
	makeMenuItem(fileMenu,gettext("New Admin Editor"),0,DATADIR"/pixmaps/ROOTKKEdit.png",NEWADMINMENUNAME,&newEditor,NULL,1);
	makeMenuItem(fileMenu,gettext("New Editor"),0,DATADIR"/pixmaps/MenuKKEdit.png",NEWEDITORMENUNAME,&newEditor,NULL,2);
	
	if(gotManEditor==0)
		makeMenuItem(fileMenu,gettext("Manpage Editor"),0,DATADIR"/pixmaps/ManPageEditor.png",MANEDITORMENUNAME,&newEditor,NULL,3);

//doxy
	if(gotDoxygen==0)
		makeMenuItem(fileMenu,gettext("Build Documentation"),0,"edit-copy",DOXYBUILDMENUNAME,&doDoxy,NULL,1);

	qobject_cast<QMenu*>(fileMenu)->addSeparator();

//recent menu
//TODO//
	makeMenuItem(fileMenu,gettext("Recent Files"),0,NULL,RECENTMENUNAME,&newFile,NULL,1000);
	qobject_cast<QMenu*>(fileMenu)->addSeparator();

//save
	saveMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Save"),QKeySequence::Save,"document-save",SAVEMENUNAME,NULL,&saveFile,0));
//savas
	saveAsMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Save As"),QKeySequence::SaveAs,"document-save-as",SAVEASMENUNAME,NULL,&saveFile,1));
//save all
	saveAllMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Save All"),0,"document-save",SAVEALLMENUNAME,NULL,&doSaveAll,0));
	qobject_cast<QMenu*>(fileMenu)->addSeparator();

//save session
	makeMenuItem(fileMenu,gettext("Save Session"),0,"document-save",SAVESESSIONMENUNAME,&saveSession,NULL,0);
//restore session
	makeMenuItem(fileMenu,gettext("Restore Session"),0,"document-open",RESTORESESSIONMENUNAME,&restoreSession,NULL,1);
//restore session and bookmarks
	makeMenuItem(fileMenu,gettext("Restore Session With Bookmark"),0,"document-open",RESTORESESSIONBMMENUNAME,&restoreSession,NULL,0);
//printfile
	printMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Print"),QKeySequence::Print,"document-print",PRINTMENUNAME,&printFile,NULL,0));
	qobject_cast<QMenu*>(fileMenu)->addSeparator();

//close
	closeMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Close"),QKeySequence::Close,"window-close",CLOSEMENUNAME,&closeTab,NULL,0));
//close-all
	closeAllMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Close All Tabs"),0,"window-close",CLOSEALLMENUNAME,&closeAllTabs,NULL,0));
	qobject_cast<QMenu*>(fileMenu)->addSeparator();

//reload file
	revertMenu=reinterpret_cast<Widget*>(makeMenuItem(fileMenu,gettext("Revert"),0,NULL,REVERTMENUNAME,&reloadFile,NULL,0));
	qobject_cast<QMenu*>(fileMenu)->addSeparator();

//quit
	makeMenuItem(fileMenu,gettext("Quit"),QKeySequence::Quit,"application-exit",QUITMENUNAME,&doShutdown,NULL,0);
//////////////////////////////////////////
	
//edit menu
	editMenu=new QMenu("&Edit");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(editMenu));

//undo
	undoMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Undo"),QKeySequence::Undo,"edit-undo",UNDOMENUNAME,&undo,NULL,0));
//redo
	redoMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Redo"),QKeySequence::Redo,"edit-redo",REDOMENUNAME,&redo,NULL,0));
//undoall
	undoAllMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Undo All"),0,"edit-undo",UNDOALLMENUNAME,&unRedoAll,NULL,0));
//redoall
	redoAllMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Redo All"),0,"edit-redo",REDOALLMENUNAME,&unRedoAll,NULL,1));
	qobject_cast<QMenu*>(editMenu)->addSeparator();

//cut
	cutMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Cut"),QKeySequence::Cut,"edit-cut",CUTMENUNAME,&cutToClip,NULL,0));
//copy
	copyMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Copy"),QKeySequence::Copy,"edit-copy",COPYMENUNAME,&copyToClip,NULL,0));
//paste
	pasteMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Paste"),QKeySequence::Paste,"edit-paste",PASTEMENUNAME,&pasteFromClip,NULL,0));
	qobject_cast<QMenu*>(editMenu)->addSeparator();

//find
	findMenu=reinterpret_cast<Widget*>(makeMenuItem(editMenu,gettext("Find"),QKeySequence::Find,"edit-find",FINDMENUNAME,&find,NULL,0));
	qobject_cast<QMenu*>(editMenu)->addSeparator();

//prefs
	makeMenuItem(editMenu,gettext("Preferences"),QKeySequence::Preferences,"preferences-desktop",PREFSMENUNAME,&doPrefs,NULL,0);
//plugs
	makeMenuItem(editMenu,gettext("Plugin Prefs"),0,"preferences-system",PLUGPREFSMENUNAME,&doPlugPrefs,NULL,0);
//////////////////////////////////////////

//view menu
	viewMenu=new QMenu("&View");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(viewMenu));

//show docs
	makeMenuItem(viewMenu,gettext("Show Documentaion"),0,NULL,SHOWDOCSMENUNAME,&doDoxy,NULL,2);

//toggle bookmark bar
	if(showBMBar)
		makeMenuItem(viewMenu,gettext("Hide Bookmarks Bar"),0,NULL,SHOWBMBARMENUNAME,&toggleBookMarkBar,NULL,0);
	else
		makeMenuItem(viewMenu,gettext("Show Bookmarks Bar"),0,NULL,SHOWBMBARMENUNAME,&toggleBookMarkBar,NULL,0);
//toggle toolbar bar
	if(showToolBar)
		makeMenuItem(viewMenu,gettext("Hide Tool Bar"),0,NULL,SHOWTOOLBARMENUNAME,&toggleToolBar,NULL,0);
	else
		makeMenuItem(viewMenu,gettext("Show Tool Bar"),0,NULL,SHOWTOOLBARMENUNAME,&toggleToolBar,NULL,0);
//tooloutput
	if(showToolOutWin)
			toolOutMenu=(Widget*)makeMenuItem(viewMenu,gettext("Hide Tool Output"),0,NULL,SHOWTOOLOUTMENUNAME,&toggleToolOutput,NULL,0);
	else
			toolOutMenu=(Widget*)makeMenuItem(viewMenu,gettext("Show Tool Output"),0,NULL,SHOWTOOLOUTMENUNAME,&toggleToolOutput,NULL,0);
//toggle statusbar
	if(showStatus)
		statusBarMenu=(Widget*)makeMenuItem(viewMenu,gettext("Hide Status Bar"),0,NULL,SHOWSTATUSMENUNAME,&toggleStatusBar,NULL,0);
	else
		statusBarMenu=(Widget*)makeMenuItem(viewMenu,gettext("Show Status Bar"),0,NULL,SHOWSTATUSMENUNAME,&toggleStatusBar,NULL,0);

//#ifdef _USEQT5_
//	showDocViewWidget=(Widget*)makeMenuItem(viewMenu,gettext("Show Docviewer"),0,NULL,SHOWDOCVIEWERMENUNAME,&toggleDocviewer,NULL,0);
//#endif

#ifdef _BUILDDOCVIEWER_
//toggle docviewer
	showDocViewWidget=(Widget*)makeMenuItem(viewMenu,gettext("Show Docviewer"),0,NULL,SHOWDOCVIEWERMENUNAME,&toggleDocviewer,NULL,0);
#endif
//////////////////////////////////////////

//navigation menu
	navMenu=new QMenu("&Navigation");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(navMenu));

//goto define
	goToDefineMenu=reinterpret_cast<Widget*>(makeMenuItem(navMenu,gettext("Go To Definition"),QKeySequence::fromString("Ctrl+D"),"dialog-question",GOTODEFMENUNAME,&goToDefinition,NULL,0));

//open include
	makeMenuItem(navMenu,gettext("Open Include File"),QKeySequence::fromString("Ctrl+I"),"document-open",OPENINCLUDEMENUNAME,&findFile,NULL,0);
//goto line number
	makeMenuItem(navMenu,gettext("Go To Line"),0,"go-down",GOTOLINEMENUNAME,&jumpToLine,NULL,0);
//find define
	makeMenuItem(navMenu,gettext("Search For Define"),0,"edit-find",SEARCHFORDEFMENUNAME,&functionSearch,NULL,0);
//find gtkdoc
	makeMenuItem(navMenu,gettext("Search In Gtk Docs"),0,"edit-find",SEARCHGTKMENUNAME,&searchGtkDocs,NULL,0);
//find qt5
	makeMenuItem(navMenu,gettext("Search In Qt5 Docs"),0,"edit-find",SEARCHQT5MENUNAME,&searchQT5Docs,NULL,0);
//goto doxy docs
	if(gotDoxygen==0)
		makeMenuItem(navMenu,gettext("Find In Documentation"),0,"edit-find",SEARCHDOXYMENUNAME,&doxyDocs,NULL,0);
////go back
		goBackMenu=reinterpret_cast<Widget*>(makeMenuItem(navMenu,gettext("Back"),0,"go-previous",GOBACKMENUNAME,&goBack,NULL,0));
	qobject_cast<QMenu*>(navMenu)->addSeparator();
//////////////////////////////////////////

//function menu
	funcMenu=new QMenu("Fun&ctions");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(funcMenu));

///////////////////////////////////////////

//newbookmarks
	bookMarkMenu=new QMenu("&Bookmarks");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(bookMarkMenu));
	rebuildBookMarkMenu();

///////////////////////////////////////////

//external tools	
	toolsMenu=new QMenu("&Tools");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(toolsMenu));
	buildTools();

///////////////////////////////////////////

//help
	helpMenu=new QMenu("&Help");
	qobject_cast<QMenuBar*>(menuBar)->addMenu(qobject_cast<QMenu*>(helpMenu));

//about
	makeMenuItem(helpMenu,gettext("&About"),0,"help-about",ABOUTMENUNAME,&doAbout,NULL,0);
//help
	makeMenuItem(helpMenu,gettext("&Help"),0,"help-contents",HELPMENUNAME,&openHelp,NULL,0);

//	menuitem=makeMenuItem(GTK_STOCK_HELP,menu,(void*)openHelp,0,HELPMENUNAME,STOCKMENU,NULL,NULL);
//get plugins
	makeMenuItem(helpMenu,gettext("&Get Plugins"),0,DATADIR"/pixmaps/KKEditPlugMenu.png",GETPLUGSMENUNAME,&getPlugins,NULL,0);
//	menuitem=makeMenuItem(DATADIR"/pixmaps/KKEditPlugMenu.png",menu,(void*)getPlugins,0,GETPLUGSMENUNAME,PIXMAPMENU,gettext("Get Plugins"),NULL);

///////////////////////////////////////////
	setUpToolBar();

	mainWindowVBox->addWidget(menuBar);
	mainWindowVBox->addWidget(toolBar);
	mainWindowVBox->addWidget(mainNotebook);

 	mainWindow->show();
}
#endif

#ifndef _USEQT5_
void buildMainGui(void)
{
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GtkWidget*		menurecent;
	GtkWidget*		plugsubmenu=NULL;

	mainWindowVBox=gtk_vbox_new(false,0);
	mainTopUserVBox=gtk_vbox_new(false,0);
	mainLeftUserVBox=gtk_vbox_new(false,0);
	mainNotebookVBox=gtk_vbox_new(false,0);
	mainRightUserVBox=gtk_vbox_new(false,0);
	mainBottomUserVBox=gtk_vbox_new(false,0);

	mainWindowHBox=gtk_hbox_new(false,0);
	mainWindowHPane=gtk_hpaned_new();
	secondWindowHPane=gtk_hpaned_new();

	mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)mainWindow,windowWidth,windowHeight);
	if(windowX!=-1 && windowY!=-1)
		gtk_window_move((GtkWindow *)mainWindow,windowX,windowY);

	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(doShutdown),NULL);
	g_signal_connect(G_OBJECT(mainWindow),"key-press-event",G_CALLBACK(keyShortCut),NULL);

	accgroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)mainWindow,accgroup);

	mainNotebook=gtk_notebook_new();
	gtk_notebook_set_scrollable((GtkNotebook*)mainNotebook,true);
	g_signal_connect(G_OBJECT((GtkNotebook*)mainNotebook),"switch-page",G_CALLBACK(switchPage),NULL);
	g_signal_connect(G_OBJECT((GtkNotebook*)mainNotebook),"page-reordered",G_CALLBACK(switchPage),NULL);

	menuBar=gtk_menu_bar_new();
	toolBarBox=gtk_hbox_new(true,0);
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
	menuitem=makeMenuItem(GTK_STOCK_NEW,menu,(void*)newFile,'N',NEWMENUNAME,STOCKMENU,NULL,NULL);
//open
	menuItemOpen=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)doOpenFile,'O',OPENMENUNAME,STOCKMENU,NULL,NULL);
//open as hexdump
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)openAsHexDump,0,HEXDUMPMENUNAME,IMAGEMENU,gettext("Open As Hexdump"),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//extras
	menuitem=makeMenuItem(DATADIR"/pixmaps/ROOTKKEdit.png",menu,(void*)newEditor,0,NEWADMINMENUNAME,PIXMAPMENU,gettext("New Admin Editor"),(void*)1);
	menuitem=makeMenuItem(DATADIR"/pixmaps/MenuKKEdit.png",menu,(void*)newEditor,0,NEWEDITORMENUNAME,PIXMAPMENU,gettext("New Editor"),(void*)2);

	if(gotManEditor==0)
		menuitem=makeMenuItem(DATADIR"/pixmaps/ManPageEditor.png",menu,(void*)newEditor,0,MANEDITORMENUNAME,PIXMAPMENU,gettext("Manpage Editor"),(void*)3);

//doxy
	if(gotDoxygen==0)
		menuitem=makeMenuItem(GTK_STOCK_COPY,menu,(void*)doDoxy,0,DOXYBUILDMENUNAME,IMAGEMENU,gettext("Build Documentation"),(void*)1);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//recent menu
	menuitem=gtk_image_menu_item_new_with_label(gettext("Recent Files"));
	menurecent=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),menurecent);
	addRecentToMenu((GtkRecentChooser*)recent,menurecent);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_widget_set_name(menuitem,RECENTMENUNAME);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save
	saveMenu=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)saveFile,'S',SAVEMENUNAME,STOCKMENU,NULL,NULL);
//savas
	saveAsMenu=makeMenuItem(GTK_STOCK_SAVE_AS,menu,(void*)saveFile,-'S',SAVEASMENUNAME,STOCKMENU,NULL,(void*)1);
//save all
	saveAllMenu=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)doSaveAll,0,SAVEALLMENUNAME,IMAGEMENU,gettext("Save All"),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save session
	menuitem=makeMenuItem(GTK_STOCK_SAVE,menu,(void*)saveSession,0,SAVESESSIONMENUNAME,IMAGEMENU,gettext("Save Session"),(void*)false);
//restore session
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)restoreSession,0,RESTORESESSIONMENUNAME,IMAGEMENU,gettext("Restore Session"),(void*)false);
//restore session and bookmarks
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)restoreSession,0,RESTORESESSIONBMMENUNAME,IMAGEMENU,gettext("Restore Session With Bookmarks"),(void*)true);
//printfile
	printMenu=makeMenuItem(GTK_STOCK_PRINT,menu,(void*)printFile,0,PRINTMENUNAME,STOCKMENU,NULL,NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//close
	closeMenu=makeMenuItem(GTK_STOCK_CLOSE,menu,(void*)closeTab,'W',CLOSEMENUNAME,STOCKMENU,NULL,NULL);
//close-all
	closeAllMenu=makeMenuItem(GTK_STOCK_CLOSE,menu,(void*)closeAllTabs,0,CLOSEALLMENUNAME,IMAGEMENU,gettext("Close All Tabs"),NULL);
//sort
	sortTabsMenu=makeMenuItem(GTK_STOCK_SORT_ASCENDING,menu,(void*)sortTabs,0,SORTTABSMENUNAME,IMAGEMENU,gettext("Sort Tabs"),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//reload file
	revertMenu=makeMenuItem(GTK_STOCK_REVERT_TO_SAVED,menu,(void*)reloadFile,0,REVERTMENUNAME,STOCKMENU,NULL,NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//quit
	menuitem=makeMenuItem(GTK_STOCK_QUIT,menu,(void*)doShutdown,'Q',QUITMENUNAME,STOCKMENU,NULL,NULL);

//edit menu
	editMenu=gtk_menu_item_new_with_label(gettext("_Edit"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)editMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMenu),menu);
//undo
	undoMenu=makeMenuItem(GTK_STOCK_UNDO,menu,(void*)undo,'Z',UNDOMENUNAME,STOCKMENU,NULL,NULL);
//redo
	redoMenu=makeMenuItem(GTK_STOCK_REDO,menu,(void*)redo,-'Z',REDOMENUNAME,STOCKMENU,NULL,NULL);
//undoall
	undoAllMenu=makeMenuItem(GTK_STOCK_UNDO,menu,(void*)unRedoAll,0,UNDOALLMENUNAME,IMAGEMENU,gettext("Undo All"),(void*)0);
//redoall
	redoAllMenu=makeMenuItem(GTK_STOCK_REDO,menu,(void*)unRedoAll,0,REDOALLMENUNAME,IMAGEMENU,gettext("Redo All"),(void*)1);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//cut
	cutMenu=makeMenuItem(GTK_STOCK_CUT,menu,(void*)cutToClip,'X',CUTMENUNAME,STOCKMENU,NULL,NULL);
//copy
	copyMenu=makeMenuItem(GTK_STOCK_COPY,menu,(void*)copyToClip,'C',COPYMENUNAME,STOCKMENU,NULL,NULL);
//paste
	pasteMenu=makeMenuItem(GTK_STOCK_PASTE,menu,(void*)pasteFromClip,'V',PASTEMENUNAME,STOCKMENU,NULL,NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//find
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)find,'F',FINDMENUNAME,STOCKMENU,NULL,NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//prefs
	menuitem=makeMenuItem(GTK_STOCK_PREFERENCES,menu,(void*)doPrefs,0,PREFSMENUNAME,STOCKMENU,NULL,NULL);
//plugs
	menuitem=makeMenuItem(GTK_STOCK_PREFERENCES,menu,(void*)doPlugPrefs,0,PLUGPREFSMENUNAME,IMAGEMENU,gettext("Plugin Prefs"),NULL);

//view menu
	viewMenu=gtk_menu_item_new_with_label(gettext("_View"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)viewMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewMenu),menu);

//show docs
	menuitem=makeMenuItem(gettext("Show Documentaion"),menu,(void*)doDoxy,0,SHOWDOCSMENUNAME,NORMALMENU,NULL,(void*)2);
//toggle bookmark bar
	if(showBMBar)
		menuitem=makeMenuItem(gettext("Hide Bookmarks Bar"),menu,(void*)toggleBookMarkBar,0,SHOWBMBARMENUNAME,NORMALMENU,NULL,NULL);
	else
		menuitem=makeMenuItem(gettext("Show Bookmarks Bar"),menu,(void*)toggleBookMarkBar,0,SHOWBMBARMENUNAME,NORMALMENU,NULL,NULL);
//toggle toolbar bar
	if(showToolBar)
		menuitem=makeMenuItem(gettext("Hide Tool Bar"),menu,(void*)toggleToolBar,0,SHOWTOOLBARMENUNAME,NORMALMENU,NULL,NULL);
	else
		menuitem=makeMenuItem(gettext("Show Tool Bar"),menu,(void*)toggleToolBar,0,SHOWTOOLBARMENUNAME,NORMALMENU,NULL,NULL);
//tooloutput
	if(showToolOutWin)
		toolOutMenu=makeMenuItem(gettext("Hide Tool Output"),menu,(void*)toggleToolOutput,0,SHOWTOOLOUTMENUNAME,NORMALMENU,NULL,NULL);
	else
		toolOutMenu=makeMenuItem(gettext("Show Tool Output"),menu,(void*)toggleToolOutput,0,SHOWTOOLOUTMENUNAME,NORMALMENU,NULL,NULL);
//toggle statusbar
	if(showStatus)
		statusBarMenu=makeMenuItem(gettext("Hide Status Bar"),menu,(void*)toggleStatusBar,0,SHOWSTATUSMENUNAME,NORMALMENU,NULL,NULL);
	else
		statusBarMenu=makeMenuItem(gettext("Show Status Bar"),menu,(void*)toggleStatusBar,0,SHOWSTATUSMENUNAME,NORMALMENU,NULL,NULL);

#ifdef _BUILDDOCVIEWER_
//toggle docviewer
	showDocViewWidget=makeMenuItem(gettext("Show Docviewer"),menu,(void*)toggleDocviewer,0,SHOWDOCVIEWERMENUNAME,NORMALMENU,NULL,NULL);
#endif

//navigation menu
	navMenu=gtk_menu_item_new_with_label(gettext("_Navigation"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)navMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(navMenu),menu);

//goto define
	menuitem=makeMenuItem(GTK_STOCK_DIALOG_QUESTION,menu,(void*)goToDefinition,'D',GOTODEFMENUNAME,IMAGEMENU,gettext("Go To Definition"),NULL);
//open include
	menuitem=makeMenuItem(GTK_STOCK_OPEN,menu,(void*)findFile,'I',OPENINCLUDEMENUNAME,IMAGEMENU,gettext("Open Include File"),NULL);
//goto line number
	menuitem=makeMenuItem(GTK_STOCK_GO_DOWN,menu,(void*)jumpToLine,0,GOTOLINEMENUNAME,IMAGEMENU,gettext("Go To Line"),NULL);
//find define
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)functionSearch,0,SEARCHFORDEFMENUNAME,IMAGEMENU,gettext("Search For Define"),NULL);
//find gtkdoc
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)searchGtkDocs,0,SEARCHGTKMENUNAME,IMAGEMENU,gettext("Search In Gtk Docs"),NULL);
//find qt5
	menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)searchQT5Docs,0,SEARCHQT5MENUNAME,IMAGEMENU,gettext("Search In Qt5 Docs"),NULL);

//goto doxy docs
	if(gotDoxygen==0)
		menuitem=makeMenuItem(GTK_STOCK_FIND,menu,(void*)doxyDocs,0,SEARCHDOXYMENUNAME,IMAGEMENU,gettext("Find In Documentation"),NULL);

//go back
	goBackMenu=makeMenuItem(GTK_STOCK_GO_BACK,menu,(void*)goBack,0,GOBACKMENUNAME,STOCKMENU,NULL,NULL);

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
	menuitem=makeMenuItem(GTK_STOCK_ABOUT,menu,(void*)doAbout,0,ABOUTMENUNAME,STOCKMENU,NULL,NULL);
//help
	menuitem=makeMenuItem(GTK_STOCK_HELP,menu,(void*)openHelp,0,HELPMENUNAME,STOCKMENU,NULL,NULL);
//get plugins
	menuitem=makeMenuItem(DATADIR"/pixmaps/KKEditPlugMenu.png",menu,(void*)getPlugins,0,GETPLUGSMENUNAME,PIXMAPMENU,gettext("Get Plugins"),NULL);

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
	mainVPane=gtk_vpaned_new();
	gtk_container_set_border_width(GTK_CONTAINER(mainVPane),0);
	gtk_paned_add1(GTK_PANED(mainVPane),mainNotebookVBox);

	toolOutVBox=gtk_vbox_new(false,0);
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
	mainWindowVPane=gtk_vpaned_new();
	secondWindowVPane=gtk_vpaned_new();

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
}
#endif
void doneDialog(int state)
{
printf(">>%i<<\n",state);
//	spellCheckWord=NULL;
}

#ifdef _USEQT5_
void doFindForwardWrap(void)
{
	doFindReplace(FINDNEXT);
}
void doFindBackWrap(void)
{
	doFindReplace(FINDPREV);
}
void doReplaceWrap(void)
{
	doFindReplace(REPLACE);
}
#endif

void buildFindReplace(void)
{
#ifdef _USEQT5_
	QVBoxLayout*	vlayout=new QVBoxLayout;
	QWidget*		hbox;
	QHBoxLayout*	hlayout;
	QLabel			*label;
	QPushButton		*button;
	QIcon			icon;

	findReplaceDialog=new QDialog(mainWindow);
	findReplaceDialog->setWindowTitle(gettext("Find/Replace"));
	vlayout->setContentsMargins(4,0,4,0);
//	QObject::connect((QDialog*)spellCheckWord,&QDialog::finished,doneDialog);

//find
	label=new QLabel(gettext("Find"));
	label->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(label);

	findDropBox=new QComboBox;
	reinterpret_cast<QComboBox*>(findDropBox)->setEditable(true);
	reinterpret_cast<QComboBox*>(findDropBox)->setCompleter(0);
	vlayout->addWidget(findDropBox);

//replace
	label=new QLabel(gettext("Replace With"));
	label->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(label);

	replaceDropBox=new QComboBox;
	reinterpret_cast<QComboBox*>(replaceDropBox)->setEditable(true);
	reinterpret_cast<QComboBox*>(replaceDropBox)->setCompleter(0);
	vlayout->addWidget(replaceDropBox);

//switches 1st row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

//case
	frSwitches[FRCASE]=new QCheckBox(gettext("Case insensitive"));
	reinterpret_cast<QCheckBox*>(frSwitches[FRCASE])->setChecked(insensitiveSearch);
	QObject::connect((QCheckBox*)frSwitches[FRCASE],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(frSwitches[FRCASE]);
//use regex
	frSwitches[FRUSEREGEX]=new QCheckBox(gettext("Use Regex"));
	reinterpret_cast<QCheckBox*>(frSwitches[FRUSEREGEX])->setChecked(useRegex);
	QObject::connect((QCheckBox*)frSwitches[FRUSEREGEX],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(frSwitches[FRUSEREGEX]);

	vlayout->addWidget(hbox);

//switches 2nd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);
//wrap
	frSwitches[FRWRAP]=new QCheckBox(gettext("Wrap"));
	reinterpret_cast<QCheckBox*>(frSwitches[FRWRAP])->setChecked(wrapSearch);
	QObject::connect((QCheckBox*)frSwitches[FRWRAP],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(frSwitches[FRWRAP]);

//all files
	frSwitches[FRALLFILES]=new QCheckBox(gettext("All Files"));
	reinterpret_cast<QCheckBox*>(frSwitches[FRALLFILES])->setChecked(findInAllFiles);
	QObject::connect((QCheckBox*)frSwitches[FRALLFILES],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(frSwitches[FRALLFILES]);
//hilite all
	frSwitches[FRHIGHLIGHTALL]=new QCheckBox(gettext("Highlight All"));
	reinterpret_cast<QCheckBox*>(frSwitches[FRHIGHLIGHTALL])->setChecked(hightlightAll);
	QObject::connect((QCheckBox*)frSwitches[FRHIGHLIGHTALL],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(frSwitches[FRHIGHLIGHTALL]);
//rep all
	frSwitches[FRREPLACEALL]=new QCheckBox(gettext("Replace All"));
	reinterpret_cast<QCheckBox*>(frSwitches[FRREPLACEALL])->setChecked(replaceAll);
	QObject::connect((QCheckBox*)frSwitches[FRREPLACEALL],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(frSwitches[FRREPLACEALL]);

	vlayout->addWidget(hbox);

//switches 3rd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,4,0,4);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

	button=new QPushButton(gettext("Forward"));
	QObject::connect((QPushButton*)button,&QPushButton::clicked,doFindForwardWrap);
	icon=QIcon::fromTheme("go-next",QIcon("go-next"));
	button->setIcon(icon);
	hlayout->addWidget(button);

	button=new QPushButton(gettext("Back"));
	QObject::connect((QPushButton*)button,&QPushButton::clicked,doFindBackWrap);
	icon=QIcon::fromTheme("go-previous",QIcon("go-previous"));
	button->setIcon(icon);
	hlayout->addWidget(button);

	if(replaceAll==false)
		frReplace=new QPushButton(gettext("Replace"));
	else
		frReplace=new QPushButton(gettext("Replace All"));
	QObject::connect((QPushButton*)frReplace,&QPushButton::clicked,doReplaceWrap);
	icon=QIcon::fromTheme("edit-find-replace",QIcon("edit-find-replace"));
	reinterpret_cast<QPushButton*>(frReplace)->setIcon(icon);
	hlayout->addWidget(frReplace);

	vlayout->addWidget(hbox);

	findReplaceDialog->setLayout(vlayout);

#else
	GtkWidget*	content_area;
	GtkWidget*	replace;
	GtkWidget*	goback;
	GtkWidget*	image;
	GtkWidget*	label;
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	item;

	findReplaceDialog=gtk_dialog_new_with_buttons(gettext("Find/Replace"),(GtkWindow*)mainWindow, GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_GO_FORWARD,FINDNEXT,GTK_STOCK_GO_BACK,FINDPREV,gettext("Replace"),REPLACE,NULL);
	gtk_dialog_set_default_response((GtkDialog*)findReplaceDialog,GTK_RESPONSE_OK);
	g_signal_connect(G_OBJECT(findReplaceDialog),"response",G_CALLBACK(doFindReplace),NULL);
	content_area=gtk_dialog_get_content_area(GTK_DIALOG(findReplaceDialog));

	vbox=gtk_vbox_new(true,0);
	hbox=gtk_hbox_new(false,0);

	label=gtk_label_new(gettext("Find"));
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	findBox=gtk_entry_new();
	findDropBox=gtk_combo_box_text_new();
	g_signal_connect_after(G_OBJECT(findDropBox),"changed",G_CALLBACK(pasteFRClip),findBox);
	gtk_container_add(GTK_CONTAINER(content_area),findDropBox);
	gtk_widget_show(findDropBox);

	gtk_entry_set_text((GtkEntry*)findBox,"");
	gtk_entry_set_activates_default((GtkEntry*)findBox,true);
	gtk_container_add(GTK_CONTAINER(content_area),findBox);

	label=gtk_label_new(gettext("Replace With"));
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	replaceBox=gtk_entry_new();
	replaceDropBox=gtk_combo_box_text_new();
	g_signal_connect_after(G_OBJECT(replaceDropBox),"changed",G_CALLBACK(pasteFRClip),replaceBox);
	gtk_container_add(GTK_CONTAINER(content_area),replaceDropBox);
	gtk_widget_show(replaceDropBox);

	gtk_entry_set_text((GtkEntry*)replaceBox,"");
	gtk_entry_set_activates_default((GtkEntry*)replaceBox,true);
	gtk_container_add(GTK_CONTAINER(content_area),replaceBox);

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
	image=gtk_image_new_from_stock(GTK_STOCK_FIND_AND_REPLACE,GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image((GtkButton*)replace,image);

	goback=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,FINDPREV);
	gtk_widget_set_sensitive(goback,!useRegex);
	
	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
	if(replaceAll==true)
		gtk_button_set_label((GtkButton*)replace,gettext("Replace All"));

	gtk_widget_show(findBox);
	gtk_widget_show(replaceBox);
	gtk_widget_show(vbox);
	gtk_widget_show(hbox);

	gtk_signal_connect_object(GTK_OBJECT(findReplaceDialog),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(findReplaceDialog));
	g_signal_connect(G_OBJECT(findReplaceDialog),"delete_event",GTK_SIGNAL_FUNC(gtk_true),NULL);
#endif
}

#ifdef _ASPELL_
void buildWordCheck(int documentCheck)
{
#ifndef _USEQT5_
	GtkWidget*	vbox;
	GtkWidget*	button;
	GtkWidget*	hbox;
	GtkWidget*	label;
	GtkWidget*	image;
	char*		labeltext[512];
	int			docflag=documentCheck;

	spellCheckWord=gtk_dialog_new();
	gtk_window_set_title((GtkWindow*)spellCheckWord,gettext("Spell check word"));
	vbox=gtk_vbox_new(true,8);

	hbox=gtk_hbox_new(true,8);

	sprintf((char*)&labeltext,gettext("Change <i><b>%s</b></i> to: "),badWord);
	label=gtk_label_new((char*)&labeltext);
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(hbox),label,true,true,0);

	wordListDropbox=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(hbox),wordListDropbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	hbox=gtk_hbox_new(true,8);
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doChangeWord),(gpointer)(long)docflag);

	button=gtk_button_new_with_label(gettext("Ignore"));
	image=gtk_image_new_from_stock(GTK_STOCK_ADD,GTK_ICON_SIZE_MENU);
	gtk_button_set_image((GtkButton*)button,image);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)1);

	button=gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)2);

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doCancelCheck),NULL);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	GtkWidget* content=gtk_dialog_get_content_area((GtkDialog *)spellCheckWord);
	gtk_container_add(GTK_CONTAINER(content),(GtkWidget*)vbox);

	gtk_signal_connect_object(GTK_OBJECT(spellCheckWord),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(spellCheckWord));
	g_signal_connect(G_OBJECT(spellCheckWord),"delete_event",GTK_SIGNAL_FUNC(gtk_true),NULL);
#endif
}
#endif

int showFunctionEntry(void)
{
	gint		result=false;
#ifndef _USEQT5_
	GtkWidget*	dialog;
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
		debugFree(&functionSearchText,"showFunctionEntry functionSearchText");
	functionSearchText=strdup(gtk_entry_get_text((GtkEntry*)entrybox));
	gtk_widget_destroy(dialog);

#endif
	return(result);
}

#ifdef _BUILDDOCVIEWER_
#ifdef _USEQT5_
void buildDocViewer(void)
{
	QVBoxLayout	*docvlayout=new QVBoxLayout;
	QHBoxLayout	*dochlayout=new QHBoxLayout;
	QWidget		*widget;

	docView=new QMainWindow(mainWindow);
	docView->setGeometry(docWindowX,docWindowY,docWindowWidth,docWindowHeight);
	
	widget=new QWidget;
	widget->setLayout(docvlayout);
	qobject_cast<QMainWindow*>(docView)->setCentralWidget(widget);

	webView=new QWebView(widget);
    qobject_cast<QWebView*>(webView)->load(QUrl("file://" DATADIR "/help/help.en.html"));
    docvlayout->addWidget(webView);

	widget=new QPushButton(QIcon::fromTheme("go-previous",QIcon("go-previous")),gettext("Back"));
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoBack);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("go-home",QIcon("go-home")),gettext("Home"));
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoHome);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("edit-find",QIcon("edit-find")),gettext("Find"));
	dochlayout->addWidget(widget);

	widget=new QLineEdit;
	dochlayout->addWidget(widget);

	widget=new QPushButton(QIcon::fromTheme("go-down",QIcon("go-down")),gettext("Down"));
	dochlayout->addWidget(widget);
	widget=new QPushButton(QIcon::fromTheme("go-up",QIcon("go-up")),gettext("Up"));
	dochlayout->addWidget(widget);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("go-next",QIcon("go-next")),gettext("Forward"));
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoForward);

	widget=new QWidget;
	widget->setLayout(dochlayout);
	docvlayout->addWidget(widget);

	docView->hide();
}
#endif
#endif

#ifdef _BUILDDOCVIEWER_
void buildGtkDocViewer(void)
{
#ifndef _USEQT5_
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

	vbox=gtk_vbox_new(false,0);
	hbox=gtk_hbox_new(false,4);

	webView=webkit_web_view_new();
	g_signal_connect(G_OBJECT(webView),"navigation-policy-decision-requested",G_CALLBACK(docLinkTrap),NULL);	

	settings=webkit_web_view_get_settings((WebKitWebView*)webView);
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

	button=gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoBack),(void*)webView);	

//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	button=gtk_button_new_from_stock(GTK_STOCK_HOME);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoHome),(void*)webView);	
 
//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	entry=gtk_entry_new();
	findbutton=gtk_button_new_from_stock(GTK_STOCK_FIND);
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

	gtk_signal_connect_object(GTK_OBJECT(docView),"delete-event",GTK_SIGNAL_FUNC(toggleDocviewer),GTK_OBJECT(docView));
#endif
}
#endif




