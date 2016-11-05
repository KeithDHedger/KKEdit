/*
 *
 * ©K. D. Hedger. Tue  6 Oct 12:24:38 BST 2015 kdhedger68713@gmail.com

 * This file (sessionmanager.cpp) is part of KKEdit.

 * KKEditPlugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEditPlugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEditPlugins.  If not, see <http://www.gnu.org/licenses/>.
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

#define PLUGVERSION VERSION
#define	MAXSESSIONS 16
#ifdef _USEGTK3_
#define TEXTDOMAIN "SessionManager-3"
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "SessionManager" 
#define ABOUTICON "KKEditPlug"
#endif

char		*prefsPath;
GtkWidget*	menuPlug;
GtkWidget*	saveSessionMenu=NULL;
GtkWidget*	restoreSessionMenu=NULL;
GtkWidget*	holdWidget=NULL;
GtkWidget*	menusm;
char		*currentdomain=NULL;
int			sinkReturn;
char		*sinkReturnStr;

int	(*module_plug_function)(gpointer globaldata);

extern void saveSession(const char* name,const char* filepath);
extern void restoreSession(GtkWidget* widget,gpointer data);
extern void closeAllTabs(GtkWidget* widget,gpointer data);
extern void toggleBookmark(GtkWidget* widget,GtkTextIter* titer);

extern GList*			newBookMarksList;

struct bookMarksNew
{
	pageStruct*			page;
	char				*label;
	GtkSourceMark*		mark;
	char				*markName;
	int					line;
};

char* sessionNames[MAXSESSIONS]= {NULL,};

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

GtkWidget* findMenu(GtkWidget* parent, const gchar* name)
{
	const gchar	*mname=NULL;

	if ( (GTK_IS_MENU_ITEM(parent)) && !(GTK_IS_SEPARATOR_MENU_ITEM(parent)) )
		{
			mname=gtk_widget_get_name((GtkWidget*)parent);
			if(mname!=NULL)
				{
					if(strcmp(name,mname)==0)
						{
							holdWidget=parent;
						}
				}
		}

	if (GTK_IS_CONTAINER(parent))
		{
			GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
			while ((children = g_list_next(children)) != NULL)
				{
					GtkWidget* widget = findMenu((GtkWidget*)children->data, name);

					if (widget != NULL)
						{
							return widget;
						}
				}
			g_list_free(children);
		}
	return NULL;
}

extern "C" const gchar* g_module_check_init(GModule *module)
{
	return(NULL);
}

extern "C" void g_module_unload(GModule *module)
{
	for(int j=0; j<MAXSESSIONS; j++)
		free(sessionNames[j]);
	return;
}

char* getNewSessionName(int sessionnumber,plugData* plugdata)
{
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	prefs;
	GtkWidget*	vbox;
	int			response;
	char		*command;

	vbox=createNewBox(NEWVBOX,false,0);

	setTextDomain(true,plugdata);
	dialog=gtk_dialog_new_with_buttons(gettext("Session Manager"),(GtkWindow*)plugdata->mainWindow,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	prefs=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)prefs,sessionNames[sessionnumber]);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Session Name")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,prefs,true,true,4);

	gtk_widget_show_all(vbox);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		sinkReturn=asprintf(&command,"%s",gtk_entry_get_text((GtkEntry*)prefs));
	else
		command=NULL;

	gtk_widget_destroy((GtkWidget*)dialog);
	setTextDomain(false,plugdata);
	return(command);
}

void saveSessionPlug(char* name,plugData* plugdata,int snum)
{
	char			*filename;

	sinkReturn=asprintf(&filename,"%s/session-%i",plugdata->lPlugFolder,snum);
	saveSession(name,(const char*)filename);
	debugFree(&filename);
}

void restoreSessionNum(GtkWidget* widget,gpointer data)
{
	char		*sessionfile;
	const char	*widgetname=NULL;
	plugData	*plugdata=(plugData*)data;

	widgetname=gtk_widget_get_name(widget);
	sinkReturn=asprintf(&sessionfile,"%s/session-%i",plugdata->lPlugFolder,atoi(widgetname));
	restoreSession(NULL,sessionfile);
	free(sessionfile);
}

void rebuildMainMenu(bool dosavemenu,GtkWidget* menu,plugData*	plugdata,GCallback* func)
{
	GtkWidget*	menuitem;
	char		*sessionname;
	GtkWidget*	submenu;

	submenu=gtk_menu_item_get_submenu(GTK_MENU_ITEM(menu));
	gtk_widget_destroy(submenu);
	submenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu),submenu);

	for(int j=0; j<MAXSESSIONS; j++)
		{
			menuitem=gtk_menu_item_new_with_label(sessionNames[j]);
			
			if(dosavemenu==true)
				sinkReturn=asprintf(&sessionname,"%i-SaveSession",j);
			else
				sinkReturn=asprintf(&sessionname,"%i-RestoreSession",j);
			gtk_widget_set_name(menuitem,sessionname);
			free(sessionname);
			
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(func),plugdata);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuitem);
		}
	gtk_widget_show_all(menu);
}

void saveSessionNum(GtkWidget* widget,gpointer data)
{
	char		*sname=NULL;
	int			snum=0;
	plugData*	plugdata=(plugData*)data;

	snum=atoi(gtk_widget_get_name(widget));

	sname=getNewSessionName(snum,plugdata);
	if(sname!=NULL)
		{
			free(sessionNames[snum]);
			sessionNames[snum]=sname;
			saveSessionPlug(sname,plugdata,snum);
			rebuildMainMenu(true,saveSessionMenu,plugdata,(GCallback*)saveSessionNum);
			rebuildMainMenu(false,restoreSessionMenu,plugdata,(GCallback*)restoreSessionNum);
		}
}

extern "C" int addToGui(gpointer data)
{

	GtkWidget*	menuitem;
	char		*sessionname;
	GtkWidget*	menu;

	plugData*	plugdata=(plugData*)data;

	char	*sessionfile;
	FILE*	fd=NULL;

	setTextDomain(true,plugdata);
	for(int j=0; j<MAXSESSIONS; j++)
		{
			sinkReturn=asprintf(&sessionfile,"%s/session-%i",plugdata->lPlugFolder,j);
			fd=fopen(sessionfile,"r");
			if(fd!=NULL)
				{
					sinkReturn=fscanf(fd,"%a[^\n]s",&sessionNames[j]);
					fclose(fd);
				}
			else
				sinkReturn=asprintf(&sessionNames[j],gettext("Session %i"),j);
		}

	setTextDomain(false,plugdata);

	holdWidget=NULL;

	findMenu(gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuFile),SAVESESSIONMENUNAME);
	if(holdWidget!=NULL)
		{
			saveSessionMenu=holdWidget;
			g_signal_handlers_disconnect_by_func(saveSessionMenu,(void*)saveSession,NULL);
			menu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(saveSessionMenu),menu);

			for(int j=0; j<MAXSESSIONS; j++)
				{
					menuitem=gtk_menu_item_new_with_label(sessionNames[j]);
					sinkReturn=asprintf(&sessionname,"%i-SaveSession",j);
					gtk_widget_set_name(menuitem,sessionname);
					free(sessionname);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(saveSessionNum),plugdata);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
				}
			gtk_widget_show_all(saveSessionMenu);
		}

	holdWidget=NULL;

	findMenu(gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuFile),RESTORESESSIONMENUNAME);
	if(holdWidget!=NULL)
		{
			restoreSessionMenu=holdWidget;
			g_signal_handlers_disconnect_by_func(restoreSessionMenu,(void*)restoreSession,NULL);
			menu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(restoreSessionMenu),menu);

			for(int j=0; j<MAXSESSIONS; j++)
				{
					menuitem=gtk_menu_item_new_with_label(sessionNames[j]);
					sinkReturn=asprintf(&sessionname,"%i-RestoreSession",j);
					gtk_widget_set_name(menuitem,sessionname);
					free(sessionname);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(restoreSessionNum),plugdata);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
				}
			gtk_widget_show_all(restoreSessionMenu);
		}

	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData		*plugdata=(plugData*)data;
	char			*licencepath;
	char			*licence;
	GtkAboutDialog*	about;
	char			*translators;

	setTextDomain(true,plugdata);

	const char		*aboutboxstring=gettext("Session Manager - Adds multiple named sessions to KKEdit");
	sinkReturn=asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	sinkReturn=asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Session Manager"));
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,COPYRITE);
	gtk_about_dialog_set_version(about,PLUGVERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
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
			return(1);
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	return(0);
}

