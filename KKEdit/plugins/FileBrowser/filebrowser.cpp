/*
 *
 * ©K. D. Hedger. Tue  6 Oct 12:23:43 BST 2015 kdhedger68713@gmail.com

 * This file (filebrowser.cpp) is part of KKEdit.

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
#include <libintl.h>
#include <locale.h>

#include "../common.h"
#include "../kkedit-plugins.h"

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://kkedit.darktech.org"
#define VERSION "0.3.0"
#define NUM_COLUMNS 3
#define COLUMN_FILENAME 1
#define COLUMN_PATHNAME 2
#define COLUMN_PIXBUF 0
#ifdef _USEGTK3_
#define TEXTDOMAIN "FileBrowser-3" 
#define ABOUTICON "KKEditPlug-3"
#else
#define TEXTDOMAIN "FileBrowser" 
#define ABOUTICON "KKEditPlug"
#endif

int	(*module_plug_function)(gpointer globaldata);

GtkWidget*		leftButton;
GtkTreeStore*	store;
GtkWidget*		treeview;
GtkWidget*		scrollbox;
GtkWidget*		leftBox;
bool			flag=false;
GtkWidget*		hideMenu;
bool			showing;
bool			colflag=false;
int				colsize=0;
bool			showInvisible;
gchar*			node=NULL;
char*			currentdomain=NULL;

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

void touch(char* path)
{
	int	fd;

	fd=open(path,O_WRONLY|O_CREAT,DEFFILEMODE);
	if(fd!=-1)
		close(fd);
}

void showHideInvisibles(plugData* pdata)
{
	char*	filepath;

	asprintf(&filepath,"%s/filebrowser.inv",pdata->lPlugFolder);
	if(showInvisible==true)
		touch(filepath);
	else
		unlink(filepath);
	debugFree(&filepath);
}

extern "C" int plugPrefs(gpointer data)
{
	plugData*	plugdata=(plugData*)data;
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	showinvis;
	GtkWidget*	vbox;
	int			response;

	vbox=createNewBox(NEWVBOX,false,0);
	setTextDomain(true,plugdata);

	dialog=gtk_dialog_new_with_buttons(gettext("File Browser Plug In Prefs"),NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	showinvis=gtk_check_button_new_with_label(gettext("Show Invisible File/Folders"));
	gtk_toggle_button_set_active((GtkToggleButton*)showinvis,showInvisible);
	gtk_box_pack_start((GtkBox*)vbox,showinvis,true,true,4);

	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_APPLY);
	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		{
			showInvisible=gtk_toggle_button_get_active((GtkToggleButton*)showinvis);
			showHideInvisibles(plugdata);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
	setTextDomain(false,plugdata);
	return(0);
}

GdkPixbuf* getPixBuf(char* name)
{
	GFile*				file=g_file_new_for_path(name);
	GFileInfo*			file_info=g_file_query_info(file,"standard::*",G_FILE_QUERY_INFO_NONE,NULL,NULL);
	GIcon*				icon=g_file_info_get_icon(file_info);
	GdkPixbuf*			pix=NULL;
	gchar*				path;
	gchar const* const*	names;
	GFile*				icon_file;
	char*				newname;

	if(G_IS_THEMED_ICON(icon))
		{
			names=g_themed_icon_get_names(G_THEMED_ICON(icon));
			pix=gtk_icon_theme_load_icon(gtk_icon_theme_get_default(),*names,16,(GtkIconLookupFlags)(GTK_ICON_LOOKUP_USE_BUILTIN|GTK_ICON_LOOKUP_FORCE_SVG|GTK_ICON_LOOKUP_GENERIC_FALLBACK|GTK_ICON_LOOKUP_FORCE_SIZE),NULL);
			if(pix==NULL)
				{
					asprintf(&newname,"gnome-mime-%s",*names);
					pix=gtk_icon_theme_load_icon(gtk_icon_theme_get_default(),(const gchar*)newname,16,(GtkIconLookupFlags)(GTK_ICON_LOOKUP_USE_BUILTIN|GTK_ICON_LOOKUP_FORCE_SVG|GTK_ICON_LOOKUP_GENERIC_FALLBACK|GTK_ICON_LOOKUP_FORCE_SIZE),NULL);
					debugFree(&newname);
				}
		}
	else if(G_IS_FILE_ICON(icon))
		{
			icon_file=g_file_icon_get_file(G_FILE_ICON(icon));
			path=g_file_get_path(icon_file);
			pix=gdk_pixbuf_new_from_file_at_size(path,16,16,NULL);
			debugFree(&path);
			g_object_unref(G_OBJECT(icon_file));
		}
	g_object_unref(G_OBJECT(file));
	g_object_unref(G_OBJECT(file_info));

   return(pix);
}

void addFolderContents(const char* folder,GtkTreeIter* iter,bool root)
{
	GtkTreeIter child_iter;

	gtk_tree_store_append ((GtkTreeStore*)store,iter,NULL);
	gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,"/",COLUMN_PATHNAME,"/",COLUMN_PIXBUF,getPixBuf((char*)"/"),-1);

	gtk_tree_store_append(store,&child_iter,iter);
	gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,"",-1);
}

void addToIter(GtkTreeView* treeview,char* filename,GtkTreeIter* iter,char* folderpath)
{
	GtkTreeModel*		model;
	GtkTreeIter			childiter;
	GtkTreeIter			parentiter;
	char*				pathname;
	
	if(strcasecmp("/",folderpath)!=0)
		asprintf(&pathname,"%s/%s",folderpath,filename);
	else
		asprintf(&pathname,"/%s",filename);

	model=gtk_tree_view_get_model(treeview);
	gtk_tree_model_iter_parent(model,&parentiter,iter);
	childiter=*iter;

	gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,filename,COLUMN_PATHNAME,pathname,COLUMN_PIXBUF,getPixBuf(pathname),-1);
	if((gtk_tree_model_iter_next(model,&childiter)==false) && (g_file_test(pathname,G_FILE_TEST_IS_DIR)))
		gtk_tree_store_append((GtkTreeStore*)store,&childiter,iter);
}

bool addContents(GtkTreeView* treeview,GtkTreeIter* iter,char* name)
{
	FILE*			fp=NULL;
	char			line[1024];
	char*			command;
	GtkTreeModel*	model;
	GtkTreeIter		parentiter;

	model=gtk_tree_view_get_model(treeview);
	gtk_tree_model_iter_parent(model,&parentiter,iter);

	if(showInvisible==true)
		asprintf(&command,"ls -1 -A \"%s\"|sort",name);
	else
		asprintf(&command,"ls -1 \"%s\"|sort",name);

	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					line[strlen(line)-1]=0;
					addToIter(treeview,line,iter,name);
					if(gtk_tree_model_iter_next(model,iter)==false)
						gtk_tree_store_append((GtkTreeStore*)store,iter,&parentiter);
				}
			pclose(fp);
		}
	return(gtk_tree_store_remove((GtkTreeStore*)store,iter));
}

void expandRow(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
{
	GtkTreeModel*		model;
	bool				gotchild;
	GtkTreeIter			childiter;
	char*				folder;
	GtkTreeIter*		cleariter;
	GtkTreeIter			childchild;

	model=gtk_tree_view_get_model(treeview);
	gotchild=gtk_tree_model_iter_children(model,&childiter,iter);
	gtk_tree_model_get(model,iter,COLUMN_PATHNAME,&folder,-1);
	cleariter=gtk_tree_iter_copy(&childiter);

	gtk_tree_model_iter_next(model,cleariter);
	if(gtk_tree_store_iter_is_valid((GtkTreeStore*)store,cleariter))
		while(gtk_tree_store_remove((GtkTreeStore*)store,cleariter));

	cleariter=gtk_tree_iter_copy(&childiter);
	gtk_tree_model_iter_children(model,&childchild,cleariter);
	if(gtk_tree_store_iter_is_valid((GtkTreeStore*)store,&childchild))
		gtk_tree_store_remove((GtkTreeStore*)store,&childchild);

	while(gotchild)
		{
			if(addContents(treeview,&childiter,(char*)folder))
				gotchild=gtk_tree_model_iter_next(model,&childiter);
			else
				gotchild=false;
		}
	flag=true;
}

void collapseRow(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
{
	char*	pathstring=gtk_tree_path_to_string(path);

	if(strcmp(pathstring,"0")==0)
		flag=false;
	debugFree(&pathstring);
}

const char* getApp(char* name)
{
	GFile*		file=g_file_new_for_path(name);
	GFileInfo*	file_info=g_file_query_info(file,"standard::*",G_FILE_QUERY_INFO_NONE,NULL,NULL);

    const char*	content_type=g_file_info_get_content_type(file_info);
    GAppInfo*	app_info=g_app_info_get_default_for_type(content_type,false);

	return(g_app_info_get_executable(app_info));
}

void onRowActivated(GtkTreeView* treeview, GtkTreePath* path,GtkTreeViewColumn* col,gpointer userdata)
{
	GtkTreeModel		*model;
	GtkTreeIter			iter;
	gchar				*name;
	char				*command;
	GdkModifierType		mask;
	const char			*app;

#ifdef _USEGTK3_
	GdkDeviceManager	*device_manager=gdk_display_get_device_manager(gdk_display_get_default());
	GdkDevice			*device=gdk_device_manager_get_client_pointer(device_manager);
	GdkWindow			*window;

	window=gdk_screen_get_active_window (gdk_screen_get_default());
	gdk_window_get_device_position(window,device,NULL,NULL,&mask);
#else
	gdk_window_get_pointer(NULL,NULL,NULL,&mask);
#endif

	model=gtk_tree_view_get_model(treeview);

	if(gtk_tree_model_get_iter(model,&iter,path))
		{
			gtk_tree_model_get(model,&iter,COLUMN_PATHNAME,&name,-1);
			app=getApp(name);
			asprintf(&command,"%s '%s'",app,name);
			
			if (GDK_CONTROL_MASK & mask )
				runCommand(command,NULL,false,TOOL_ASYNC,true,NULL);
			else
				runCommand(command,NULL,false,TOOL_ASYNC,false,NULL);
				
			debugFree(&command);
			debugFree(&name);
		}
}

void showHideBrowser(plugData* pdata,bool startup)
{
	char*	filepath;

	setTextDomain(true,pdata);
	asprintf(&filepath,"%s/filebrowser.rc",pdata->lPlugFolder);
	if(showing==true)
		{
			gtk_widget_show_all(scrollbox);
			if(pdata->leftShow==0 && startup==false)
				showSide(true);
			touch(filepath);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Hide Browser"));
		}
	else
		{
			gtk_widget_hide(scrollbox);
			unlink(filepath);
			if(pdata->leftShow==1 && startup==false)
				hideSide(true);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,gettext("Show Browser"));
		}
	debugFree(&filepath);
	setTextDomain(false,pdata);
}

void toggleBrowser(GtkWidget* widget,gpointer data)
{
	showing=!showing;
	showHideBrowser((plugData*)data,false);
}

extern "C" int setSensitive(gpointer data)
{
	return(0);
}

extern "C" int switchTab(gpointer data)
{
	return(0);
}

void doStartUpCheck(plugData* pdata)
{
	char*	filepath;

	asprintf(&filepath,"%s/filebrowser.rc",pdata->lPlugFolder);
	if(g_file_test(filepath,G_FILE_TEST_EXISTS))
		showing=true;
	else
		showing=false;

	debugFree(&filepath);

	asprintf(&filepath,"%s/filebrowser.inv",pdata->lPlugFolder);
	if(g_file_test(filepath,G_FILE_TEST_EXISTS))
		showInvisible=true;
	else
		showInvisible=false;

	debugFree(&filepath);
}

gboolean getNode(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter,gpointer data)
{
	char*	name=NULL;
	char*	thispath=NULL;
	bool	retval=false;

	thispath=gtk_tree_path_to_string(path);
	if(node!=NULL && thispath!=NULL)
		{
			if((strlen(thispath)<strlen(node)) || (strcmp(thispath,node)==0))
				{
					debugFree(&thispath);
					return(false);
				}
		}

	gtk_tree_model_get(model,iter,COLUMN_FILENAME,&name,-1);
	if(name!=NULL)
		{
			if(strcmp(name,(char*)data)==0)
				{
					debugFree(&node);
					node=gtk_tree_path_to_string(path);
					gtk_tree_view_expand_row((GtkTreeView*)treeview,path,false);
					retval=true;
				}
			debugFree(&name);
		}
	debugFree(&thispath);
	return(retval);
}

extern "C" void doTabMenu(GtkWidget *widget,gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	pageStruct*		page=plugdata->page;

	char*			ptr=strdup(page->filePath);
	GtkTreeModel*	model=NULL;
	GtkTreePath*	scrollpath;

	gtk_tree_view_collapse_all((GtkTreeView *)treeview);

	ptr=strtok(ptr,"/");

	model=GTK_TREE_MODEL(store);
	gtk_tree_model_foreach(model,getNode,(gpointer)"/");

	while(ptr!=NULL)
		{
			gtk_tree_model_foreach(model,getNode,ptr);
			ptr=strtok(NULL,"/");
		}

	scrollpath=gtk_tree_path_new_from_string(node);
	gtk_tree_view_set_cursor((GtkTreeView *)treeview,scrollpath,NULL,false);
	gtk_tree_view_scroll_to_cell((GtkTreeView *)treeview,scrollpath,NULL,true,0.5,0.5);
	debugFree(&node);
	node=NULL;

	showing=false;
	toggleBrowser(NULL,plugdata);
}

extern "C" int addToTab(gpointer data)
{
	GtkWidget*	menuitem;
	plugData*	plugdata=(plugData*)data;

	setTextDomain(true,plugdata);

	menuitem=createNewImageMenuItem(GTK_STOCK_OPEN,gettext("Open In Browser"));
	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->tabPopUpMenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)plugdata);

	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int addToContext(gpointer data)
{
	GtkWidget*	menuitem;
	plugData*	plugdata=(plugData*)data;

	menuitem=createNewImageMenuItem(GTK_STOCK_OPEN,gettext("Open In Browser"));
	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->contextPopUpMenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)plugdata);

	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int addToGui(gpointer data)
{
	plugData*			plugdata=(plugData*)data;
	GtkTreeViewColumn*	column;
	GtkTreeModel*		model=NULL;
	GtkCellRenderer*	renderer;
	GtkTreeIter			iter;
	GtkWidget*			menu;

	setTextDomain(true,plugdata);

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuView);
	hideMenu=gtk_menu_item_new_with_label(gettext("Hide Browser"));
	g_signal_connect(G_OBJECT(hideMenu),"activate",G_CALLBACK(toggleBrowser),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),hideMenu);
	gtk_widget_show_all(plugdata->mlist.menuView);

	store=gtk_tree_store_new(NUM_COLUMNS,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);
	addFolderContents("/",&iter,true);
	model=GTK_TREE_MODEL(store);
	treeview=gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_enable_tree_lines((GtkTreeView*)treeview,true);
	scrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrollbox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

	gtk_container_add(GTK_CONTAINER(scrollbox),(GtkWidget*)treeview);
	gtk_box_pack_start((GtkBox*)plugdata->leftUserBox,scrollbox,true,true,0);

	gtk_widget_show_all(plugdata->leftUserBox);
	gtk_widget_set_size_request((GtkWidget*)scrollbox,100,-1);

//pixbuf
	renderer=gtk_cell_renderer_pixbuf_new();
	column=gtk_tree_view_column_new_with_attributes("path",renderer,"pixbuf",COLUMN_PIXBUF,NULL);
	gtk_tree_view_column_set_resizable(column,false);
	gtk_tree_view_column_set_sizing(column,GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);
	gtk_tree_view_column_set_spacing(column,0);
	gtk_tree_view_column_set_expand(column,false);

//colom file
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes(NULL,renderer,"text",COLUMN_FILENAME,NULL);
	gtk_tree_view_column_set_resizable(column,true);
	gtk_tree_view_column_set_sizing(column,GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);
	gtk_tree_view_column_set_expand(column,true);
	gtk_tree_view_column_set_spacing(column,0);

	gtk_tree_view_set_headers_visible((GtkTreeView*)treeview,false);

	g_signal_connect(treeview,"row-expanded",G_CALLBACK(expandRow),column);
	g_signal_connect(treeview,"row-collapsed",G_CALLBACK(collapseRow),column);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(onRowActivated),NULL);

	leftBox=(GtkWidget*)plugdata->leftUserBox;
	doStartUpCheck(plugdata);
	showHideBrowser(plugdata,true);
	if(showing==true)
		showSide(true);
	else
		hideSide(true);

	setTextDomain(false,plugdata);

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

	const char*		aboutboxstring=gettext("File Browser - Add's a simple file browser to the left pane");
	const char*	authors[]= {"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,gettext("\nMore by the same author\n"),"Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("FileBrowser"));
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,copyright);
	gtk_about_dialog_set_version(about,VERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
	gtk_about_dialog_set_logo_icon_name(about,ABOUTICON);
	gtk_about_dialog_set_license(about,licence);
	gtk_about_dialog_set_translator_credits(about,(const gchar*)translators);

	gtk_dialog_run(GTK_DIALOG(about));
	gtk_widget_destroy((GtkWidget*)about);
	debugFree(&licence);
	debugFree(&licencepath);
	free(translators);

	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(scrollbox);
			gtk_widget_destroy(hideMenu);
			hideSide(true);
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
		}
	return(0);
}
