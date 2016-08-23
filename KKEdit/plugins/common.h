/*
 *
 * ©K. D. Hedger. Mon 28 Mar 16:34:26 BST 2016 kdhedger68713@gmail.com

 * This file (common.h) is part of KKEditPlugins.

 * Projects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * Projects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Projects.  If not, see <http://www.gnu.org/licenses/>.
 */

//#include "../src/links.h"

#include "../src/internet.h"
#ifndef _COMMON_
#define _COMMON_

#ifdef _USEGTK3_

#define GTK_STOCK_APPLY gettext("Apply")
#define GTK_STOCK_CANCEL gettext("Cancel")
#define GTK_STOCK_NEW gettext("New")
#define GTK_STOCK_HELP gettext("Help")
#define GTK_STOCK_OPEN gettext("Open")
#define GTK_STOCK_CONNECT gettext("Connect")
#define GTK_STOCK_OK gettext("OK")

#define GDK_C GDK_KEY_C
#define GDK_V GDK_KEY_V

#endif

#ifndef _ENUMS_
#define _ENUMS_
enum {NEWVBOX=0,NEWHBOX};
#endif

GtkWidget* createNewBox(int orient,bool homog,int spacing)
{
	GtkWidget	*retwidg=NULL;

#ifdef _USEGTK3_
	if(orient==NEWVBOX)
		retwidg=gtk_box_new(GTK_ORIENTATION_VERTICAL,spacing);
	else
		retwidg=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,spacing);
	gtk_box_set_homogeneous((GtkBox*)retwidg,homog);
#else
	if(orient==NEWVBOX)
		retwidg=gtk_vbox_new(homog,spacing);
	else
		retwidg=gtk_hbox_new(homog,spacing);
#endif

	return(retwidg);
}

GtkWidget* createNewStockMenuItem(const char* stock,const char* label)
{
	GtkWidget*	item;

#ifdef _USEGTK3_
	item=gtk_menu_item_new_with_mnemonic(label);
#else
	item=gtk_image_menu_item_new_from_stock(stock,NULL);
#endif

	return(item);
}

GtkWidget* createNewImageMenuItem(const char* stock,const char* label)
{
	GtkWidget	*item;

#ifdef _USEGTK3_
	item=gtk_menu_item_new_with_label(label);
#else
	GtkWidget	*image;
	item=gtk_image_menu_item_new_with_label(label);
	image=gtk_image_new_from_stock(stock,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)item,image);
#endif

	return(item);
}

#endif
