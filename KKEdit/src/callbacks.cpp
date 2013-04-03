/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include <gtk/gtk.h>

#include "globals.h"
#include "files.h"

void doOpenFile(GtkWidget* widget,gpointer data)
{
	GtkWidget *dialog;
	char *filename;

	dialog=gtk_file_chooser_dialog_new("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			openFile(filename);
			g_free (filename);
	}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}

int show_question(void)
{
	GtkWidget	*dialog;
	gint			result;

	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,"Save file before closing?");

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_SAVE,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Warning unsaved data!");

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	return(result);
}

void closeTab(GtkWidget* widget,gpointer data)
{
	long			thispage;
	int			result;
	pageStruct* page;

	if(data==NULL)
		thispage=gtk_notebook_get_current_page(notebook);
	else
		thispage=gtk_notebook_page_num(notebook,(GtkWidget *)data);

	page=getPageStructPtr(thispage);

	if(gtk_text_buffer_get_modified((GtkTextBuffer*)page->buffer))
		{
			result=show_question();
			switch(result)
				{
					case GTK_RESPONSE_YES:
						saveFile(NULL,NULL);
						break;
					case GTK_RESPONSE_NO:
						break;
					default:
						return;
						break;
				}
		}
		
	g_free(page->filePath);
	g_free(page);
	currentPage--;
	gtk_notebook_remove_page(notebook,thispage);
}

void getTagList(char* filepath,void* ptr)
{
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint   retval=0;

	char*	command;
	asprintf(&command,"ctags -x %s",filepath);
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			asprintf((char**)ptr,"%s",stdout);
			g_free(stdout);
			g_free(stderr);
		}
}

void gotoLine(GtkWidget* widget,gpointer data)
{
	int	line=(long)data;
	GtkTextIter iter;
	pageStruct*	page=getPageStructPtr(-1);

	gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,line-1,0);
	gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);
	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
}

void setSensitive(void)
{
	pageStruct*		page=getPageStructPtr(currentTabNumber);
	const gchar*	text=gtk_label_get_text((GtkLabel*)page->tabName);
	char*				newlabel;
	int				offset=0;

//toolbar
	gtk_widget_set_sensitive((GtkWidget*)undoButton,gtk_source_buffer_can_undo(page->buffer));
	gtk_widget_set_sensitive((GtkWidget*)redoButton,gtk_source_buffer_can_redo(page->buffer));
	gtk_widget_set_sensitive((GtkWidget*)saveButton,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));

//menu
	gtk_widget_set_sensitive((GtkWidget*)undoMenu,gtk_source_buffer_can_undo(page->buffer));
	gtk_widget_set_sensitive((GtkWidget*)redoMenu,gtk_source_buffer_can_redo(page->buffer));
	gtk_widget_set_sensitive((GtkWidget*)saveMenu,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));

//tab
	gtk_widget_set_sensitive((GtkWidget*)saveButton,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
	if(text[0]=='*')
		offset=1;

	if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer))==true)
		{
			asprintf(&newlabel,"*%s",&text[offset]);
		}
	else
		asprintf(&newlabel,"%s",&text[offset]);


	gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)newlabel);
	g_free(newlabel);
}

void switchPage(GtkNotebook *notebook,gpointer arg1,guint thispage,gpointer user_data)
{
	pageStruct*	page=getPageStructPtr(thispage);
//	pageStruct*	page=(pageStruct*)g_list_nth_data(pageList,thispage);
	char*			functions;
	GtkWidget*	menu;
	GtkWidget*	menuitem;
	int			linenum;
	char			tmpstr[1024];
	char*			lineptr;
long result;

	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)menunav);
	if (submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)menunav,NULL);

	currentTabNumber=thispage;

//	if(page->rebuildMenu==true)
//		{
//			printf("rebuilding menu for tab %i\n",thispage);

			page->rebuildMenu=false;
			getTagList(page->filePath,&functions);
			lineptr=functions;

			page->navSubMenu=(GtkMenuItem*)gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menunav),(GtkWidget*)page->navSubMenu);

			while (lineptr!=NULL)
				{
					sscanf (lineptr,"%*s %*s %i %*s %[\]a-zA-Z0-9 ()_-,.*;\[\"]s",&linenum,tmpstr);
					lineptr=strchr(lineptr,'\n');
					if (lineptr!=NULL)
						lineptr++;

					menuitem=gtk_image_menu_item_new_with_label(tmpstr);
					gtk_menu_shell_append(GTK_MENU_SHELL(page->navSubMenu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(gotoLine),(void*)linenum);
				}
			gtk_widget_show_all(window);
			g_free(functions);
//		}
//	else
//		{
//				printf("just switch neu\n");)
//			menu=gtk_menu_new();
//			gtk_menu_item_set_submenu((GtkMenuItem*)menunav,NULL);
//			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menunav),(GtkWidget*)page->navSubMenu);
//			gtk_menu_shell_append(GTK_MENU_SHELL(menu),(GtkWidget*)page->navSubMenu);
//			gtk_widget_show_all(window);
//		}
	setSensitive();
}

void copyToClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_copy_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
}

void cutToClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_cut_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),true);
	setSensitive();
}

void pasteFromClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_paste_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),NULL,true);
	setSensitive();
}

void undo(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_source_buffer_undo(page->buffer);
	setSensitive();
}

void redo(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_source_buffer_redo(page->buffer);
	setSensitive();
}

void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	gchar**	array=gtk_selection_data_get_uris(selection_data);
	int		cnt=g_strv_length(array);
	char*		filename;

	for(int j=0;j<cnt;j++)
		{
			filename=g_filename_from_uri(array[j],NULL,NULL);
			openFile(filename);
		}

	g_strfreev(array);
}

void find(GtkWidget* widget,gpointer data)
{
	gtk_widget_show(findReplaceDialog);
	gtk_dialog_run((GtkDialog *)findReplaceDialog);
}

#if false
gboolean search_dialog (GtkWidget *widget,gboolean replace,char **what_p,char **replacement_p,GtkTextSearchFlags *flags_p)
{
GtkWidget *dialog;
GtkEntry *entry1, *entry2;
GtkToggleButton *case_sensitive;

dialog = gtk_dialog_new_with_buttons (replace ? "Replace" : "Find",
GTK_WINDOW (gtk_widget_get_toplevel (widget)),
GTK_DIALOG_MODAL,
GTK_STOCK_CANCEL,
GTK_RESPONSE_CANCEL,
GTK_STOCK_OK,
GTK_RESPONSE_OK,
NULL);
gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

entry1 = g_object_new (GTK_TYPE_ENTRY,
"visible", TRUE,
"text", search_data.what ? search_data.what : "",
"activates-default", TRUE,
NULL);
gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))),
GTK_WIDGET (entry1), TRUE, TRUE, 0);
entry2 = g_object_new (GTK_TYPE_ENTRY,
"visible", replace,
"text", search_data.replacement ? search_data.replacement : "",
"activates-default", TRUE,
NULL);
gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))),
GTK_WIDGET (entry2), TRUE, TRUE, 0);

case_sensitive = g_object_new (GTK_TYPE_CHECK_BUTTON,
"visible", TRUE,
"label", "Case sensitive",
"active", !(search_data.flags & GTK_TEXT_SEARCH_CASE_INSENSITIVE),
NULL);
gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))),
GTK_WIDGET (case_sensitive), FALSE, FALSE, 0);

while (TRUE)
{
if (gtk_dialog_run (GTK_DIALOG (dialog)) != GTK_RESPONSE_OK)
{
gtk_widget_destroy (dialog);
return FALSE;
}

if (*gtk_entry_get_text (entry1))
break;
}

g_free (search_data.what);
*what_p = search_data.what = g_strdup (gtk_entry_get_text (entry1));
g_free (search_data.replacement);
*replacement_p = search_data.replacement = g_strdup (gtk_entry_get_text (entry2));
*flags_p = search_data.flags = gtk_toggle_button_get_active (case_sensitive) ?
0 : GTK_TEXT_SEARCH_CASE_INSENSITIVE;

gtk_widget_destroy (dialog);
return TRUE;
}
#endif




#if false
void do_search_replace(GtkTextView *view,gboolean replace)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (view);
	GtkTextIter iter;
	char *what, *replacement;
GtkTextSearchFlags flags;

if (!search_dialog (GTK_WIDGET (view), replace, &what, &replacement, &flags))
return;

if (replace)
{
gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

while (TRUE)
{
GtkTextIter match_start, match_end;

if (!gtk_text_iter_forward_search (&iter, what, flags,
&match_start,
&match_end,
NULL))
{
break;
}

gtk_text_buffer_delete (buffer, &match_start, &match_end);
gtk_text_buffer_insert (buffer, &match_start, replacement, -1);
iter = match_start;
}
}
else
{
GtkTextIter match_start, match_end;

gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_insert (buffer));

if (gtk_text_iter_forward_search (&iter, what, flags, &match_start, &match_end, NULL))
{
gtk_text_buffer_select_range (buffer, &match_start, &match_end);
}
else
{
GtkTextIter insert = iter;
gtk_text_buffer_get_start_iter (buffer, &iter);
if (gtk_text_iter_forward_search (&iter, what, flags, &match_start, &match_end, &insert))
gtk_text_buffer_select_range (buffer, &match_start, &match_end);
}
}
}

void find_cb (GtkAction *action,gpointer user_data)
{
 do_search_replace (user_data, FALSE);
}

void replace_cb (GtkAction *action,gpointer user_data)
{
do_search_replace (user_data, TRUE);
}
#endif







