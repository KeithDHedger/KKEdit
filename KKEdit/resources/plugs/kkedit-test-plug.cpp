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

struct plugMenuList
{
	GtkWidget*		menuBar;
	GtkWidget*		menuFile;
	GtkWidget*		menuEdit;
	GtkWidget*		menuFunc;
	GtkWidget*		menuNav;
	GtkWidget*		menuTools;
	GtkWidget*		menuHelp;
	GtkWidget*		menuBookMark;
	GtkWidget*		menuView;
};


//int module_variable = 42;
//
//G_MODULE_EXPORT extern gchar module_text[] = "This is module_text";
//
//G_MODULE_EXPORT extern gint module_test (gchar * szoveg)  {
//    GtkWidget * plugin_window = NULL;
//    g_message ("this is module text: %sn", szoveg);
//    g_message ("this is module variable: %dn", module_variable);
//    plugin_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//    gtk_window_set_title (GTK_WINDOW(plugin_window), "plugin_window");
//    gtk_widget_show (plugin_window);
//    return module_variable;
//}
extern "C" const gchar* g_module_check_init(GModule *module);
const gchar* g_module_check_init(GModule *module)
{
	printf("ZZZZZZZZZZZZ\n");
	perror("doin ininit");
	return(NULL);
}

const gchar* g_module_unload(GModule *module)
{
	printf("ZZZZZZZZZZZZ\n");
	perror("doin cleanup");
	return(NULL);
}

int addMenus (gpointer menulist)
{
	printf("ZZZZZZZZZZZZ\n");
	printf("adding  plug menus\n");
//	GtkWidget*		menuitem;
//	GtkWidget*		menu;
//	menu=gtk_menu_item_get_submenu((GtkMenuItem*)(plugMenuList*)menulist->menuHelp);
//	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,NULL);
////	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(openHelp),NULL);
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//
//	printf("adding  plug menusn");
	return(0);
}





