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

#include "../common.h"
#include "../kkedit-plugins.h"

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://kkedit.darktech.org"
#define PLUGVERSION "0.3.0"
#define	MAXSESSIONS 16
#define TEXTDOMAIN "SessionManager"

extern void restoreSession(GtkWidget* widget,gpointer data);

char*		prefsPath;
GtkWidget*	menuPlug;
GtkWidget*	saveSessionMenu=NULL;
GtkWidget*	restoreSessionMenu=NULL;
GtkWidget*	holdWidget=NULL;
GtkWidget*	menusm;
char*		currentdomain=NULL;

int	(*module_plug_function)(gpointer globaldata);

extern void saveSession(GtkWidget* widget,gpointer data);
extern void restoreSession(GtkWidget* widget,gpointer data);
extern void closeAllTabs(GtkWidget* widget,gpointer data);
extern void toggleBookmark(GtkWidget* widget,GtkTextIter* titer);

extern GList*			newBookMarksList;

struct bookMarksNew
{
	pageStruct*			page;
	char*				label;
	GtkSourceMark*		mark;
	char*				markName;
	int					line;
};

char*		sessionNames[MAXSESSIONS]= {NULL,};

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
	const gchar* mname=NULL;

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
	char*		command;

	vbox=createNewBox(NEWVBOX,false,0);

	setTextDomain(true,plugdata);
	dialog=gtk_dialog_new_with_buttons(gettext("Session Manager"),NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	prefs=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)prefs,sessionNames[sessionnumber]);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Session Name")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,prefs,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		asprintf(&command,"%s",gtk_entry_get_text((GtkEntry*)prefs));
	else
		command=NULL;

	gtk_widget_destroy((GtkWidget*)dialog);
	setTextDomain(false,plugdata);
	return(command);
}

void saveSessionPlug(char* name,plugData* plugdata,int snum)
{
	pageStruct*		page;
	FILE*			fd=NULL;
	char*			filename;
	GtkTextMark*	mark;
	GtkTextIter		iter;
	int				linenumber;
	GtkTextIter		markiter;
	GList*			ptr;

	asprintf(&filename,"%s/session-%i",plugdata->lPlugFolder,snum);
	fd=fopen(filename,"w");
	if (fd!=NULL)
		{
			fprintf(fd,"%s\n",name);
			for(int loop=0; loop<gtk_notebook_get_n_pages(plugdata->notebook); loop++)
				{
					page=getPageStructPtr(loop);
					mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
					linenumber=gtk_text_iter_get_line(&iter);
					fprintf(fd,"%i %s\n",linenumber,page->filePath);

					ptr=newBookMarksList;
					while(ptr!=NULL)
						{
							if(((bookMarksNew*)ptr->data)->page==page)
								{
									gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&markiter,(GtkTextMark*)((bookMarksNew*)ptr->data)->mark);
									fprintf(fd,"%i ",gtk_text_iter_get_line(&markiter));
									fprintf(fd,"%s\n",((bookMarksNew*)ptr->data)->label);
								}
							ptr=g_list_next(ptr);
						}
					fprintf(fd,"-1 endmarks\n");
				}

			fclose(fd);
			debugFree(&filename);
		}
}

void restoreSessionNum(GtkWidget* widget,gpointer data)
{
	char*		sessionfile;
	char*		sname=NULL;
	FILE*		fd=NULL;
	const char	*widgetname=NULL;
	plugData	*plugdata=(plugData*)data;

	widgetname=gtk_widget_get_name(widget);
	for(int j=0; j<MAXSESSIONS; j++)
		{
			asprintf(&sessionfile,"%s/session-%i",plugdata->lPlugFolder,j);
			fd=fopen(sessionfile,"r");
			if(fd!=NULL)
				{
					fscanf(fd,"%a[^\n]s",&sname);
					if(strcmp(sname,widgetname)==0)
						{
							restoreSession(NULL,sessionfile);
							free(sessionfile);
							free(sname);
							fclose(fd);
							return;
						}
					free(sname);
					fclose(fd);
				}
		}
}

void rebuildMainMenu(GtkWidget* menu,plugData*	plugdata,GCallback* func)
{
	GtkWidget*	menuitem;
	char*		sessionname;
	GtkWidget*	submenu;

	submenu=gtk_menu_item_get_submenu(GTK_MENU_ITEM(menu));
	gtk_widget_destroy(submenu);
	submenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu),submenu);

	for(int j=0; j<MAXSESSIONS; j++)
		{
			asprintf(&sessionname,"%s",sessionNames[j]);
			menuitem=gtk_menu_item_new_with_label(sessionname);
			gtk_widget_set_name(menuitem,sessionNames[j]);
			free(sessionname);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(func),plugdata);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuitem);
		}
	gtk_widget_show_all(menu);
}

void saveSessionNum(GtkWidget* widget,gpointer data)
{
	char*		sname=NULL;
	int			snum=0;
	plugData*	plugdata=(plugData*)data;

	for(int j=0; j<MAXSESSIONS; j++)
		{
			if(strcmp(sessionNames[j],gtk_widget_get_name(widget))==0)
				snum=j;
		}

	sname=getNewSessionName(snum,plugdata);
	if(sname!=NULL)
		{
			free(sessionNames[snum]);
			sessionNames[snum]=sname;
			saveSessionPlug(sname,plugdata,snum);
			rebuildMainMenu(saveSessionMenu,plugdata,(GCallback*)saveSessionNum);
			rebuildMainMenu(restoreSessionMenu,plugdata,(GCallback*)restoreSessionNum);
		}
}

extern "C" int addToGui(gpointer data)
{

	GtkWidget*	menuitem;
	char*		sessionname;
	GtkWidget*	menu;

	plugData*	plugdata=(plugData*)data;

	char*	sessionfile;
	FILE*	fd=NULL;

	setTextDomain(true,plugdata);
	for(int j=0; j<MAXSESSIONS; j++)
		{
			asprintf(&sessionfile,"%s/session-%i",plugdata->lPlugFolder,j);
			fd=fopen(sessionfile,"r");
			if(fd!=NULL)
				{
					fscanf(fd,"%a[^\n]s",&sessionNames[j]);
					fclose(fd);
				}
			else
				asprintf(&sessionNames[j],gettext("Session %i"),j);
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
					asprintf(&sessionname,"%s",sessionNames[j]);
					menuitem=gtk_menu_item_new_with_label(sessionname);
					gtk_widget_set_name(menuitem,sessionNames[j]);
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
					asprintf(&sessionname,"%s",sessionNames[j]);
					menuitem=gtk_menu_item_new_with_label(sessionname);
					gtk_widget_set_name(menuitem,sessionNames[j]);
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
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	char*			licence;
	GtkAboutDialog*	about;
	char*			translators;

	setTextDomain(true,plugdata);

	const char*		aboutboxstring=gettext("Session Manager - Adds multiple named sessions to KKEdit");
	const char*	authors[]= {"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,gettext("\nMore by the same author\n"),"Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Session Manager"));
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,copyright);
	gtk_about_dialog_set_version(about,PLUGVERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
	gtk_about_dialog_set_logo_icon_name(about,"KKEditPlug");
	gtk_about_dialog_set_license(about,licence);
	gtk_about_dialog_set_translator_credits(about,(const gchar*)translators);

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
