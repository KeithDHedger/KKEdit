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

#define MYEMAIL "keithhedger@keithhedger.darktech.org"
#define MYWEBSITE "http://kkedit.darktech.org"
#define DIALOG_ABOUT_MORE_LABEL gettext("\nMore by the same author\n")

const char		*moreapps[]={
								"Big thanks to Sadi Yumuşak and Thanh for all their help.\n",
								"More by the same author",
								"Xfce-Theme-Manager\nhttp://khapplications.darktech.org/pages/apps.html#themeed\n",
								"Xfce4-Composite-Editor\nhttp://khapplications.darktech.org/pages/apps.html#xfcecomp\n",
								"Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n",
								"GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n",
								"ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n",
								"Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667"
							};

const char		*authors[]={"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,moreapps[0],moreapps[1],moreapps[2],moreapps[3],moreapps[4],moreapps[5],moreapps[6],moreapps[7],NULL};


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
