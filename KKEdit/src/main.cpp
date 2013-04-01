/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
*/
 
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "globals.h"
#include "files.h"
#include "callbacks.h"

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

void init(void)
{
	indent=true;
	lineNumbers=true;
	lineWrap=true;
	asprintf(&fontAndSize,"%s","mono 10");
}

int main(int argc,char **argv)
{
	GtkWidget*		vbox;
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GtkWidget*		toolbar;
//	GtkToolItem*	toolbutton;

	gtk_init(&argc,&argv);
	init();

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,800,600);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	notebook=(GtkNotebook*)gtk_notebook_new();
	gtk_signal_connect(GTK_OBJECT(notebook),"switch-page",G_CALLBACK(switchPage),NULL);

	vbox=gtk_vbox_new(FALSE,0);
	menubar=gtk_menu_bar_new();
	toolbar=gtk_toolbar_new();

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)menubar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)toolbar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)notebook,true,true,0);

//	toolbar
//new
	newButton=gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	gtk_toolbar_insert((GtkToolbar*)toolbar,newButton,-1);
//	gtk_signal_connect(GTK_OBJECT(toolbutton),"clicked",G_CALLBACK(doOpenFile),NULL);
//open
	openButton=gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert((GtkToolbar*)toolbar,openButton,-1);
	gtk_signal_connect(GTK_OBJECT(openButton),"clicked",G_CALLBACK(doOpenFile),NULL);
//save
	saveButton=gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_toolbar_insert((GtkToolbar*)toolbar,saveButton,-1);
	gtk_signal_connect(GTK_OBJECT(saveButton),"clicked",G_CALLBACK(saveFile),NULL);
//saveas
	saveasButton=gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
	gtk_toolbar_insert((GtkToolbar*)toolbar,saveasButton,-1);
//	gtk_signal_connect(GTK_OBJECT(toolbutton),"clicked",G_CALLBACK(saveFile),NULL);
//close
	closeButton=gtk_tool_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_toolbar_insert((GtkToolbar*)toolbar,closeButton,-1);
	gtk_signal_connect(GTK_OBJECT(closeButton),"clicked",G_CALLBACK(closeTab),NULL);

//file menu
	menufile=gtk_menu_item_new_with_label("File");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufile),menu);
//new
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//open
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doOpenFile),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//save
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(saveFile),NULL);
//savas
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//close
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(closeTab),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(shutdown),NULL);

//edit menu
	menuedit=gtk_menu_item_new_with_label("Edit");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuedit),menu);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_UNDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_REDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND_AND_REPLACE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//nav menu
	menunav=gtk_menu_item_new_with_label("Navigation");
//	menu=gtk_menu_new();
//	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menunav),menu);

//bookmarks
	menubookmark=gtk_menu_item_new_with_label("Bookmarks");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubookmark),menu);
	menuitem=gtk_menu_item_new_with_label("Add Bookmark");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menufile);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menuedit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menunav);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menubookmark);

	gtk_container_add(GTK_CONTAINER(window),(GtkWidget*)vbox);

	for (int j=1;j<argc;j++)
		openFile(argv[j]);

	gtk_widget_show_all(window);

	gtk_main();
}
