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

int module_variable = 42;

G_MODULE_EXPORT extern gchar module_text[] = "This is module_text";

G_MODULE_EXPORT extern gint module_test (gchar * szoveg)  {
    GtkWidget * plugin_window = NULL;
    g_message ("this is module text: %s\n", szoveg);
    g_message ("this is module variable: %d\n", module_variable);
    plugin_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(plugin_window), "plugin_window");
    gtk_widget_show (plugin_window);
    return module_variable;
}
