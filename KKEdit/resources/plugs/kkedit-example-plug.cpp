/*
  plugs/kkedit-example-plug.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>

#include "kkedit-plugins.h"

extern void showDocView(int howtodisplay,char* text,const char* title);

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
	showDocView(USEURI,(char*)"KKEdit Plugin Help",(char*)"KKEdit Plugin Help");
}

extern "C" int addMenus(gpointer data)
{
	printf("adding  plug menus\n");
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	plugData*		plugdata=(plugData*)data;

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuHelp);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,NULL);
	gtk_menu_item_set_label((GtkMenuItem*)menuitem,"Plugin Help");
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(openPlugHelp),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	printf("done adding  plug menus\n");
	return(0);
}

extern "C" int setSensitive(gpointer data)
{
	printf("set sensitive\n");
	return(0);
}

extern "C" int openFile(gpointer data)
{
	printf("open file \n");
	return(0);
}

extern "C" int saveFile(gpointer data)
{
	printf("save file\n");
	return(0);
}

extern "C" int newFile(gpointer data)
{
	printf("new file\n");
	return(0);
}

extern "C" int closeFile(gpointer data)
{
	printf("close file\n");
	return(0);
}

extern "C" int newTab(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	printf("doing new tab\n");

	if(plugdata->page->filePath!=NULL)
		printf("filePath = %s\n",plugdata->page->filePath);

	if(plugdata->page->fileName!=NULL)
		printf("fileName = %s\n",plugdata->page->fileName);

	if(plugdata->page->dirName!=NULL)
		printf("dirName = %s\n",plugdata->page->dirName);
		
	printf("finished new tab\n");
	return(0);
}

