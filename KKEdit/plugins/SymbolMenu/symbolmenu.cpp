/*
 *
 * ©K. D. Hedger. Sat  5 Nov 15:42:06 GMT 2016 kdhedger68713@gmail.com

 * This file (symbolmenuplugin.cpp) is part of plugins.

 * plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with plugins.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <libgen.h>
#include <string.h>
#include <ctype.h>

#include <libintl.h>
#include <locale.h>

#include "../config.h"
#include "../common.h"
#include "../kkedit-plugins.h"
#include "../../src/sliceclass.h"

#define PLUGVERSION VERSION
#ifdef _USEGTK3_
#define TEXTDOMAIN "SymbolMenuPlugin-3"
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "SymbolMenuPlugin" 
#define ABOUTICON "KKEditPlug"
#endif

extern bool			showStatus;
extern GtkWidget	*statusWidget;

char*				prefsPath;
GtkWidget			*menuPlug;
char				*sinkReturnStr;
int					sinkReturn;
GtkClipboard		*mainClipboard;
char				*currentdomain=NULL;

int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	return(NULL);
}

void setTextDomain(bool plugdomain,plugData* pdata)
{
	if(plugdomain==true)
		{
			//set domain to plug
			bindtextdomain(TEXTDOMAIN,LOCALEDIR);
			textdomain(TEXTDOMAIN);
			bind_textdomain_codeset(TEXTDOMAIN,"UTF-8");
		}
	else
		{
			//resetdomain
			bindtextdomain(currentdomain,pdata->locale);
			textdomain(currentdomain);
			bind_textdomain_codeset(currentdomain,"UTF-8");	
		}
}

void copySymbol(GtkWidget* widget,gpointer data)
{
	char	*message=NULL;
	gtk_clipboard_set_text(mainClipboard,gtk_menu_item_get_label((GtkMenuItem*)widget),-1);

	if((showStatus==false))
		return;

	gtk_statusbar_remove_all((GtkStatusbar*)statusWidget,0);
	sinkReturn=asprintf(&message,"Copied %s to clipboard ...",gtk_menu_item_get_label((GtkMenuItem*)widget));
	gtk_statusbar_push((GtkStatusbar*)statusWidget,0,message);
	free(message);
	gtk_clipboard_set_text(mainClipboard,gtk_menu_item_get_label((GtkMenuItem*)widget),-1);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menuitem;
	GtkWidget*	submenuitem;
	GtkWidget*	submenu;
	GtkWidget*	subsubmenu;
	FILE*		fd=NULL;
	char		buffer[2048];
	StringSlice *mystr=new StringSlice;
	plugData	*plugdata=(plugData*)data;
	char		*menulabel=NULL;
	gchar		*symbol=NULL;
	char		*filepath;

	menuPlug=gtk_menu_item_new_with_label("_Symbols");
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuPlug,true);
	submenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuPlug),submenu);

	mystr->setReturnDupString(false);
	mainClipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	sinkReturn=asprintf(&filepath,"%s/indicator-chars.txt",plugdata->lPlugFolder);
	fd=fopen(filepath,"r");
	if(fd==NULL)
		{
			free(filepath);
			sinkReturn=asprintf(&filepath,"%s/indicator-chars.txt",plugdata->gPlugFolder);
			fd=fopen(filepath,"r");
		}

	if(fd==NULL)
		{
			free(filepath);
			sinkReturn=asprintf(&filepath,"%s/.indicator-chars.txt",getenv("HOME"));
			fd=fopen(filepath,"r");
		}

	if(fd!=NULL)
		{
			free(filepath);
			while(feof(fd)==0)
				{
					buffer[0]=0;
					sinkReturnStr=fgets(buffer,2048,fd);
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							menulabel=mystr->sliceBetween(buffer,(char*)"[",(char*)"]");
							if(strlen(menulabel)>0)
								{
									menuitem=gtk_menu_item_new_with_label(menulabel);
									gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuitem);
									menulabel=mystr->sliceBetween(buffer,(char*)"]",NULL);
									if(g_utf8_strlen(menulabel,-1)>0)
										{
											subsubmenu=gtk_menu_new();
											gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),subsubmenu);
											for(int j=0;j<g_utf8_strlen(menulabel,-1);j++)
												{
													symbol=g_utf8_substring (menulabel,j,j+1);
													submenuitem=gtk_menu_item_new_with_label(symbol);
													gtk_menu_shell_append(GTK_MENU_SHELL(subsubmenu),submenuitem);
													g_signal_connect(G_OBJECT(submenuitem),"activate",G_CALLBACK(copySymbol),NULL);
													g_free(symbol);
												}
										}
								}
						}
				}
		}

	delete mystr;

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuPlug);					
	gtk_widget_show_all(menuPlug);
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData		*plugdata=(plugData*)data;
	char			*licencepath;
	char			*licence;
	GtkAboutDialog	*about;
	char			*translators;

	setTextDomain(true,plugdata);

	const char		*aboutboxstring=gettext("Symbol Menu Plugin - Copy symbols to clipboard");
	sinkReturn=asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));
	sinkReturn=asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Symbol Menu Plugin"));
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,COPYRITE);
	gtk_about_dialog_set_version(about,PLUGVERSION);
	gtk_about_dialog_set_website(about,KKEDITPAGE);
	gtk_about_dialog_set_website_label(about,"KKEdit Homepage");
	gtk_about_dialog_set_logo_icon_name(about,ABOUTICON);
	gtk_about_dialog_set_license(about,licence);
	gtk_about_dialog_set_translator_credits(about,(const gchar*)translators);
	gtk_window_set_transient_for((GtkWindow*)about,(GtkWindow*)plugdata->prefsWindow);

	gtk_dialog_run(GTK_DIALOG(about));
	gtk_widget_destroy((GtkWidget*)about);
	free(licence);
	free(licencepath);
	free(translators);
	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(menuPlug);
			gtk_widget_show_all(plugdata->mlist.menuBar);	
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	return(0);
}
