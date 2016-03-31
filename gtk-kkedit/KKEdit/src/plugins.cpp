/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:52:02 GMT 2015 kdhedger68713@gmail.com

 * This file (plugins.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kkedit-includes.h"

#define COLUMN_ENABLE 0
#define COLUMN_PLUGIN 1
#define NUM_COLUMNS 2

GtkWidget*	plugAboutButton;
GtkWidget*	plugPrefsButton;
GtkWidget*	plugwindow;
GtkWidget*	treeview;

void enableToggled(GtkCellRendererToggle *cell,gchar *path_str,gpointer data)
{
	ERRDATA
	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreeIter  iter;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	gboolean fixed;

	/* get toggled iter */
	gtk_tree_model_get_iter (model,&iter, path);
	gtk_tree_model_get (model, &iter, COLUMN_ENABLE, &fixed, -1);

	/* do something with the value */
	fixed ^= 1;

	/* set new value */
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_ENABLE, fixed, -1);

	/* clean up */
	gtk_tree_path_free (path);
	ERRDATA
}

void getPlugName(gpointer data,gpointer store)
{
	ERRDATA
	GtkTreeIter		iter;
	moduleData*		plugdata;

	plugdata=(moduleData*)data;
	gtk_list_store_append((GtkListStore*)store,&iter);
	gtk_list_store_set((GtkListStore*)store,&iter,COLUMN_ENABLE,plugdata->enabled,COLUMN_PLUGIN,plugdata->name,-1);
	ERRDATA
}

gboolean doSetPlugData(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	ERRDATA
	int			enabled;
	char*		name=NULL;
	moduleData*	pd;
	char*		filepath;
	GtkWidget*	dialog;

	gtk_tree_model_get(model,iter,COLUMN_ENABLE,&enabled,COLUMN_PLUGIN,&name,-1);
	if(!enabled)
		globalPlugins->appendToBlackList(name);

	pd=globalPlugins->getPluginByName(name);

	if((pd->loaded==true) && (!enabled))
			{
				if(globalPlugins->enablePlugin(name,true)==0)
					{
						pd->loaded=!g_module_close(pd->module);
						pd->enabled=pd->loaded;
						pd->module=NULL;
						pd->loaded=false;
						pd->enabled=false;
						ERRDATA return(false);
					}
				else
					{
						dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Plugin '%s' cannot be unloaded yet.\nRestart KKEdit to unload."),name);
						gtk_dialog_run(GTK_DIALOG(dialog));
						gtk_widget_destroy(dialog);
						pd->enabled=false;
						ERRDATA return(false);
					}
			}

	if((pd->loaded==false) && (enabled))
		{
			filepath=globalPlugins->getPluginPathByName(name);
			if(filepath==NULL)
				{
					printf("Can't open: %s\n",name);
					ERRDATA return(false);
				}

			pd->module=g_module_open(filepath,G_MODULE_BIND_LAZY);
			ERRDATA debugFree(&filepath);
				
			if(pd->module!= NULL)
				{
					pd->loaded=true;
					pd->enabled=true;
					globalPlugins->enablePlugin(name,false);
				}
			else
				{
					pd->loaded=false;
					pd->enabled=false;
					printf("Can't open: %s\n",filepath);
				}
		}

	ERRDATA return(false);
}

void setPlugsEnabled(void)
{
	ERRDATA
	GtkTreeModel*	model;

	globalPlugins->deleteBlackList();
	model=gtk_tree_view_get_model((GtkTreeView*)treeview);
    gtk_tree_model_foreach(model,doSetPlugData,NULL);
	ERRDATA
}

void setPlugPrefs(GtkWidget* widget,gpointer data)
{
	ERRDATA
	GtkTreeModel*		model;
	GtkTreeSelection*	selection=NULL;
	GtkTreeIter			iter;
	char*				plugname;

	switch(long(data))
		{
			case 1:
				selection=gtk_tree_view_get_selection((GtkTreeView*)treeview);
				if((selection!=NULL) && (gtk_tree_selection_get_selected(selection,&model,&iter)))
					{
						gtk_tree_model_get(model,&iter,COLUMN_PLUGIN,&plugname,-1);
						globalPlugins->runPlugFunction(globalPlugins->getPluginByName(plugname),"plugPrefs");
						ERRDATA debugFree(&plugname);
					}
				break;
			case 2:
				selection=gtk_tree_view_get_selection((GtkTreeView*)treeview);
				if((selection!=NULL) && (gtk_tree_selection_get_selected(selection,&model,&iter)))
					{
						gtk_tree_model_get(model,&iter,COLUMN_PLUGIN,&plugname,-1);
						globalPlugins->runPlugFunction(globalPlugins->getPluginByName(plugname),"doAbout");
						ERRDATA debugFree(&plugname);
					}
				break;
//apply
			case 3:
				setPlugsEnabled();
				break;
//close
			case 4:
				gtk_widget_hide(plugwindow);
				gtk_widget_destroy(plugwindow);
				break;
		}
	ERRDATA
}

void onRowSelected(GtkTreeView* treeview,gpointer userdata)
{
	ERRDATA
	GtkTreeModel*		model;
	GtkTreeSelection*	selection=NULL;
	GtkTreeIter			iter;
	char*				plugname;

	selection=gtk_tree_view_get_selection((GtkTreeView*)treeview);
	if((selection!=NULL) && (gtk_tree_selection_get_selected(selection,&model,&iter)))
		{
			gtk_tree_model_get(model,&iter,COLUMN_PLUGIN,&plugname,-1);
			gtk_widget_set_sensitive(plugAboutButton,globalPlugins->checkForFunction(plugname,"doAbout"));
			gtk_widget_set_sensitive(plugPrefsButton,globalPlugins->checkForFunction(plugname,"plugPrefs"));
			ERRDATA debugFree(&plugname);
		}
	ERRDATA
}

VISIBLE void doPlugPrefs(GtkWidget* widget,gpointer data)
{
	ERRDATA
	GtkWidget*		vbox;
	GtkListStore*	store;
	GtkTreeModel*	model=NULL;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkWidget*		button;
	GtkWidget*		hbox;

	plugwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	vbox=createNewBox(NEWVBOX,false,4);

	store=gtk_list_store_new(NUM_COLUMNS,G_TYPE_BOOLEAN,G_TYPE_STRING);

	g_list_foreach(globalPlugins->plugins,getPlugName,store);

	model=GTK_TREE_MODEL(store);
	treeview=gtk_tree_view_new_with_model(model);
	gtk_container_add (GTK_CONTAINER (vbox),treeview);

//enable
	renderer=gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer,"toggled",G_CALLBACK(enableToggled),model);

	column=gtk_tree_view_column_new_with_attributes(gettext("Enable"),renderer,"active",COLUMN_ENABLE,NULL);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);

//plug
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes(gettext("Plug In"),renderer,"text",COLUMN_PLUGIN,NULL);
	gtk_tree_view_column_set_sort_column_id(column,COLUMN_PLUGIN);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);


//plugin prefs
	hbox=createNewBox(NEWHBOX,false,4);
	plugPrefsButton=createNewStockButton(GTK_STOCK_PREFERENCES,GTK_STOCK_PREFERENCES3);
	gtk_box_pack_start((GtkBox*)hbox,plugPrefsButton,false,false,4);
	g_signal_connect(G_OBJECT(plugPrefsButton),"clicked",G_CALLBACK(setPlugPrefs),(void*)1);

//plugin about
	plugAboutButton=createNewStockButton(GTK_STOCK_ABOUT,GTK_STOCK_ABOUT3);
	gtk_box_pack_start((GtkBox*)hbox,plugAboutButton,false,false,4);
	g_signal_connect(G_OBJECT(plugAboutButton),"clicked",G_CALLBACK(setPlugPrefs),(void*)2);

//apply
	button=createNewStockButton(GTK_STOCK_APPLY,GTK_STOCK_APPLY);
	gtk_box_pack_start((GtkBox*)hbox,button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setPlugPrefs),(void*)3);

//close
	button=createNewStockButton(GTK_STOCK_CLOSE,GTK_STOCK_CLOSE3);
	gtk_box_pack_start((GtkBox*)hbox,button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setPlugPrefs),(void*)4);

	g_signal_connect(treeview,"cursor-changed",G_CALLBACK(onRowSelected),NULL);

	gtk_container_add (GTK_CONTAINER(vbox),hbox);
	gtk_container_add (GTK_CONTAINER(plugwindow),vbox);
	gtk_widget_show_all(plugwindow);
	ERRDATA
}

//left/right
VISIBLE void showSide(bool left)
{
	ERRDATA
	if(left==true)
		{
			gtk_widget_show(globalPlugins->globalPlugData->leftUserBox);
			globalPlugins->globalPlugData->leftShow++;
		}
	else
		{
			gtk_widget_show(globalPlugins->globalPlugData->rightUserBox);
			globalPlugins->globalPlugData->rightShow++;
		}
	ERRDATA
}

VISIBLE void hideSide(bool left)
{
	ERRDATA
	if(left==true)
		{
			globalPlugins->globalPlugData->leftShow--;
			if(globalPlugins->globalPlugData->leftShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->leftUserBox);
					globalPlugins->globalPlugData->leftShow=0;
				}
		}
	else
		{
			globalPlugins->globalPlugData->rightShow--;
			if(globalPlugins->globalPlugData->rightShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->rightUserBox);
					globalPlugins->globalPlugData->rightShow=0;
				}
		}
	
	ERRDATA
}

//top/bottom
VISIBLE void showTop(bool top)
{
	ERRDATA
	if(top==true)
		{
			gtk_widget_show(globalPlugins->globalPlugData->topUserBox);
			globalPlugins->globalPlugData->topShow++;
		}
	else
		{
			gtk_widget_show(globalPlugins->globalPlugData->bottomUserBox);
			globalPlugins->globalPlugData->bottomShow++;
		}
	ERRDATA
}

VISIBLE void hideTop(bool top)
{
	ERRDATA
	if(top==true)
		{
			globalPlugins->globalPlugData->topShow--;
			if(globalPlugins->globalPlugData->topShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->topUserBox);
					globalPlugins->globalPlugData->topShow=0;
				}
		}
	else
		{
			globalPlugins->globalPlugData->bottomShow--;
			if(globalPlugins->globalPlugData->bottomShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->bottomUserBox);
					globalPlugins->globalPlugData->bottomShow=0;
				}
		}
	ERRDATA
}




