/*
  plugins/libplug_01.c
  plugin example with GLib: plugin
  Compile:
  gcc -fPIC -Wall -c libplug_01.c `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0` -o libplug_01.o
  gcc -shared -Wl,-soname,libplug_01.so -o libplug_01.so libplug_01.o -lc
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>

#include "kkedit-plugins.h"

void showDocView(int howtodisplay,char* text);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	perror("doin ininit");
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	perror("doin cleanup");
	return(NULL);
}

void openPlugHelp(GtkWidget* widget,gpointer data)
{
	plugData*	pdata=(plugData*)data;
	asprintf(pdata->thePage,"file://%s/plughelp.html",(char*)pdata->plugFolder);
	showDocView(USEURI,(char*)"KKEdit Plugin Help");
}

extern "C" int addMenus (gpointer menulist)
{
	printf("adding  plug menus\n");
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	plugData*		plugdata=(plugData*)menulist;

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuHelp);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,NULL);
	gtk_menu_item_set_label((GtkMenuItem*)menuitem,"Plugin Help");
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(openPlugHelp),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	printf("done adding  plug menus\n");
	return(0);
}





