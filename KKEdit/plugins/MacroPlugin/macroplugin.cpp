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
#define TEXTDOMAIN "MacroPlugin-3" 
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "MacroPlugin" 
#define ABOUTICON "KKEditPlug"
#endif

struct	macroData
{
	unsigned	command;
	const void	*data;
	int			intData;
};

enum {STARTRECORDING=1,STOPRECORDING,PAUSERECORDING,PLAYBACK};
enum {DOOPEN=1};

macroData	recording[100];
unsigned	commandNum=0;
bool		paused=false;
unsigned	commandCnt=0;

GtkWidget	*menuStart;
GtkWidget	*menuStop;
GtkWidget	*menuPause;
GtkWidget	*menuPlayback;

GtkWidget	*menuPlug;
plugData	*plugdata;
char		*currentdomain=NULL;

int	(*module_plug_function)(gpointer globaldata);

void recordMacro(GtkMenuItem *menuitem,gpointer what)
{
	switch((long)what)
		{
			case STARTRECORDING:
				commandNum=0;
				commandCnt=0;
printf("--->STARTRECORDING=%i<--\n",(long)what);
				break;
			case STOPRECORDING:
				commandCnt=commandNum;
				commandNum=0;
printf("--->STOPRECORDING=%i<--\n",(long)what);
				break;
			case PAUSERECORDING:
				commandCnt=commandNum;
				paused=!paused;
printf("--->PAUSERECORDING=%i<--\n",(long)what);
				break;
			case PLAYBACK:
				for(unsigned j=0;j<commandCnt;j++)
					printf("--->PLAYBACK command num=%i data=%s<--\n",j,recording[j].data);
				break;
		}
}

extern "C" int informPlugin(plugData *plugdata)
{
	recording[commandNum++].data=plugdata->userDataStr;
//	recording[commandNum].command=DOOPEN
printf("---->%s<---\n",plugdata->userDataStr);
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

	plugdata=(plugData*)data;
	setTextDomain(true,plugdata);

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

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuPlug);					

	for(unsigned j=0;j<100;j++)
		{
			recording[j].command=0;
			recording[j].data=NULL;
			recording[j].intData=0;
		}

	setTextDomain(false,plugdata);
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
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));
	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

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
