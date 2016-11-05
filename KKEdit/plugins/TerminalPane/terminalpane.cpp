/*
 *
 * ©K. D. Hedger. Tue  6 Oct 12:24:48 BST 2015 kdhedger68713@gmail.com

 * This file (terminalpane.cpp) is part of KKEdit.

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>

#include <libintl.h>
#include <locale.h>

#include "../config.h"
#include "../common.h"
#include "../kkedit-plugins.h"

#define PLUGVERSION VERSION
#ifdef _USEGTK3_
#define TEXTDOMAIN "TerminalPane-3" 
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "TerminalPane" 
#define ABOUTICON "KKEditPlug"
#endif

int	(*module_plug_function)(gpointer globaldata);

GtkWidget*	terminal=NULL;
GtkWidget*	hideMenu=NULL;
bool		showing=false;
GtkWidget*	swindow;

char*		foreColour=strdup("#000000");
char*		backColour=strdup("#ffffff");
int			childPid=-999;
GtkWidget*	contextMenu;
char		*currentdomain=NULL;
int			sinkInt;

args		mydata[]=
				{
					{"forecol",TYPESTRING,&foreColour},
					{"backcol",TYPESTRING,&backColour},
					{NULL,0,NULL}
				};

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

void touch(char* path)
{
	int	fd;

	fd=open(path,O_WRONLY|O_CREAT,DEFFILEMODE);
	if(fd!=-1)
		close(fd);
}

extern "C" const gchar* g_module_check_init(GModule *module)
{
	currentdomain=strdup(textdomain(NULL));
	return(NULL);
}

extern "C" void g_module_unload(GModule *module)
{
	return;
}

void doStartUpCheck(plugData* pdata)
{
	char*	filepath;

	sinkInt=asprintf(&filepath,"%s/terminal.rc",pdata->lPlugFolder);
	if(g_file_test(filepath,G_FILE_TEST_EXISTS))
		showing=true;
	else
		showing=false;

	debugFree(&filepath);
}

void showHideTerminal(plugData* plugdata,bool startup)
{
	char*	filepath;

	setTextDomain(true,plugdata);
	sinkInt=asprintf(&filepath,"%s/terminal.rc",plugdata->lPlugFolder);
	if(showing==true)
		{
			gtk_widget_show_all(swindow);
			if(plugdata->bottomShow==0 && startup==false)
				showTop(false);
			touch(filepath);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Hide Terminal"));
		}
	else
		{
			gtk_widget_show_all(swindow);
			unlink(filepath);
			if(plugdata->bottomShow==1 && startup==false)
				hideTop(false);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Show Terminal"));
		}
	setTextDomain(false,plugdata);
	debugFree(&filepath);
}

void toggleTerminal(GtkWidget* widget,gpointer data)
{
	showing=!showing;
	showHideTerminal((plugData*)data,false);
}

void cdHere(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;

	if(plugdata->page->dirName!=NULL)
		{
			vte_terminal_feed_child((VteTerminal*)terminal,"cd ",-1);
			vte_terminal_feed_child((VteTerminal*)terminal,plugdata->page->dirName,-1);
			vte_terminal_feed_child((VteTerminal*)terminal,"\n",-1);
		}
}

gboolean doButton(GtkWidget *widget, GdkEventButton *event,gpointer data)
{
	int button, event_time;

	gtk_widget_set_can_focus(terminal,true);
	gtk_widget_grab_focus(terminal);

///* Ignore double-clicks and triple-clicks */
  if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
    {
		gtk_widget_show_all(contextMenu);
		if (event)
			{
				button=event->button;
				event_time=event->time;
			}
		else
			{
				button=0;
				event_time=gtk_get_current_event_time();
				}

		gtk_menu_popup(GTK_MENU(contextMenu),NULL,NULL,NULL,NULL,button,event_time);
	}
	return(false);
}

gboolean on_key_press (GtkWidget *terminal, GdkEventKey *event)
{
	if (event->state == (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
		{
			switch (event->keyval)
				{
				case GDK_C:
					vte_terminal_copy_clipboard (VTE_TERMINAL (terminal));
					return true;
				case GDK_V:
					vte_terminal_paste_clipboard (VTE_TERMINAL (terminal));
					return true;
				}
		}
	return false;
}

void copyFromTerm(GtkWidget* widget,gpointer data)
{
	vte_terminal_copy_clipboard((VteTerminal*)terminal);
}

void pasteToTerm(GtkWidget* widget,gpointer data)
{
	vte_terminal_paste_clipboard((VteTerminal*)terminal);
}

void selectAllInTerm(GtkWidget* widget,gpointer data)
{
	vte_terminal_select_all((VteTerminal*)terminal);
}

void makeMenu(gpointer plugdata)
{
	GtkWidget *popmenuitem;

	contextMenu=gtk_menu_new ();

	setTextDomain(true,(plugData*)plugdata);
	popmenuitem=gtk_menu_item_new_with_label(gettext("Hide Terminal"));
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(toggleTerminal),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label(gettext("CD To Page"));
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(cdHere),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label(gettext("Copy"));
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(copyFromTerm),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label(gettext("Paste"));
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(pasteToTerm),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);

	popmenuitem=gtk_menu_item_new_with_label(gettext("Select All"));
	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(selectAllInTerm),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);
	setTextDomain(false,(plugData*)plugdata);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menu;
	plugData*	plugdata=(plugData*)data;
#ifdef _USEGTK3_
	GdkRGBA		colour;
#else
	GdkColor	colour;
#endif
	char*		startterm[2]={0,0};
	char*		filename;

	setTextDomain(true,plugdata);
	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuView);
	hideMenu=gtk_menu_item_new_with_label(gettext("Hide Terminal"));
	g_signal_connect(G_OBJECT(hideMenu),"activate",G_CALLBACK(toggleTerminal),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),hideMenu);
	gtk_widget_show_all(plugdata->mlist.menuView);

	terminal=vte_terminal_new();
	vte_terminal_set_default_colors((VteTerminal*)terminal);
	vte_terminal_set_scrollback_lines((VteTerminal*)terminal,-1);
	sinkInt=asprintf(&filename,"%s/terminalpane.rc",plugdata->lPlugFolder);
	loadVarsFromFile(filename,mydata);
	debugFree(&filename);

//gdk_rgba_parse
#ifdef _USEGTK3_
	gdk_rgba_parse(&colour,(const gchar*)foreColour);

#ifdef _VTEVERS290_
	vte_terminal_set_color_foreground_rgba((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#else
	vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#endif

	gdk_rgba_parse(&colour,(const gchar*)backColour);
#ifdef _VTEVERS290_
	vte_terminal_set_color_background_rgba((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#else
	vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#endif

#else
	gdk_color_parse((const gchar*)foreColour,&colour);
	vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkColor*)&colour);
	gdk_color_parse((const gchar*)backColour,&colour);
	vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkColor*)&colour);
#endif

	swindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(swindow),terminal);
	gtk_box_pack_start((GtkBox*)plugdata->bottomUserBox,swindow,true,true,0);
	gtk_widget_show_all(plugdata->bottomUserBox);

	g_signal_connect(terminal,"key-press-event",G_CALLBACK(on_key_press),NULL);
	g_signal_connect(terminal,"button-press-event",G_CALLBACK(doButton),(void*)plugdata);
	startterm[0]=vte_get_user_shell();

#ifdef _USEGTK3_

#ifdef _VTEVERS290_
	vte_terminal_fork_command_full((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL);
#else
	vte_terminal_spawn_sync((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL,NULL);
#endif

#else
	vte_terminal_fork_command_full((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL);
#endif

	doStartUpCheck(plugdata);
	showHideTerminal(plugdata,true);
	if(showing==true)
		showTop(false);
	else
		hideTop(false);

	makeMenu(plugdata);
	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	fcolour;
	GtkWidget*	bcolour;
	GtkWidget*	vbox;
	int			response;
#ifdef _USEGTK3_
	GdkRGBA		colour;
#else
	GdkColor	colour;
#endif
	char*		filename;

	plugData*	plugdata=(plugData*)data;

	setTextDomain(true,plugdata);
	vbox=createNewBox(NEWVBOX,false,0);
	dialog=gtk_dialog_new_with_buttons(gettext("Terminal Pane"),(GtkWindow*)plugdata->prefsWindow,GTK_DIALOG_MODAL,gettext(GTK_STOCK_APPLY),GTK_RESPONSE_APPLY,gettext(GTK_STOCK_CANCEL),GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	fcolour=gtk_entry_new();
	bcolour=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)fcolour,foreColour);
	gtk_entry_set_text((GtkEntry*)bcolour,backColour);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Foreground Colour")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,fcolour,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Background Colour")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,bcolour,true,true,4);

	gtk_window_set_transient_for((GtkWindow*)dialog,(GtkWindow*)plugdata->prefsWindow);
	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY);
		{
			free(foreColour);
			free(backColour);
			foreColour=strdup((char*)gtk_entry_get_text((GtkEntry*)fcolour));
			backColour=strdup((char*)gtk_entry_get_text((GtkEntry*)bcolour));

			sinkInt=asprintf(&filename,"%s/terminalpane.rc",plugdata->lPlugFolder);
			saveVarsToFile(filename,mydata);
			debugFree(&filename);
			
			vte_terminal_set_default_colors((VteTerminal*)terminal);
#ifdef _USEGTK3_
			gdk_rgba_parse(&colour,(const gchar*)foreColour);
#ifdef _VTEVERS290_
			vte_terminal_set_color_foreground_rgba((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#else
			vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#endif
			gdk_rgba_parse(&colour,(const gchar*)backColour);
#ifdef _VTEVERS290_
			vte_terminal_set_color_background_rgba((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#else
			vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkRGBA*)&colour);
#endif
#else
			gdk_color_parse((const gchar*)foreColour,&colour);
			vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkColor*)&colour);
			gdk_color_parse((const gchar*)backColour,&colour);
			vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkColor*)&colour);
#endif
			gtk_widget_show_all(terminal);
		}
	gtk_widget_destroy((GtkWidget*)dialog);

	setTextDomain(false,plugdata);
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

	const char		*aboutboxstring=gettext("A Simple Terminal Pane For KKEdit");
	sinkInt=asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	sinkInt=asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Terminal Pane"));
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
