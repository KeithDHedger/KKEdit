/*
 *
 * ©K. D. Hedger. Sun  7 Aug 13:01:41 BST 2016 kdhedger68713@gmail.com

 * This file (browserplugin.cpp) is part of plugins.

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include <vte/vte.h>
//#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>
//#include <webkit2/webkit2.h>
#include <libintl.h>
#include <locale.h>

#include "../config.h"
#include "../common.h"
#include "../kkedit-plugins.h"

#define PLUGVERSION VERSION
#ifdef _USEGTK3_
#define TEXTDOMAIN "browserplugin-3" 
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "browserplugin" 
#define ABOUTICON "KKEditPlug"
#endif

#define GHELPFOLDER DATADIR _EXECSUFFIX_ "/help"

//#define MENU_GO_BACK_LABEL gettext("_Back")
//#define MENU_GO_FORWARD_LABEL DIALOG_GO_FORWARD_LABEL
extern GtkWidget *createNewStockButton(const char *stock,const char *label);
extern char	*thePage;

int	(*module_plug_function)(gpointer globaldata);

WebKitWebView	*browser=NULL;
GtkWidget		*hideMenu=NULL;
bool			showing=false;
//GtkWidget		*swindow;
GtkWidget		*webWindow=NULL;
//GtkWidget		*mainBrowserWindow=NULL;
char			*currentdomain=NULL;
int				sinkInt;

//args		mydata[]=
//				{
//					{"forecol",TYPESTRING,&foreColour},
//					{"backcol",TYPESTRING,&backColour},
//					{NULL,0,NULL}
//				};

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
//G_DEFINE_TYPE(WebKitWebView, webkit_web_view, GTK_TYPE_CONTAINER)

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

	sinkInt=asprintf(&filepath,"%s/browser.rc",pdata->lPlugFolder);
	if(g_file_test(filepath,G_FILE_TEST_EXISTS))
		showing=true;
	else
		showing=false;

	debugFree(&filepath);
}

void showHideBrowser(plugData* plugdata,bool startup)
{
	char*	filepath;

	setTextDomain(true,plugdata);
	sinkInt=asprintf(&filepath,"%s/browser.rc",plugdata->lPlugFolder);
	if(showing==true)
		{
			gtk_widget_show_all((GtkWidget*)browser);
			if(plugdata->bottomShow==0 && startup==false)
				showTop(false);
			touch(filepath);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Hide Browser"));
		}
	else
		{
			gtk_widget_show_all((GtkWidget*)browser);
			unlink(filepath);
			if(plugdata->bottomShow==1 && startup==false)
				hideTop(false);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Show Browser"));
		}
	setTextDomain(false,plugdata);
	debugFree(&filepath);
}

extern "C" int loadPage(plugData* pdata)
{
//	if(thePage!=NULL)
//		webkit_web_view_load_uri(browser,thePage);

webkit_web_view_load_uri(browser,"file:///usr/local/share/KKEdit/help/help.en.html");
	return(0);
}

void toggleBrowser(GtkWidget* widget,gpointer data)
{

	showing=!showing;
	showHideBrowser((plugData*)data,false);
}


//gboolean doButton(GtkWidget *widget, GdkEventButton *event,gpointer data)
//{
//	int button, event_time;
//
//	gtk_widget_set_can_focus(terminal,true);
//	gtk_widget_grab_focus(terminal);
//
/////* Ignore double-clicks and triple-clicks */
//  if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
//    {
//		gtk_widget_show_all(contextMenu);
//		if (event)
//			{
//				button=event->button;
//				event_time=event->time;
//			}
//		else
//			{
//				button=0;
//				event_time=gtk_get_current_event_time();
//				}
//
//		gtk_menu_popup(GTK_MENU(contextMenu),NULL,NULL,NULL,NULL,button,event_time);
//	}
//	return(false);
//}
//
//gboolean on_key_press (GtkWidget *terminal, GdkEventKey *event)
//{
//	if (event->state == (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
//		{
//			switch (event->keyval)
//				{
//				case GDK_C:
//					vte_terminal_copy_clipboard (VTE_TERMINAL (terminal));
//					return true;
//				case GDK_V:
//					vte_terminal_paste_clipboard (VTE_TERMINAL (terminal));
//					return true;
//				}
//		}
//	return false;
//}
//
//void copyFromTerm(GtkWidget* widget,gpointer data)
//{
//	vte_terminal_copy_clipboard((VteTerminal*)terminal);
//}
//
//void pasteToTerm(GtkWidget* widget,gpointer data)
//{
//	vte_terminal_paste_clipboard((VteTerminal*)terminal);
//}
//
//void selectAllInTerm(GtkWidget* widget,gpointer data)
//{
//	vte_terminal_select_all((VteTerminal*)terminal);
//}

//void makeMenu(gpointer plugdata)
//{
//	GtkWidget *popmenuitem;
//
//	contextMenu=gtk_menu_new ();
//
//	setTextDomain(true,(plugData*)plugdata);
//	popmenuitem=gtk_menu_item_new_with_label(gettext("Hide Browser"));
//	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(toggleBrowser),plugdata);
//	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);
//
//	popmenuitem=gtk_menu_item_new_with_label(gettext("CD To Page"));
//	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(cdHere),plugdata);
//	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);
//
//	popmenuitem=gtk_menu_item_new_with_label(gettext("Copy"));
//	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(copyFromTerm),plugdata);
//	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);
//
//	popmenuitem=gtk_menu_item_new_with_label(gettext("Paste"));
//	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(pasteToTerm),plugdata);
//	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);
//
//	popmenuitem=gtk_menu_item_new_with_label(gettext("Select All"));
//	g_signal_connect(G_OBJECT(popmenuitem),"activate",G_CALLBACK(selectAllInTerm),plugdata);
//	gtk_menu_shell_append(GTK_MENU_SHELL(contextMenu),popmenuitem);
//	setTextDomain(false,(plugData*)plugdata);
//}

void webKitGoBack(GtkWidget *widget,gpointer data)
{
printf("XXXXXXXXX\n");
	webkit_web_view_go_back((WebKitWebView*)data);
}

GtkWidget* buildGtkDocViewer(void)
{
	GtkWidget	*vbox;
	GtkWidget	*hbox;
	GtkWidget	*button;
	GtkWidget	*scrolledWindow;
	GtkWidget	*entry;
	GtkWidget	*findbutton;
	GtkWidget	*findnextinpage;
	WebKitWebSettings	*settings;
	GtkWidget	*webVBox=NULL;

//TODO//
//	docView=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//	gtk_window_set_title((GtkWindow*)docView,DOCVIEW_DOCVIEWER_LABEL);

//	gtk_window_set_default_size((GtkWindow*)docView,docWindowWidth,docWindowHeight);
//	if(docWindowX!=-1 && docWindowY!=-1)
//		gtk_window_move((GtkWindow *)docView,docWindowX,docWindowY);

	webVBox=createNewBox(NEWVBOX,false,0);
	hbox=createNewBox(NEWHBOX,false,4);
//	gtk_box_pack_start(GTK_BOX(webVBox),hbox,false,false,4);

	webWindow=gtk_scrolled_window_new(NULL,NULL);
	browser=WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_container_add(GTK_CONTAINER(webWindow),(GtkWidget*)browser);

//	g_signal_connect(G_OBJECT(webView),"navigation-policy-decision-requested",G_CALLBACK(docLinkTrap),NULL);

//	settings=webkit_web_view_get_settings(browser);
//	g_object_set((gpointer)settings,"enable-file-access-from-file-uris",true,NULL);
//	g_object_set((gpointer)settings,"enable-page-cache",true,NULL);
//	g_object_set((gpointer)settings,"enable-plugins",false,NULL);
//	g_object_set((gpointer)settings,"enable-caret-browsing",true,NULL);
//	g_object_set((gpointer)settings,"enable-private-browsing",true,NULL);
//	g_object_set((gpointer)settings,"enable-java-applet",false,NULL);
//
//	scrolledWindow=gtk_scrolled_window_new(NULL,NULL);
//	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
////	gtk_container_add_with_viewport(GTK_CONTAINER(scrolledWindow),GTK_WIDGET(browser));
//gtk_scrolled_window_add_with_viewport((GtkScrolledWindow*)scrolledWindow,GTK_WIDGET(browser));

//
//	//button=createNewStockButton(GTK_STOCK_GO_BACK,MENU_GO_BACK_LABEL);
	button=createNewStockButton(GTK_STOCK_GO_BACK,"Back");
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoBack),(void*)browser);	
//
//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

////	button=createNewStockButton(GTK_STOCK_HOME,DOCVIEW_HOME_LABEL);
	button=createNewStockButton(GTK_STOCK_HOME,"Home");
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
//	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoHome),(void*)webView);	
 
//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

	entry=gtk_entry_new();
//	//findbutton=createNewStockButton(GTK_STOCK_FIND,MENU_FIND_LABEL);
	findbutton=createNewStockButton(GTK_STOCK_FIND,"Find");
	gtk_box_pack_start(GTK_BOX(hbox),findbutton,false,false,0);
//	g_signal_connect(G_OBJECT(findbutton),"clicked",G_CALLBACK(docSearchFromBar),(void*)entry);	

	gtk_box_pack_start(GTK_BOX(hbox),entry,false,true,0);
//	g_signal_connect_after(G_OBJECT(entry),"activate",G_CALLBACK(docSearchFromBar),(void*)entry);

//	//findnextinpage=createNewStockButton(GTK_STOCK_GO_DOWN,DOCVIEW_GO_DOWN_LABEL);
	findnextinpage=createNewStockButton(GTK_STOCK_GO_DOWN,"Down");
	gtk_box_pack_start(GTK_BOX(hbox),findnextinpage,false,false,0);
//	g_signal_connect(G_OBJECT(findnextinpage),"clicked",G_CALLBACK(docSearchInPageFoward),(void*)entry);

//	//findbutton=createNewStockButton(GTK_STOCK_GO_UP,DOCVIEW_GO_UP_LABEL);
	findbutton=createNewStockButton(GTK_STOCK_GO_UP,"Up");
	gtk_box_pack_start(GTK_BOX(hbox),findbutton,false,false,0);
//	g_signal_connect(G_OBJECT(findbutton),"clicked",G_CALLBACK(docSearchInPageBack),(void*)entry);

//spacer
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(" "),true,false,0);

////	button=createNewStockButton(GTK_STOCK_GO_FORWARD,DIALOG_GO_FORWARD_LABEL);
	button=createNewStockButton(GTK_STOCK_GO_FORWARD,"Forward");
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,4);
//	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoForward),(void*)webView);	

//
////	gtk_container_add(GTK_CONTAINER(docView),vbox);
//	gtk_widget_grab_focus(GTK_WIDGET(browser));
//
////	g_signal_connect_object(G_OBJECT(docView),"delete-event",G_CALLBACK(toggleDocviewer),G_OBJECT(docView),G_CONNECT_SWAPPED);
//
//	const char *lang;
////
////	if(strncmp(localeLang,"en",2)==0)
//		lang="en";
////	else
////		lang="fr";
//
//	//sinkInt=asprintf(&thePage,"file://%s/help.%s.html",GHELPFOLDER,lang);
//	//webkit_web_view_load_uri(browser,"file:///usr/share/KKEdit/help/help.en.html");
//	//freeAndNull(&thePage);
//	//free(thePage);
//	//thePage=NULL;
//// gtk_widget_grab_focus(GTK_WIDGET(browser));

	
	gtk_box_pack_start((GtkBox*)webVBox,webWindow,true,true,0);
	gtk_box_pack_start(GTK_BOX(webVBox),hbox,false,false,4);

	return(webVBox);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menu;
	plugData*	plugdata=(plugData*)data;
	char*		filename;

	setTextDomain(true,plugdata);
	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuView);
	hideMenu=gtk_menu_item_new_with_label(gettext("Hide Browser"));
	g_signal_connect(G_OBJECT(hideMenu),"activate",G_CALLBACK(toggleBrowser),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),hideMenu);
	gtk_widget_show_all(plugdata->mlist.menuView);

//TODO//
//	sinkInt=asprintf(&filename,"%s/terminalpane.rc",plugdata->lPlugFolder);
//	loadVarsFromFile(filename,mydata);
//	debugFree(&filename);
//

//	webWindow=gtk_scrolled_window_new(NULL,NULL);
	//buildGtkDocViewer();
//	gtk_container_add(GTK_CONTAINER(webWindow),(GtkWidget*)browser);
	//gtk_widget_show_all(widg);
	
//	gtk_box_pack_start((GtkBox*)plugdata->bottomUserBox,(GtkWidget*)browser,true,true,0);
	gtk_box_pack_start((GtkBox*)plugdata->bottomUserBox,buildGtkDocViewer(),true,true,0);
	
	gtk_widget_show_all(plugdata->bottomUserBox);

//	g_signal_connect(terminal,"key-press-event",G_CALLBACK(on_key_press),NULL);
//	g_signal_connect(terminal,"button-press-event",G_CALLBACK(doButton),(void*)plugdata);
//	startterm[0]=vte_get_user_shell();
//
//#ifdef _USEGTK3_
//
//#ifdef _VTEVERS290_
//	vte_terminal_fork_command_full((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL);
//#else
//	vte_terminal_spawn_sync((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL,NULL);
//#endif
//
//#else
//	vte_terminal_fork_command_full((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL);
//#endif

	doStartUpCheck(plugdata);
	showHideBrowser(plugdata,true);
	if(showing==true)
		showTop(false);
	else
		hideTop(false);

	//makeMenu(plugdata);
	setTextDomain(false,plugdata);
//	webkit_web_view_load_uri(browser,"file:///usr/share/KKEdit/help/help.en.html");
//	webkit_web_view_load_uri(browser,"file:///usr/local/share/KKEdit/help/help.en.html");
	webkit_web_view_load_uri(browser,"http://google.com");
    gtk_widget_grab_focus(GTK_WIDGET(browser));
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
#if 0
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
	dialog=gtk_dialog_new_with_buttons(gettext("Browser Pane"),NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
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
#endif
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

	const char*		aboutboxstring=gettext("A Simple Browser Pane For KKEdit");
	sinkInt=asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	sinkInt=asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Browser Pane"));
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
