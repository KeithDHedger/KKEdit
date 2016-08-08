/*
 *
 * ©K. D. Hedger. Wed 20 Apr 17:13:14 BST 2016 kdhedger68713@gmail.com

 * This file (macroplugin.cpp) is part of KKEdit.

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

#define PLUGVERSION VERSION
#ifdef _USEGTK3_
#define TEXTDOMAIN "macroplugin-3" 
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "macroplugin" 
#define ABOUTICON "KKEditPlug"
#endif

typedef plugUserData macroData;

enum {STARTRECORDING=1,STOPRECORDING,PAUSERECORDING,PLAYBACK};
enum {DOOPEN=1};

macroData	recording[100];
unsigned	commandNum=0;
bool		paused=false;
unsigned	commandCnt=0;
bool		isRecording=false;

GtkWidget	*menuStart;
GtkWidget	*menuStop;
GtkWidget	*menuPause;
GtkWidget	*menuPlayback;
GtkWidget	*holdWidget=NULL;

GtkWidget	*menuPlug;
plugData	*globalPlugData;
char		*currentdomain=NULL;
int			sinkInt;

int	(*module_plug_function)(gpointer globaldata);
extern void gotoLine(GtkWidget *widget,gpointer data);
extern void basicFind(int dowhat);

extern GtkWidget	*findBox;
extern GtkWidget	*replaceBox;

GtkWidget* findMenu(GtkWidget* parent, const gchar* name)
{
	const gchar	*mname=NULL;

	if ( (GTK_IS_MENU_ITEM(parent)) && !(GTK_IS_SEPARATOR_MENU_ITEM(parent)) )
		{
			mname=gtk_widget_get_name((GtkWidget*)parent);
			if(mname!=NULL)
				{
			//printf("name=%s,mname=%s\n",name,mname);
					if(strcmp(name,mname)==0)
						holdWidget=parent;
				}
		}

	if (GTK_IS_CONTAINER(parent))
		{
			GList	*children=gtk_container_get_children(GTK_CONTAINER(parent));
			while(children != NULL)
				{
					GtkWidget	*widget=findMenu((GtkWidget*)children->data, name);

					if (widget!=NULL)
						return widget;
					children=g_list_next(children);
				}
			g_list_free(children);
		}
	return NULL;
}

void recordMacro(GtkMenuItem *menuitem,gpointer what)
{
	switch((long)what)
		{
			case STARTRECORDING:
				commandNum=0;
				commandCnt=0;
				isRecording=true;
printf("--->STARTRECORDING<--\n");
				break;
			case STOPRECORDING:
				commandCnt=commandNum;
				commandNum=0;
				isRecording=false;
printf("--->STOPRECORDING=<--\n");
				break;
			case PAUSERECORDING:
				commandCnt=commandNum;
				paused=!paused;
printf("--->PAUSERECORDING<--\n");
				break;
			case PLAYBACK:
				printf("Playing back %i commands ..\n",commandCnt);
				for(unsigned j=0;j<commandCnt;j++)
					{
						holdWidget=NULL;
						switch(recording[j].type)
							{
								case FROMFILEMENU:
									findMenu(gtk_menu_item_get_submenu((GtkMenuItem*)globalPlugData->mlist.menuFile),recording[j].userStr);
									if(holdWidget!=NULL)
										g_signal_emit_by_name((gpointer)holdWidget,"activate",recording[j].userData,NULL);
									break;

								case FROMEDITMENU:
									findMenu(gtk_menu_item_get_submenu((GtkMenuItem*)globalPlugData->mlist.menuEdit),recording[j].userStr);
									if(holdWidget!=NULL)
										g_signal_emit_by_name((gpointer)holdWidget,"activate",NULL,NULL);
									break;

								case FROMSELECTION:
									if((recording[j].userStart!=-1) && (recording[j].userEnd!=-1))
										{
											pageStruct	*page=getPageStructByIDFromPage(-1);
											if(page!=NULL)
												{
													GtkTextIter sc,ec;
													gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&sc);
													gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&ec);
													gtk_text_iter_set_offset(&sc,recording[j].userStart);
													gtk_text_iter_set_offset(&ec,recording[j].userEnd);
													gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&sc,&ec);
												}
										}
									break;

								case FROMGOTO:
									gotoLine(NULL,(gpointer)recording[j].userData);
									break;

								case FROMBASICFIND:
								printf("user str1=%s,userstr2=%s dowaht=%i\n",recording[j].userStr,recording[j].userStr2,recording[j].userInt);
									gtk_entry_set_text((GtkEntry*)findBox,recording[j].userStr);
									gtk_entry_set_text((GtkEntry*)replaceBox,recording[j].userStr2);
								//	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)findBox));
//	replacetext=g_strcompress(gtk_entry_get_text((GtkEntry*)replaceBox));
									basicFind(recording[j].userInt);
									break;
							}
					}
				printf("Finished.\n");
				break;
		}
}

extern "C" int informPlugin(plugData *plugdata)
{
	if(isRecording && !paused)
		recording[commandNum++]=plugdata->userData;
	return(0);
}
             
extern "C" const gchar* g_module_check_init(GModule *module)
{
	currentdomain=strdup(textdomain(NULL));
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

extern "C" void g_module_unload(GModule *module)
{
	return;
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menu;

	globalPlugData=(plugData*)data;
	setTextDomain(true,globalPlugData);

	menuPlug=gtk_menu_item_new_with_label(gettext("_Macro"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuPlug,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuPlug),menu);

	menuStart=gtk_menu_item_new_with_label("Start Recording");
	g_signal_connect(G_OBJECT(menuStart),"activate",G_CALLBACK(recordMacro),(void*)(long)STARTRECORDING);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuStart);
	menuStop=gtk_menu_item_new_with_label("Stop Recording");
	g_signal_connect(G_OBJECT(menuStop),"activate",G_CALLBACK(recordMacro),(void*)(long)STOPRECORDING);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuStop);
	menuPause=gtk_menu_item_new_with_label("Pause Recording");
	g_signal_connect(G_OBJECT(menuPause),"activate",G_CALLBACK(recordMacro),(void*)(long)PAUSERECORDING);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuPause);
	menuPlayback=gtk_menu_item_new_with_label("Playback");
	g_signal_connect(G_OBJECT(menuPlayback),"activate",G_CALLBACK(recordMacro),(void*)(long)PLAYBACK);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuPlayback);

	gtk_menu_shell_append(GTK_MENU_SHELL(globalPlugData->mlist.menuBar),menuPlug);					

	setTextDomain(false,globalPlugData);
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData		*plugdata=(plugData*)data;
	char			*licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	char			*licence;
	GtkAboutDialog	*about;
	char			*translators;

	setTextDomain(true,plugdata);

	const char		*aboutboxstring=gettext("Macro Plugin - Add's macro recording/playback");
	sinkInt=asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));
	sinkInt=asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"MacroPlugin");
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,copyright);
	gtk_about_dialog_set_version(about,PLUGVERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
	gtk_about_dialog_set_logo_icon_name(about,ABOUTICON);
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
