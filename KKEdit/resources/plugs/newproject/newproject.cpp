/*
  plugs/newproject.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>

#include "kkedit-plugins.h"

GtkWidget*		menuProjects;

extern "C" const gchar* g_module_check_init(GModule *module)
{
	perror("doin ininit form newproject");
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	perror("doin cleanup form newproject");
	return(NULL);
}

void newProject(GtkWidget* widget,gpointer data)
{
}

extern "C" int addMenus(gpointer data)
{
	printf("adding  plug menus\n");
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	plugData*		plugdata=(plugData*)data;

	menuProjects=gtk_menu_item_new_with_label("_Projects");
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuProjects,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuProjects),menu);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_item_set_label((GtkMenuItem*)menuitem,"New Project");
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newProject),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuProjects);

	printf("done adding  plug menus\n");
	return(0);
}


