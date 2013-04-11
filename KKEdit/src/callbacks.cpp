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
	GtkWidget*	dialog;
	char*		filename;

	dialog=gtk_file_chooser_dialog_new("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			openFile(filename,0);
			g_free (filename);
		}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}

int show_question(char* filename)
{
	GtkWidget*	dialog;
	gint		result;
	char*		message;

	asprintf(&message,"Save file %s before closing?",filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_SAVE,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Warning unsaved data!");

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	g_free(message);
	return(result);
}

void closeTab(GtkWidget* widget,gpointer data)
{
	long		thispage;
	int			result;
	pageStruct*	page;

	if(data==NULL)
		thispage=gtk_notebook_get_current_page(notebook);
	else
		thispage=gtk_notebook_page_num(notebook,(GtkWidget *)data);

	page=getPageStructPtr(thispage);

	if(gtk_text_buffer_get_modified((GtkTextBuffer*)page->buffer))
		{
			result=show_question(page->filePath);
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
	char*	command;
	gchar*	stdout=NULL;
	gchar*	stderr=NULL;
	gint	retval=0;

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
	int			line=(long)data;
	GtkTextIter	iter;
	pageStruct*	page=getPageStructPtr(-1);

	gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,line-1,0);
	gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);
	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
}

void setSensitive(void)
{
	pageStruct*		page=getPageStructPtr(currentTabNumber);
	const gchar*	text=gtk_label_get_text((GtkLabel*)page->tabName);
	char*			newlabel;
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
		asprintf(&newlabel,"*%s",&text[offset]);
	else
		asprintf(&newlabel,"%s",&text[offset]);


	gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)newlabel);
	g_free(newlabel);
	gtk_widget_show_all(page->tabName);
}

void switchPage(GtkNotebook *notebook,gpointer arg1,guint thispage,gpointer user_data)
{
	pageStruct*	page=getPageStructPtr(thispage);
	char*		functions;
	GtkWidget*	menuitem;
	int			linenum;
	char		tmpstr[1024];
	char*		lineptr;

	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)menufunc);
	if (submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)menufunc,NULL);

	currentTabNumber=thispage;

//	if(page->rebuildMenu==true)
//		{
//			printf("rebuilding menu for tab %i\n",thispage);

			page->rebuildMenu=false;
			getTagList(page->filePath,&functions);
			lineptr=functions;

			page->isFirst=true;

			page->navSubMenu=(GtkMenuItem*)gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufunc),(GtkWidget*)page->navSubMenu);

			while (lineptr!=NULL)
				{
					sscanf (lineptr,"%*s %*s %i %*s %[]a-zA-Z0-9 ()_-,.*#;[\"]s",&linenum,tmpstr);
					lineptr=strchr(lineptr,'\n');
					if (lineptr!=NULL)
						lineptr++;

					menuitem=gtk_image_menu_item_new_with_label(tmpstr);
					gtk_menu_shell_append(GTK_MENU_SHELL(page->navSubMenu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(gotoLine),(void*)(long)linenum);
				}
			gtk_window_set_title((GtkWindow*)window,page->fileName);
			gtk_widget_show_all(window);
			g_free(functions);
//		}
//	else
//		{
//				printf("just switch neu\n");)
//			menu=gtk_menu_new();
//			gtk_menu_item_set_submenu((GtkMenuItem*)menufunc,NULL);
//			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufunc),(GtkWidget*)page->navSubMenu);
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

	if(gtk_source_buffer_can_undo(page->buffer));
		{
			gtk_source_buffer_undo(page->buffer);
			page->isFirst=true;
			setSensitive();
		}
}

void redo(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);

	if(gtk_source_buffer_can_redo(page->buffer));
		{
			gtk_source_buffer_redo(page->buffer);
			page->isFirst=true;
			setSensitive();
		}
}

void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	gchar**	array=gtk_selection_data_get_uris(selection_data);
	int		cnt=g_strv_length(array);
	char*	filename;

	for(int j=0;j<cnt;j++)
		{
			filename=g_filename_from_uri(array[j],NULL,NULL);
			openFile(filename,0);
		}

	g_strfreev(array);
}

void find(GtkWidget* widget,gpointer data)
{
	gtk_widget_show(findReplaceDialog);
	gtk_dialog_run((GtkDialog *)findReplaceDialog);
}

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data)
{
	pageStruct* page=getPageStructPtr(-1);
	gchar*		selectedtext=NULL;

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	switch (response_id)
		{
//forward search
			case FINDNEXT:
				if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
				if(gtk_text_iter_forward_search(&page->match_end,gtk_entry_get_text((GtkEntry*)findBox),GTK_TEXT_SEARCH_TEXT_ONLY,&page->match_start,&page->match_end,NULL))
					{
						gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
						gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
						page->iter=page->match_end;
					}
				break;
//backward search
			case FINDPREV:
				if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
				if(gtk_text_iter_backward_search(&page->match_start,gtk_entry_get_text((GtkEntry*)findBox),GTK_TEXT_SEARCH_TEXT_ONLY,&page->match_start,&page->match_end,NULL))
					{
						gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
						gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
						page->iter=page->match_start;
					}
				break;
//replace and search
			case REPLACE:
				if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
					{
						selectedtext=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end,false);
						if(strcasecmp(selectedtext,gtk_entry_get_text((GtkEntry*)findBox))==0)
							{
								gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
								gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&page->match_start,gtk_entry_get_text((GtkEntry*)replaceBox),-1);
								gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
								if(gtk_text_iter_forward_search(&page->iter,gtk_entry_get_text((GtkEntry*)findBox),GTK_TEXT_SEARCH_TEXT_ONLY,&page->match_start,&page->match_end,NULL))
									{
										gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
										page->iter=page->match_end;
									}
							}
						if(selectedtext!=NULL)
							g_free(selectedtext);
					}
				break;

			default:
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
				page->isFirst=true;
				page->match_start=page->iter;
				page->match_end=page->iter;
				break;
		}
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}

void goToDefinition(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	int			numpages=gtk_notebook_get_n_pages(notebook);
	char*		functions;
	char		name[1024];
	int			line;
	char		file[4096];
	char*		lineptr;
	GtkTextIter	iter;

	char*		command;
	gchar*		stdout=NULL;
	gchar*		stderr=NULL;
	gint		retval=0;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			getTagList(page->filePath,&functions);
			lineptr=functions;
			while (lineptr!=NULL)
				{
					sscanf (lineptr,"%s %*s %i",name,&line);
					lineptr=strchr(lineptr,'\n');
					if (lineptr!=NULL)
						lineptr++;

					if((strcasecmp(name,selection)==0))
						{
							gtk_notebook_set_current_page(notebook,loop);
							gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,line-1,0);
							gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);
							gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
							return;
						}
				}
		}
//not in any open files
//check ./
	asprintf(&command,"ctags -xR %s",g_path_get_dirname(page->filePath));
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			g_free(stderr);
		}

	lineptr=stdout;
	while (lineptr!=NULL)
		{
			sscanf (lineptr,"%s %*s %i %s",name,&line,file);
			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;

			if((strcasecmp(name,selection)==0))
				{
					openFile((char*)&file,line-1);
					g_free(stdout);
					return;
				}
		}
}

void findFile(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char		strarg[2048];
	char*		filename;
	char*		command;
	gchar*		stdout=NULL;
	gchar*		stderr=NULL;
	gint		retval=0;
	char*		lineptr;
	char		buffer[2048];
	char*		searchdir;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	sscanf(selection,"#include %s",(char*)&strarg);
	strarg[strlen(strarg)-1]=0;
	
	if(strarg[0]=='<')
		asprintf(&searchdir,"/usr/include");
	else
		asprintf(&searchdir,"%s",g_path_get_dirname(page->filePath));

	filename=g_path_get_basename(&strarg[1]);
	asprintf(&command,"find \"%s\" -name \"%s\"",searchdir,filename);
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			lineptr=stdout;
			while (lineptr!=NULL)
				{
					sscanf (lineptr,"%s",(char*)&buffer);
					lineptr=strchr(lineptr,'\n');
					if (lineptr!=NULL)
						lineptr++;
					openFile(buffer,0);
				}
			g_free(stdout);
			g_free(stderr);
		}

	g_free(searchdir);
	g_free(filename);
	g_free(command);
}

void externalTool(GtkWidget* widget,gpointer data)
{
	toolStruct*	tool=(toolStruct*)data;
	pageStruct*	page=getPageStructPtr(-1);
	char*		dirname;
	char*		text=NULL;
	GtkTextIter	start;
	GtkTextIter	end;

	dirname=g_path_get_dirname(page->filePath);
//
//	printf("%s\n",tool->menuName);
//	printf("%s\n",tool->filePath);
//	printf("%i\n",tool->flags);
//	printf("%s\n",tool->currentDir);
//	printf("%i\n",tool->paste);
//	printf("%i\n",tool->inTerminal);
//	asprintf(&fullcommand,"(cd \"%s\";%s)",dirname,tool->filePath);
//	runCommand(fullcommand,&text);
	chdir(dirname);
	setenv("KKEDIT_CURRENTFILE",page->filePath,1);
	setenv("KKEDIT_CURRENTDIR",dirname,1);
	setenv("KKEDIT_DATADIR",DATADIR,1);
	runCommand(tool->filePath,&text,tool->inTerminal);
	if(text!=NULL)
		{
			if(tool->flags & TOOL_REPLACE_OP)
				{
					gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
					gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
					gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (page->buffer),&start);
					gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&start,text,strlen(text));
				}

			if(tool->flags & TOOL_PASTE_OP)
				{
					if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
						gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
					gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(page->buffer),text,strlen(text));
				}
		}

	unsetenv("KKEDIT_FILEPATH");
	unsetenv("KKEDIT_CURRENTDIR");
	unsetenv("KKEDIT_DATADIR");
	g_free(text);
	g_free(dirname);
}

void addBookmark(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	printf("addBookmark\n");
	gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
}

void removeBookmark(GtkWidget* widget,gpointer data)
{
	printf("removeBookmark\n");
}

void openHelp(GtkWidget* widget,gpointer data)
{
	char*	runhelp;

	asprintf(&runhelp,"xdg-open %s/help/help.html",DATADIR);
	runCommand(runhelp,NULL,false);
	g_free(runhelp);
}
