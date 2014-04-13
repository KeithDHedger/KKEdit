/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>

#include "config.h"
#include "globals.h"
#include "files.h"
#include "callbacks.h"
#include "navcallbacks.h"
#include "searchcallbacks.h"
#include "spellcheck.h"

#ifdef BUILDDOCVIEWER
#include <webkit/webkit.h>
#endif

void selectToolOptions(GtkWidget* widget,gpointer data)
{
	char*			datafolder[2];
	char*			nameoftool=NULL;
	GDir*			folder;
	const gchar*	entry=NULL;
	FILE*			fd=NULL;
	char*			filepath;
	char			buffer[4096];
	char			strarg[1024];

	int				intermarg=0;
	int				inpopup=0;
	int				alwayspopup=0;
	int				flagsarg=0;

	char*			text=gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);

	asprintf(&datafolder[0],"%s/tools/",DATADIR);
	asprintf(&datafolder[1],"%s/.KKEdit/tools/",getenv("HOME"));

	for(int loop=0;loop<2;loop++)
		{
			folder=g_dir_open(datafolder[loop],0,NULL);

			if(folder!=NULL)
				{
					entry=g_dir_read_name(folder);
					while(entry!=NULL)
						{
							asprintf(&filepath,"%s%s",datafolder[loop],entry);
							fd=fopen(filepath,"r");
							if(fd!=NULL)
								{
									while(fgets(buffer,4096,fd))
										{
											buffer[strlen(buffer)-1]=0;
											sscanf((char*)&buffer,"%s",(char*)&strarg);
											if(strcmp(strarg,"name")==0)
												{
													nameoftool=strdup((char*)&buffer[5]);
													if((nameoftool!=NULL) &&(strlen(nameoftool)>0) && (strcmp(nameoftool,text)==0))
														{
															if(selectedToolPath!=NULL)
																g_free(selectedToolPath);
															selectedToolPath=strdup(filepath);
															gtk_entry_set_text((GtkEntry*)toolNameWidget,nameoftool);
														}
												}
										}
									fclose(fd);
								}
							g_free(filepath);
							entry=g_dir_read_name(folder);
						}
				}
		}
	if(selectedToolPath!=NULL)
		{
			fd=fopen(selectedToolPath,"r");
			if(fd!=NULL)
				{
					intermarg=0;
					inpopup=0;
					flagsarg=0;
					alwayspopup=0;
					gtk_toggle_button_set_active((GtkToggleButton*)inPopupWidget,(bool)inpopup);
					gtk_toggle_button_set_active((GtkToggleButton*)alwaysPopupWidget,(bool)alwayspopup);
					gtk_toggle_button_set_active((GtkToggleButton*)inTermWidget,(bool)intermarg);
					gtk_entry_set_text((GtkEntry*)commentWidget,"");
					while(fgets(buffer,4096,fd))
						{
							buffer[strlen(buffer)-1]=0;
							sscanf((char*)&buffer,"%s",(char*)&strarg);
							if(strcmp(strarg,"name")==0)
								gtk_entry_set_text((GtkEntry*)toolNameWidget,(char*)&buffer[5]);
							if(strcmp(strarg,"command")==0)
								gtk_entry_set_text((GtkEntry*)commandLineWidget,(char*)&buffer[8]);
							if(strcmp(strarg,"comment")==0)
								gtk_entry_set_text((GtkEntry*)commentWidget,(char*)&buffer[8]);
							if(strcmp(strarg,"inpopup")==0)
								{
									sscanf((char*)&buffer,"%*s %i",&inpopup);
									gtk_toggle_button_set_active((GtkToggleButton*)inPopupWidget,(bool)inpopup);
								}
							if(strcmp(strarg,"alwayspopup")==0)
								{
									sscanf((char*)&buffer,"%*s %i",&alwayspopup);
									gtk_toggle_button_set_active((GtkToggleButton*)alwaysPopupWidget,(bool)alwayspopup);
								}
							if(strcmp(strarg,"interm")==0)
								{
									sscanf((char*)&buffer,"%*s %i",&intermarg);
									gtk_toggle_button_set_active((GtkToggleButton*)inTermWidget,(bool)intermarg);
									if((bool)intermarg==true)
										{
											gtk_widget_set_sensitive(ignoreWidget,false);
											gtk_widget_set_sensitive(pasteWidget,false);
											gtk_widget_set_sensitive(replaceWidget,false);
										}
								}
							if(strcmp(strarg,"flags")==0)
								{
									sscanf((char*)&buffer,"%*s %i",&flagsarg);
									if(flagsarg & TOOL_ASYNC)
										gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,false);
									else
										gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,true);

									if((flagsarg & TOOL_INSERT_MASK)==TOOL_IGNORE_OP)
										gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
									if((flagsarg & TOOL_INSERT_MASK)==TOOL_PASTE_OP)
										gtk_toggle_button_set_active((GtkToggleButton*)pasteWidget,true);
									if((flagsarg & TOOL_INSERT_MASK)==TOOL_REPLACE_OP)
										gtk_toggle_button_set_active((GtkToggleButton*)replaceWidget,true);

									if(flagsarg & TOOL_SHOW_DOC)
										gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,true);
									else
										gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,false);
								}
							}
						fclose(fd);
					}
		}
	g_free(text);
}

void fillCombo(GtkComboBoxText* combo)
{
	GDir*			folder;
	const gchar*	entry=NULL;
	FILE*			fd=NULL;
	char*			filepath;
	char			buffer[4096];
	char*			datafolder[2];
	char			strarg[1024];
	char*			menuname=NULL;

	asprintf(&datafolder[0],"%s/tools/",DATADIR);
	asprintf(&datafolder[1],"%s/.KKEdit/tools/",getenv("HOME"));

	for(int loop=0;loop<2;loop++)
		{
			folder=g_dir_open(datafolder[loop],0,NULL);

			if(folder!=NULL)
				{
					entry=g_dir_read_name(folder);
					while(entry!=NULL)
						{
							asprintf(&filepath,"%s%s",datafolder[loop],entry);
							fd=fopen(filepath,"r");
							if(fd!=NULL)
								{
									while(fgets(buffer,4096,fd))
										{
											buffer[strlen(buffer)-1]=0;
											sscanf((char*)&buffer,"%s",(char*)&strarg);
											if(strcmp(strarg,"name")==0)
												asprintf(&menuname,"%.*s",(int)strlen(buffer)-5,(char*)&buffer[5]);
										}
									if((menuname!=NULL) &&(strlen(menuname)>0))
										gtk_combo_box_text_append_text((GtkComboBoxText*)toolSelect,menuname);
									fclose(fd);
								}
							entry=g_dir_read_name(folder);
						}
				}
		}
}

void doMakeTool(void)
{
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;
	GtkWidget*	toolwin;
	GtkWidget*	infolabel;

	toolwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)toolwin,"Edit External Tools");
	vbox=gtk_vbox_new(false,8);

//select tool
	toolSelect=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(vbox),toolSelect,false,true,0);
	g_signal_connect(G_OBJECT(toolSelect),"changed",G_CALLBACK(selectToolOptions),NULL);
	fillCombo((GtkComboBoxText*) toolSelect);

//name
	toolNameWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Tool Name: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),toolNameWidget);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(toolNameWidget);
	gtk_entry_set_text((GtkEntry*)toolNameWidget,"New Tool");

//command
	commandLineWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Command: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),commandLineWidget);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(commandLineWidget);
	gtk_entry_set_text((GtkEntry*)commandLineWidget,"");

//comment
	commentWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Comment: "),false,true,0);
	gtk_container_add(GTK_CONTAINER(hbox),commentWidget);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(commentWidget);
	gtk_entry_set_text((GtkEntry*)commentWidget,"");

//info
	infolabel=gtk_label_new(PLACEHOLDERINFO);
	gtk_label_set_selectable((GtkLabel*)infolabel,true);
	gtk_box_pack_start(GTK_BOX(vbox),infolabel,false,false,0);
//in terminal
	inTermWidget=gtk_check_button_new_with_label("Run Tool In Terminal");
	gtk_widget_set_name(inTermWidget,"interm");
	gtk_toggle_button_set_active((GtkToggleButton*)inTermWidget,inTerm);
	gtk_box_pack_start(GTK_BOX(vbox),inTermWidget,false,true,0);
	g_signal_connect(G_OBJECT(inTermWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//show in popup menu
	inPopupWidget=gtk_check_button_new_with_label("Show Tool In Pop-Up Menu");
	gtk_widget_set_name(inPopupWidget,"inpopup");
	gtk_toggle_button_set_active((GtkToggleButton*)inPopupWidget,inPopup);
	gtk_box_pack_start(GTK_BOX(vbox),inPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(inPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//always show in popup
	alwaysPopupWidget=gtk_check_button_new_with_label("Always Show Tool In Pop-Up Menu");
	gtk_widget_set_name(alwaysPopupWidget,"alwayspopup");
	gtk_toggle_button_set_active((GtkToggleButton*)alwaysPopupWidget,alwaysPopup);
	gtk_box_pack_start(GTK_BOX(vbox),alwaysPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(alwaysPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags
//snch/async
	syncWidget=gtk_check_button_new_with_label("Run Tool Synchronously");
	gtk_widget_set_name(syncWidget,"sync");
	gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,runSync);
	gtk_box_pack_start(GTK_BOX(vbox),syncWidget,false,true,0);
	g_signal_connect(G_OBJECT(syncWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - show html doc
	showDocWidget=gtk_check_button_new_with_label("Show HTML Doc");
	gtk_widget_set_name(showDocWidget,"showdoc");
	gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,showDoc);
	gtk_box_pack_start(GTK_BOX(vbox),showDocWidget,false,true,0);
	g_signal_connect(G_OBJECT(showDocWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - ignore
	ignoreWidget=gtk_radio_button_new_with_label(NULL,"Ignore Output");
	gtk_widget_set_name(ignoreWidget,"ignore");
	gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,ignoreOut);
	gtk_box_pack_start(GTK_BOX(vbox),ignoreWidget,false,true,0);
	g_signal_connect(G_OBJECT(ignoreWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - paste
	pasteWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Paste Output");
	gtk_widget_set_name(pasteWidget,"paste");
	gtk_toggle_button_set_active((GtkToggleButton*)pasteWidget,pasteOut);
	gtk_box_pack_start(GTK_BOX(vbox),pasteWidget,false,true,0);
	g_signal_connect(G_OBJECT(pasteWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - replace all
	replaceWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Replace All Contents");
	gtk_widget_set_name(replaceWidget,"replace");
	gtk_toggle_button_set_active((GtkToggleButton*)replaceWidget,replaceOut);
	gtk_box_pack_start(GTK_BOX(vbox),replaceWidget,false,true,0);
	g_signal_connect(G_OBJECT(replaceWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//buttons
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);

	hbox=gtk_hbox_new(false,0);
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"apply");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"delete");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"cancel");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,false,2);

//show it
	gtk_container_add(GTK_CONTAINER(toolwin),(GtkWidget*)vbox);
	gtk_widget_show_all(toolwin);
}

void buildTools(void)
{
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GtkWidget*		image;
	GList*			ptr;

	buildToolsList();

	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)menutools);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)menutools,NULL);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menutools),menu);

//addtool
	menuitem=gtk_image_menu_item_new_with_label("Manage External Tools");
	image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doMakeTool),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	ptr=toolsList;
	while(ptr!=NULL)
		{
			menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
			if(((toolStruct*)ptr->data)->comment!=NULL)
				gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
			ptr=g_list_next(ptr);
		}
}

static GtkWidget *mywindow = NULL;
      GtkWidget *icon_view;

GtkIconView*	iconView=NULL;
GtkListStore*	listStore=NULL;

enum {PIXBUF_COLUMN,TEXT_COLUMN,FILE_NAME};

enum
{
  COL_TEXT,
  NUM_COLS
};


static void
fill_store (GtkListStore *store)
{
  GtkTreeIter iter;
  const gchar *text[] = { "Red", "Green", "Blue", "Yellow" };
  gint i;

  /* First clear the store */
  gtk_list_store_clear (store);

  for (i = 0; i < 4; i++)
    {
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, COL_TEXT, text[i], -1);
//	gtk_list_store_set(previewBox[whatBox].store,&iter,PIXBUF_COLUMN,pixbuf,TEXT_COLUMN,iconName,FILE_NAME,dbPath,-1);
    }
}

void populateStore(void)
{
	GtkImage*	image;
	GdkPixbuf*	pbuf;
	GtkTreeIter iter;

	gtk_list_store_clear(listStore);

	gtk_list_store_append (listStore, &iter);
	image=NULL;
	image=(GtkImage*)gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	if(image==NULL)
		printf("XXXXXXXXXX\n");
	gtk_widget_show((GtkWidget*)image);
	pbuf=gdk_pixbuf_new_from_file            ("/media/LinuxData/Development/Projects/KKEdit/KKEdit/resources/pixmaps/MenuKKEdit.png",
                                                         NULL);
//	pbuf=gtk_image_get_pixbuf(image);
	gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,"O",FILE_NAME,"XX",-1);
	g_object_unref(pbuf);

	gtk_list_store_append (listStore, &iter);

	pbuf=gdk_pixbuf_new_from_file            ("/media/LinuxData/Development/Projects/KKEdit/KKEdit/resources/pixmaps/ROOTKKEdit.png",
                                                         NULL);
//	pbuf=gtk_image_get_pixbuf(image);
	gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,"S",FILE_NAME,"XX",-1);
	g_object_unref(pbuf);

	gtk_list_store_append (listStore, &iter);
	image=(GtkImage*)gtk_image_new_from_stock(GTK_STOCK_NEW,GTK_ICON_SIZE_MENU);
	pbuf=gtk_image_get_pixbuf(image);
	if(pbuf!=NULL)
	{
	gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,"O",FILE_NAME,"XX",-1);
//	g_object_unref(pbuf);
}
/*
	gtk_list_store_append (listStore, &iter);
	image=(GtkImage*)gtk_image_new_from_stock(GTK_STOCK_NEW,GTK_ICON_SIZE_MENU);
	pbuf=gtk_image_get_pixbuf(image);
	gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,"O",FILE_NAME,"XX",-1);
	g_object_unref(pbuf);

	gtk_list_store_append (listStore, &iter);
	image=(GtkImage*)gtk_image_new_from_stock(GTK_STOCK_SAVE,GTK_ICON_SIZE_MENU);
	pbuf=gtk_image_get_pixbuf(image);
	gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pbuf,TEXT_COLUMN,"O",FILE_NAME,"XX",-1);
	g_object_unref(pbuf);
*/
}

static GtkListStore *
create_store (void)
{
  GtkListStore *store;

//  store = gtk_list_store_new (1, G_TYPE_STRING);
store=gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);
//gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(previewBox[j].iconView),PIXBUF_COLUMN);
//			gtk_icon_view_set_text_column(GTK_ICON_VIEW(previewBox[j].iconView),TEXT_COLUMN);

  return store;
}


static void
edited (GtkCellRendererText *cell,
	gchar               *path_string,
	gchar               *text,
	gpointer             data)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreePath *path;

  model = gtk_icon_view_get_model (GTK_ICON_VIEW (data));
  path = gtk_tree_path_new_from_string (path_string);

  gtk_tree_model_get_iter (model, &iter, path);
  gtk_list_store_set (GTK_LIST_STORE (model), &iter,
		      COL_TEXT, text, -1);

  gtk_tree_path_free (path);
}

void dodata(GtkWidget* widget,gpointer data)
{
  GtkTreeIter iter;
  GtkTreePath *path;
 gboolean	valid;
  gint row_count = 0;
  GtkTreeModel *model;
model = gtk_icon_view_get_model (GTK_ICON_VIEW (iconView));

valid =  gtk_tree_model_get_iter_first(model,&iter);
   while (valid)
    {
      /* Walk through the list, reading each row */
      gchar *str_data;
      gint   int_data;
      /* Make sure you terminate calls to gtk_tree_model_get()
       * with a '-1' value
       */
      gtk_tree_model_get (model, &iter,
                          TEXT_COLUMN, &str_data,-1);
              printf("XXX%sXXX\n",str_data);
      /* Do something with the data */
     // g_print ("Row %d: (%s,%d)\n", row_count, str_data, int_data);
      //g_free (str_data);
      row_count ++;
      valid = gtk_tree_model_iter_next (model, &iter);
    }

}

GtkWidget *
do_iconview_edit (GtkWidget *do_widget)
{
	GtkWidget*	item;
	GtkWidget*	vbox;



	vbox=gtk_vbox_new(false,8);

  if (!mywindow)
    {
      GtkListStore *store;
      GtkCellRenderer *renderer;

      mywindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);

      gtk_window_set_screen (GTK_WINDOW (mywindow),
			     gtk_widget_get_screen (do_widget));
      gtk_window_set_title (GTK_WINDOW (mywindow), "Editing and Drag-and-Drop");

      g_signal_connect (mywindow, "destroy",
			G_CALLBACK (gtk_widget_destroyed), &mywindow);


		listStore=gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);
		iconView=(GtkIconView*)gtk_icon_view_new();
		gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconView),PIXBUF_COLUMN);
		//gtk_icon_view_set_text_column(GTK_ICON_VIEW(iconView),TEXT_COLUMN);
		gtk_icon_view_set_model(GTK_ICON_VIEW(iconView),GTK_TREE_MODEL(listStore));

		populateStore();

 //     store = create_store ();
 //     icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (store));
     
//      fill_store (store);

/*
			previewBox[j].iconView=(GtkIconView*)gtk_icon_view_new();
			previewBox[j].itemCnt=0;
			previewBox[j].partIter=NULL;
			previewBox[j].store=gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);

			gtk_icon_view_set_item_width(previewBox[j].iconView,itemSize);
			gtk_icon_view_set_item_padding(previewBox[j].iconView,0);
			gtk_icon_view_set_column_spacing(previewBox[j].iconView,0);
			gtk_icon_view_set_spacing(previewBox[j].iconView,0);

			gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(previewBox[j].iconView),PIXBUF_COLUMN);
			gtk_icon_view_set_text_column(GTK_ICON_VIEW(previewBox[j].iconView),TEXT_COLUMN);

			gtk_icon_view_set_model(GTK_ICON_VIEW(previewBox[j].iconView),GTK_TREE_MODEL(previewBox[j].store));

*/


//       g_object_unref (store);

      gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (iconView),
					GTK_SELECTION_SINGLE);
      gtk_icon_view_set_orientation (GTK_ICON_VIEW (iconView),
				     GTK_ORIENTATION_HORIZONTAL);
      gtk_icon_view_set_columns (GTK_ICON_VIEW (iconView), 4);
      gtk_icon_view_set_reorderable (GTK_ICON_VIEW (iconView), TRUE);

      renderer = gtk_cell_renderer_pixbuf_new ();
      gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (iconView),
				  renderer, TRUE);
 //     gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (icon_view),
//					  renderer,
//					  set_cell_color,
//					  NULL, NULL);

      renderer = gtk_cell_renderer_text_new ();
      gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (iconView),
				  renderer, TRUE);
// //     g_object_set (renderer, "editable", TRUE, NULL);
//      g_signal_connect (renderer, "edited", G_CALLBACK (edited), iconView);
//      gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (iconView),
//				      renderer,
//				      "text", COL_TEXT,
//				      NULL);

//      gtk_container_add (GTK_CONTAINER (mywindow), icon_view);

//gtk_container_add (GTK_CONTAINER (vbox), icon_view);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)iconView,true,true,0);

	item=gtk_button_new_with_label("data");
	gtk_box_pack_start(GTK_BOX(vbox),item,true,false,2);
		g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(dodata),listStore);

 	//gtk_box_pack_start(mywindow,vbox,true,true,0);
  gtk_container_add(GTK_CONTAINER(mywindow),(GtkWidget*)vbox);   
    }

  if (!gtk_widget_get_visible (mywindow))
    gtk_widget_show_all (mywindow);
  else
    {
      gtk_widget_destroy (mywindow);
      mywindow = NULL;
    }

  return mywindow;
}
void doDnD(void)
{
	mywindow=do_iconview_edit(prefswin);
}

void doPrefs(void)
{
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	hbox2;
	GtkWidget*	item;
	GtkWidget*	label;

	prefswin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)prefswin,"Preferences");
	vbox=gtk_vbox_new(false,8);
	hbox=gtk_hbox_new(false,8);


//toolbar dnd
	item=gtk_button_new_with_label("Set Up Tool Bar");
	gtk_box_pack_start(GTK_BOX(vbox),item,true,false,2);
	gtk_widget_set_name(item,"Set Up Tool Bar");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(doDnD),NULL);

//appearence 1
	label=gtk_label_new("<b>General Appearance</b>");
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);

//indent
	item=gtk_check_button_new_with_label("Auto Indent Lines");
	gtk_widget_set_name(item,"indent");
	gtk_toggle_button_set_active((GtkToggleButton*)item,indent);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);
//linenumbers
	item=gtk_check_button_new_with_label("Show Line Numbers");
	gtk_widget_set_name(item,"show");
	gtk_toggle_button_set_active((GtkToggleButton*)item,lineNumbers);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);
//wraplines
	item=gtk_check_button_new_with_label("Wrap Lines");
	gtk_widget_set_name(item,"wrap");
	gtk_toggle_button_set_active((GtkToggleButton*)item,lineWrap);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//highlite
	item=gtk_check_button_new_with_label("Highlight Current Line");
	gtk_widget_set_name(item,"high");
	gtk_toggle_button_set_active((GtkToggleButton*)item,highLight);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//end appearance 1
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

//apperance 2
	hbox=gtk_hbox_new(false,8);
//show bookmark bar
	item=gtk_check_button_new_with_label("Show BM Bar");
	gtk_widget_set_name(item,"showbmbar");
	gtk_toggle_button_set_active((GtkToggleButton*)item,showBMBar);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//single instance
	item=gtk_check_button_new_with_label("Use Single Instance");
	gtk_widget_set_name(item,"single");
	gtk_toggle_button_set_active((GtkToggleButton*)item,singleUse);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//auto save session
	item=gtk_check_button_new_with_label("Auto Save/Restore Session");
	gtk_widget_set_name(item,"save");
	gtk_toggle_button_set_active((GtkToggleButton*)item,onExitSaveSession);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//auto restore bookmarks
	restoreBMs=gtk_check_button_new_with_label("Restore Session Bookmarks");
	gtk_widget_set_name(restoreBMs,"marks");
	gtk_toggle_button_set_active((GtkToggleButton*)restoreBMs,restoreBookmarks);
	gtk_box_pack_start(GTK_BOX(hbox),restoreBMs,true,true,0);
	g_signal_connect(G_OBJECT(restoreBMs),"toggled",G_CALLBACK(setPrefs),(void*)restoreBMs);
	gtk_widget_set_sensitive(restoreBMs,onExitSaveSession);
//end apperance 2
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

//text appearence
	label=gtk_label_new("<b>Text Style</b>");
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);
	hbox=gtk_hbox_new(false,8);

//tabwidth
	GtkObject*	adj=gtk_adjustment_new(tmpTabWidth,1,64,1,1,0);
	hbox2=gtk_hbox_new(false,0);
	item=gtk_spin_button_new((GtkAdjustment*)adj,1,0);
	gtk_widget_set_name(item,"tabs");
	gtk_box_pack_start(GTK_BOX(hbox2),gtk_label_new("Tab width: "),true,true,0);
	gtk_container_add(GTK_CONTAINER(hbox2),item);
	gtk_box_pack_start(GTK_BOX(hbox),hbox2,true,true,0);
	g_signal_connect(G_OBJECT(item),"value-changed",G_CALLBACK(setPrefs),(void*)item);

//style
	int cnt=0;
	int foundname=0;
	const gchar * const * ids=gtk_source_style_scheme_manager_get_scheme_ids(schemeManager);

	hbox2=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox2),gtk_label_new("Theme: "),true,true,0);

	item=gtk_combo_box_text_new();
	gtk_widget_set_name(item,"style");
	
	while(ids[cnt]!=NULL)
	{
		gtk_combo_box_text_append_text((GtkComboBoxText*)item,ids[cnt]);
		if(strcmp(ids[cnt],styleName)==0)
			foundname=cnt;
		cnt++;
	}
	gtk_combo_box_set_active((GtkComboBox*)item,foundname);
	gtk_box_pack_start(GTK_BOX(hbox2),item,true,true,0);

	gtk_box_pack_start(GTK_BOX(hbox),hbox2,true,true,0);
	g_signal_connect(G_OBJECT(item),"changed",G_CALLBACK(setPrefs),(void*)item);

//font button
	hbox2=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox2),gtk_label_new("Font: "),true,true,0);
	fontButton=gtk_font_button_new_with_font(fontAndSize);
	gtk_box_pack_start(GTK_BOX(hbox2),fontButton,true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),hbox2,true,true,0);
	gtk_widget_set_name(fontButton,"fontbutton");

//bm highlight colour
	bmHighlightBox=gtk_entry_new();
	gtk_widget_set_size_request((GtkWidget*)bmHighlightBox,72,-1);
	hbox2=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox2),gtk_label_new("BM Highlight Colour: "),true,true,0);
	gtk_container_add(GTK_CONTAINER(hbox2),bmHighlightBox);
	gtk_box_pack_start(GTK_BOX(hbox),hbox2,false,false,0);
	gtk_entry_set_text((GtkEntry*)bmHighlightBox,highlightColour);
	gtk_widget_show_all(hbox2);

//end style
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

//admin
	label=gtk_label_new("<b>Administration</b>");
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);
	hbox=gtk_hbox_new(false,8);
//function search depth
	GtkObject*	adjdepth=gtk_adjustment_new(tmpDepth,1,64,1,1,0);
	hbox2=gtk_hbox_new(false,0);
	item=gtk_spin_button_new((GtkAdjustment*)adjdepth,1,0);
	gtk_widget_set_name(item,"depth");
	gtk_box_pack_start(GTK_BOX(hbox2),gtk_label_new("Tag File Search Depth: "),true,true,0);
	gtk_container_add(GTK_CONTAINER(hbox2),item);
	gtk_box_pack_start(GTK_BOX(hbox),hbox2,false,false,0);
	g_signal_connect(G_OBJECT(item),"value-changed",G_CALLBACK(setPrefs),(void*)item);

//terminalcommand
	terminalBox=gtk_entry_new();
	hbox2=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox2),gtk_label_new("Terminal Command: "),false,false,0);
	gtk_container_add(GTK_CONTAINER(hbox2),terminalBox);
	gtk_box_pack_start(GTK_BOX(hbox),hbox2,true,true,0);
	gtk_entry_set_text((GtkEntry*)terminalBox,terminalCommand);
	gtk_widget_show_all(hbox2);
//end admin
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

//nag
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

	label=gtk_label_new("<b>Be Kind To Poor Programmers</b>");
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(vbox),label,true,true,0);
	hbox=gtk_hbox_new(true,0);

	item=gtk_check_button_new_with_label("I have donated");
	gtk_widget_set_name(item,"ihavedonated");
	gtk_toggle_button_set_active((GtkToggleButton*)item,nagScreen);

	gtk_box_pack_start(GTK_BOX(hbox),gtk_alignment_new(0,0,1,0),true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_alignment_new(0,0,1,0),true,true,0);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("I have really donated some some money to the author.\nMy conscience is clear and my Karma is squeaky clean :)"),false,false,0);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(setPrefs),(void*)item);

//buttons
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

	hbox=gtk_hbox_new(true,4);
	item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"apply");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);	

	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	gtk_widget_set_name(item,"cancel");
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)item);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);

//show it
	gtk_container_add(GTK_CONTAINER(prefswin),(GtkWidget*)vbox);
	gtk_widget_show_all(prefswin);
}

GtkWidget*	recent;

void addRecentToMenu(GtkRecentChooser* chooser,GtkWidget* menu)
{
	GList*		itemlist=NULL;
	GList*		l=NULL;
	GtkWidget*	menuitem;
	char*		uri=NULL;
	char*		filename=NULL;
	int			i=0;

	itemlist=gtk_recent_chooser_get_items(chooser);

	for (l = itemlist; l != NULL; l = l->next)
		{
			const gchar *menuname;
			GtkRecentInfo *info = (GtkRecentInfo*)l->data;
			if (i >= MAXRECENT)
				break;
			i++;

			menuname=gtk_recent_info_get_display_name(info);
			uri=(char*)gtk_recent_info_get_uri(info);
			if (uri!=NULL)
				{
					filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
					menuitem=gtk_image_menu_item_new_with_label(menuname);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(recentFileMenu),(void*)filename);
					g_free (uri);
				}
		}
}
//	toolBarLayout=strdup("NORSsXCPsUrsFGs");
void setUpToolBar(void)
{
	GtkToolItem*	toolbutton;

	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						newButton=gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
						gtk_toolbar_insert((GtkToolbar*)toolBar,newButton,-1);
						gtk_signal_connect(GTK_OBJECT(newButton),"clicked",G_CALLBACK(newFile),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)newButton,"New File");
						break;
					case 'O':
//open+recent
						openButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_OPEN);
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(openButton),recent);
						gtk_toolbar_insert((GtkToolbar*)toolBar,openButton,-1);
						gtk_signal_connect(GTK_OBJECT(openButton),"clicked",G_CALLBACK(doOpenFile),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)openButton,"Open File");
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(openButton),"Open Recent File");
						break;
					case 'S':
//save
						saveButton=gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
						gtk_toolbar_insert((GtkToolbar*)toolBar,saveButton,-1);
						gtk_signal_connect(GTK_OBJECT(saveButton),"clicked",G_CALLBACK(saveFile),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)saveButton,"Save File");

						break;
					case 's':
						gtk_toolbar_insert((GtkToolbar*)toolBar,gtk_separator_tool_item_new(),-1);
						break;
					case 'X':
//cut
						cutButton=gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
						gtk_toolbar_insert((GtkToolbar*)toolBar,cutButton,-1);
						gtk_signal_connect(GTK_OBJECT(cutButton),"clicked",G_CALLBACK(cutToClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)cutButton,"Cut");
						break;
					case 'C':
//copy
						copyButton=gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
						gtk_toolbar_insert((GtkToolbar*)toolBar,copyButton,-1);
						gtk_signal_connect(GTK_OBJECT(copyButton),"clicked",G_CALLBACK(copyToClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)copyButton,"Copy");
						break;
					case 'P':
//paste
						pasteButton=gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);
						gtk_toolbar_insert((GtkToolbar*)toolBar,pasteButton,-1);
						gtk_signal_connect(GTK_OBJECT(pasteButton),"clicked",G_CALLBACK(pasteFromClip),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)pasteButton,"Paste");
						break;
					case 'U':
//undo
						undoButton=gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
						gtk_toolbar_insert((GtkToolbar*)toolBar,undoButton,-1);
						gtk_signal_connect(GTK_OBJECT(undoButton),"clicked",G_CALLBACK(undo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)undoButton,"Undo");
						break;
					case 'R':
//redo
						redoButton=gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
						gtk_toolbar_insert((GtkToolbar*)toolBar,redoButton,-1);
						gtk_signal_connect(GTK_OBJECT(redoButton),"clicked",G_CALLBACK(redo),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)redoButton,"Redo");
						break;
					case 'F':
//find
						findButton=gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
						gtk_toolbar_insert((GtkToolbar*)toolBar,findButton,-1);
						gtk_signal_connect(GTK_OBJECT(findButton),"clicked",G_CALLBACK(find),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)findButton,"Find/Replace");
						break;
					case 'G':
//navigation
						gotoDefButton=gtk_tool_button_new_from_stock(GTK_STOCK_DIALOG_QUESTION);
						gtk_toolbar_insert((GtkToolbar*)toolBar,gotoDefButton,-1);
						gtk_signal_connect(GTK_OBJECT(gotoDefButton),"clicked",G_CALLBACK(goToDefinition),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoDefButton,"Go To Definition");
						break;
					case '9':
						lineNumberWidget=gtk_entry_new();
						gotoLineButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)gotoLineButton,lineNumberWidget);
						gtk_toolbar_insert((GtkToolbar*)toolBar,gotoLineButton,-1);
						g_signal_connect_after(G_OBJECT(lineNumberWidget),"key-release-event",G_CALLBACK(jumpToLineFromBar),NULL);
						gtk_widget_set_size_request((GtkWidget*)gotoLineButton,48,-1);
						gtk_widget_set_tooltip_text((GtkWidget*)gotoLineButton,"Go To Line");
						break;
					case 'A':
//find in gtkdoc
						findApiWidget=gtk_entry_new();
						findApiButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findApiButton,findApiWidget);
						gtk_toolbar_insert((GtkToolbar*)toolBar,findApiButton,-1);
						g_signal_connect_after(G_OBJECT(findApiWidget),"activate",G_CALLBACK(docSearchFromBar),(void*)findApiWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findApiButton,"Find API In Gtk Docs");
						break;
					case 'D':
//find in function def
						findDefWidget=gtk_entry_new();
						findFuncDefButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)findFuncDefButton,findDefWidget);
						gtk_toolbar_insert((GtkToolbar*)toolBar,findFuncDefButton,-1);
						g_signal_connect_after(G_OBJECT(findDefWidget),"activate",G_CALLBACK(defSearchFromBar),(void*)findDefWidget);
						gtk_widget_set_tooltip_text((GtkWidget*)findFuncDefButton,"Search For Define");
						break;
					case 'L':
//livesearch
						liveSearchWidget=gtk_entry_new();
						liveSearchButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)liveSearchButton,liveSearchWidget);
						gtk_toolbar_insert((GtkToolbar*)toolBar,liveSearchButton,-1);
						g_signal_connect_after(G_OBJECT(liveSearchWidget),"key-release-event",G_CALLBACK(doLiveSearch),NULL);
						gtk_widget_set_tooltip_text((GtkWidget*)liveSearchButton,"Live Search");
						break;
					case 'E':
//expander
						toolbutton=gtk_separator_tool_item_new();
						gtk_tool_item_set_expand(toolbutton,true);
						gtk_separator_tool_item_set_draw((GtkSeparatorToolItem*)toolbutton,false);
						gtk_toolbar_insert((GtkToolbar*)toolBar,toolbutton,-1);
						break;
				}
		}
}

void buildMainGui(void)
{
	GtkWidget*					vbox;
	GtkWidget*					menuitem;
	GtkWidget*					menu;

	GtkToolItem*				toolbutton;
	GtkAccelGroup*				accgroup;
	GtkWidget*					image;

	GtkRecentFilter*			filter;
	GtkWidget*					menurecent;

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)window,windowWidth,windowHeight);
	if(windowX!=-1 && windowY!=-1)
		gtk_window_move((GtkWindow *)window,windowX,windowY);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(doShutdown),NULL);
	accgroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)window,accgroup);

	notebook=(GtkNotebook*)gtk_notebook_new();
	gtk_notebook_set_scrollable(notebook,true);
	gtk_signal_connect(GTK_OBJECT(notebook),"switch-page",G_CALLBACK(switchPage),NULL);
	gtk_signal_connect(GTK_OBJECT(notebook),"page-reordered",G_CALLBACK(switchPage),NULL);

	vbox=gtk_vbox_new(FALSE,0);
	menubar=gtk_menu_bar_new();
	toolBar=gtk_toolbar_new();

	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)menubar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)toolBar,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)notebook,true,true,0);

//dnd
	gtk_drag_dest_set(vbox,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(vbox);
	g_signal_connect(G_OBJECT(vbox),"drag_data_received",G_CALLBACK(dropUri),NULL);

//recent
	recent=gtk_recent_chooser_menu_new();
	gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recent),false);
	gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent),GTK_RECENT_SORT_MRU);
	gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent),MAXRECENT);

	filter=gtk_recent_filter_new();
	gtk_recent_filter_add_application(filter,"kkedit");
	gtk_recent_chooser_set_filter(GTK_RECENT_CHOOSER(recent),filter);
	g_signal_connect(recent,"item_activated",G_CALLBACK(recentFileMenu),NULL);

	setUpToolBar();

//menus
//file menu
	menufile=gtk_menu_item_new_with_label("File");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufile),menu);
//new
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'N',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//open
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doOpenFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'O',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//open as hexdump
	menuitem=gtk_image_menu_item_new_with_label("Open As Hexdump");
	image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(openAsHexDump),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//extras
	menuitem=gtk_image_menu_item_new_with_label("New Admin Editor");
	image=gtk_image_new_from_file(DATADIR"/pixmaps/ROOTKKEdit.png");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newEditor),(void*)1);

	menuitem=gtk_image_menu_item_new_with_label("New Editor");
	image=gtk_image_new_from_file(DATADIR"/pixmaps/MenuKKEdit.png");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newEditor),(void*)2);

#ifdef _MANPAGEEDITOR_
	image=gtk_image_new_from_file(DATADIR"/pixmaps/ManPageEditor.png");
	menuitem=gtk_image_menu_item_new_with_label("Manpage Editor");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newEditor),(void*)3);
#endif

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//recent menu
	menuitem=gtk_image_menu_item_new_with_label("Recent Files");
	menurecent=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),menurecent);
	addRecentToMenu((GtkRecentChooser*)recent,menurecent);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save
	saveMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),saveMenu);
	gtk_signal_connect(GTK_OBJECT(saveMenu),"activate",G_CALLBACK(saveFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)saveMenu,"activate",accgroup,'S',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//savas
	saveAsMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),saveAsMenu);
	gtk_signal_connect(GTK_OBJECT(saveAsMenu),"activate",G_CALLBACK(saveFile),(void*)1);
	gtk_widget_add_accelerator((GtkWidget *)saveAsMenu,"activate",accgroup,'S',(GdkModifierType)(GDK_SHIFT_MASK|GDK_CONTROL_MASK),GTK_ACCEL_VISIBLE);
	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,false);
//save all
	menusaveall=gtk_image_menu_item_new_with_label("Save All");
	image=gtk_image_new_from_stock(GTK_STOCK_SAVE,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menusaveall,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menusaveall);
	gtk_signal_connect(GTK_OBJECT(menusaveall),"activate",G_CALLBACK(doSaveAll),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//save session
	image=gtk_image_new_from_stock(GTK_STOCK_SAVE,GTK_ICON_SIZE_MENU);
	menuitem=gtk_image_menu_item_new_with_label("Save Session");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(saveSession),(void*)false);
//restore session
	image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	menuitem=gtk_image_menu_item_new_with_label("Restore Session");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(restoreSession),(void*)false);
//restore session and bookmarks
	image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	menuitem=gtk_image_menu_item_new_with_label("Restore Session With Bookmarks");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(restoreSession),(void*)true);

//printfile
	menuprint=gtk_image_menu_item_new_from_stock(GTK_STOCK_PRINT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuprint);
	gtk_signal_connect(GTK_OBJECT(menuprint),"activate",G_CALLBACK(printFile),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//close
	menuclose=gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuclose);
	gtk_signal_connect(GTK_OBJECT(menuclose),"activate",G_CALLBACK(closeTab),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuclose,"activate",accgroup,'W',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//close-all
	image=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
	menucloseall=gtk_image_menu_item_new_with_label("Close All Tabs");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menucloseall,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menucloseall);
	gtk_signal_connect(GTK_OBJECT(menucloseall),"activate",G_CALLBACK(closeAllTabs),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//reload file
	menurevert=gtk_image_menu_item_new_from_stock(GTK_STOCK_REVERT_TO_SAVED,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menurevert);
	gtk_signal_connect(GTK_OBJECT(menurevert),"activate",G_CALLBACK(reloadFile),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doShutdown),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'Q',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//edit menu
	menuedit=gtk_menu_item_new_with_label("Edit");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuedit),menu);
//undo
	undoMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_UNDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),undoMenu);
	gtk_signal_connect(GTK_OBJECT(undoMenu),"activate",G_CALLBACK(undo),NULL);
	gtk_widget_add_accelerator((GtkWidget *)undoMenu,"activate",accgroup,'Z',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//redo
	redoMenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_REDO,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),redoMenu);
	gtk_signal_connect(GTK_OBJECT(redoMenu),"activate",G_CALLBACK(redo),NULL);
	gtk_widget_add_accelerator((GtkWidget *)redoMenu,"activate",accgroup,'Z',(GdkModifierType)(GDK_SHIFT_MASK|GDK_CONTROL_MASK),GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//cut
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(cutToClip),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'X',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//copy
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(copyToClip),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'C',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//paste
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(pasteFromClip),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'V',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//find
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(find),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'F',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//prefs
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doPrefs),NULL);

//navigation menu
	menunav=gtk_menu_item_new_with_label("Navigation");
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menunav),menu);
//goto define
	image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_MENU);
	menuitem=gtk_image_menu_item_new_with_label("Go To Definition");
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(goToDefinition),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'D',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//open include
	menuitem=gtk_image_menu_item_new_with_label("Open Include File");
	image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(findFile),NULL);
	gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,'I',GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
//goto line number
	menuitem=gtk_image_menu_item_new_with_label("Go To Line");
	image=gtk_image_new_from_stock(GTK_STOCK_GO_DOWN,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(jumpToLine),NULL);
//find define
	menuitem=gtk_image_menu_item_new_with_label("Search For Define");
	image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(functionSearch),NULL);

//find gtkdoc
	menuitem=gtk_image_menu_item_new_with_label("Search In Gtk-Docs");
	image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(showDocView),NULL);

//function menu
	menufunc=gtk_menu_item_new_with_label("Functions");

//newbookmarks
	menuBookMark=gtk_menu_item_new_with_label("Bookmarks");
	rebuildBookMarkMenu();

//external tools
	menutools=gtk_menu_item_new_with_label("Tools");
	buildTools();

//help
	menuhelp=gtk_menu_item_new_with_label("Help");
	menu=gtk_menu_new();
//about
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuhelp),menu);
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doAbout),NULL);
//help
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,NULL);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(openHelp),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menufile);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menuedit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menunav);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menufunc);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menuBookMark);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menutools);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),menuhelp);

	gtk_container_add(GTK_CONTAINER(window),(GtkWidget*)vbox);
	gtk_widget_set_sensitive((GtkWidget*)saveButton,false);
	gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);
}

void buildFindReplace(void)
{
	GtkWidget*	content_area;
	GtkWidget*	replace;
	GtkWidget*	goback;
	GtkWidget*	image;
	GtkWidget*	label;
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	item;

	findReplaceDialog=gtk_dialog_new_with_buttons("Find/Replace",(GtkWindow*)window, GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_GO_FORWARD,FINDNEXT,GTK_STOCK_GO_BACK,FINDPREV,"Replace",REPLACE,NULL);
	gtk_dialog_set_default_response((GtkDialog*)findReplaceDialog,GTK_RESPONSE_OK);
	g_signal_connect(G_OBJECT(findReplaceDialog),"response",G_CALLBACK(doFindReplace),NULL);
	content_area=gtk_dialog_get_content_area(GTK_DIALOG(findReplaceDialog));

	vbox=gtk_vbox_new(true,0);
	hbox=gtk_hbox_new(false,0);

	label=gtk_label_new("Find");
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	findBox=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)findBox,"");
	gtk_entry_set_activates_default((GtkEntry*)findBox,true);
	gtk_container_add(GTK_CONTAINER(content_area),findBox);

	label=gtk_label_new("Replace With");
	gtk_container_add(GTK_CONTAINER(content_area),label);
	gtk_widget_show(label);

	replaceBox=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)replaceBox,"");
	gtk_entry_set_activates_default((GtkEntry*)replaceBox,true);
	gtk_container_add(GTK_CONTAINER(content_area),replaceBox);

	item=gtk_check_button_new_with_label("Case insensitive");
	gtk_toggle_button_set_active((GtkToggleButton*)item,insensitiveSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)1);

	item=gtk_check_button_new_with_label("Use Regex");
	gtk_toggle_button_set_active((GtkToggleButton*)item,useRegex);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)5);

	item=gtk_check_button_new_with_label("Wrap");
	gtk_toggle_button_set_active((GtkToggleButton*)item,wrapSearch);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)2);


	item=gtk_check_button_new_with_label("All Files");
	gtk_toggle_button_set_active((GtkToggleButton*)item,findInAllFiles);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)4);

	item=gtk_check_button_new_with_label("Replace All");
	gtk_toggle_button_set_active((GtkToggleButton*)item,replaceAll);
	gtk_box_pack_start(GTK_BOX(hbox),item,true,true,0);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item),"toggled",G_CALLBACK(doSearchPrefs),(void*)3);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(content_area),vbox,true,true,0);

	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
	image=gtk_image_new_from_stock(GTK_STOCK_FIND_AND_REPLACE,GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image((GtkButton*)replace,image);

	goback=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,FINDPREV);
	gtk_widget_set_sensitive(goback,!useRegex);
	
	replace=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
	if(replaceAll==true)
		gtk_button_set_label((GtkButton*)replace,"Replace All");

	gtk_widget_show(findBox);
	gtk_widget_show(replaceBox);
	gtk_widget_show(vbox);
	gtk_widget_show(hbox);

	gtk_signal_connect_object(GTK_OBJECT(findReplaceDialog),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(findReplaceDialog));
	gtk_signal_connect(GTK_OBJECT(findReplaceDialog),"delete_event",GTK_SIGNAL_FUNC(gtk_true),NULL);
}

#ifdef _ASPELL_
void buildWordCheck(int documentCheck)
{
	GtkWidget*	vbox;
	GtkWidget*	button;
	GtkWidget*	hbox;
	GtkWidget*	label;
	GtkWidget*	image;
	char*		labeltext[512];
	int			docflag=documentCheck;

	spellCheckWord=gtk_dialog_new();
	gtk_window_set_title((GtkWindow*)spellCheckWord,"Spell check word");
	vbox=gtk_vbox_new(true,8);

	hbox=gtk_hbox_new(true,8);

	sprintf((char*)&labeltext,"Change <i><b>%s</b></i> to: ",badWord);
	label=gtk_label_new((char*)&labeltext);
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(hbox),label,true,true,0);

	wordListDropbox=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(hbox),wordListDropbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	hbox=gtk_hbox_new(true,8);
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doChangeWord),(gpointer)(long)docflag);

	button=gtk_button_new_with_label("Ignore");
	image=gtk_image_new_from_stock(GTK_STOCK_ADD,GTK_ICON_SIZE_MENU);
	gtk_button_set_image((GtkButton*)button,image);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)1);

	button=gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)2);

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doCancelCheck),NULL);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	GtkWidget* content=gtk_dialog_get_content_area((GtkDialog *)spellCheckWord);
	gtk_container_add(GTK_CONTAINER(content),(GtkWidget*)vbox);

	gtk_signal_connect_object(GTK_OBJECT(spellCheckWord),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(spellCheckWord));
	gtk_signal_connect(GTK_OBJECT(spellCheckWord),"delete_event",GTK_SIGNAL_FUNC(gtk_true),NULL);
}
#endif

int showFunctionEntry(void)
{
	GtkWidget*	dialog;
	gint		result;
	GtkWidget*	content_area;
	GtkWidget*	entrybox;

	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,"Enter Function Name");

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Find Function");

	content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));	
	entrybox=gtk_entry_new();
	gtk_entry_set_activates_default((GtkEntry*)entrybox,true);
	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_YES);
	gtk_container_add(GTK_CONTAINER(content_area),entrybox);
	gtk_widget_show_all(content_area);
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	if(functionSearchText!=NULL)
		g_free(functionSearchText);
	functionSearchText=strdup(gtk_entry_get_text((GtkEntry*)entrybox));
	gtk_widget_destroy(dialog);

	return(result);
}

#ifdef BUILDDOCVIEWER
void buildGtkDocViewer(void)
{
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;
	GtkWidget*	scrolledWindow;
	GtkWidget*	entry;
	GtkWidget*	findbutton;

	docView=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(docView),800,600);
	gtk_window_set_title((GtkWindow*)docView,"Search Gtk Docs");

	vbox=gtk_vbox_new(false,0);
	hbox=gtk_hbox_new(true,0);

	webView=WEBKIT_WEB_VIEW(webkit_web_view_new());

	scrolledWindow=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow),GTK_WIDGET(webView));

	gtk_container_add(GTK_CONTAINER(vbox),scrolledWindow);

	button=gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoBack),(void*)webView);	
 
	button=gtk_button_new_from_stock(GTK_STOCK_HOME);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoHome),(void*)webView);	
 
	entry=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),entry,false,true,0);
	g_signal_connect_after(G_OBJECT(entry),"activate",G_CALLBACK(docSearchFromBar),(void*)entry);
	findbutton=gtk_button_new_from_stock(GTK_STOCK_FIND);
	gtk_box_pack_start(GTK_BOX(hbox),findbutton,false,false,0);
	g_signal_connect(G_OBJECT(findbutton),"clicked",G_CALLBACK(docSearchFromBar),(void*)entry);	

	button=gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(webKitGoForward),(void*)webView);	

	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,4);

	gtk_container_add(GTK_CONTAINER(docView),vbox);
	gtk_widget_grab_focus(GTK_WIDGET(webView));

	gtk_signal_connect_object(GTK_OBJECT(docView),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(docView));
	gtk_signal_connect(GTK_OBJECT(docView),"delete_event",GTK_SIGNAL_FUNC(gtk_true),NULL);
}
#endif




