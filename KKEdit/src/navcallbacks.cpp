/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceiter.h>
#include <unique/unique.h>

#include "globals.h"
#include "files.h"
#include "guis.h"

int	theLineNum=0;
int marknum=0;

void goToDefine(functionData* fdata)
{
	pageStruct*	page;
	GtkTextIter	iter;

	if(fdata->intab==-1)
		openFile(fdata->file,fdata->line-1);
	else
		{
			page=getPageStructPtr(fdata->intab);
			gtk_notebook_set_current_page(notebook,fdata->intab);
			gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,fdata->line-1,0);
			gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);
			gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
		}

}

void goToDefinition(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	functionData* fdata=getFunctionByName(selection);
	if(fdata!=NULL)
		{
			goToDefine(fdata);
			destroyData(fdata);
		}
	return;
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

void gotoLine(GtkWidget* widget,gpointer data)
{
	int			line=(long)data;
	GtkTextIter	iter;
	pageStruct*	page=getPageStructPtr(-1);

	if(page!=NULL)
		{
			gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&iter,line-1,0);
			gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);
			gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
		}
}

void jumpToLineFromBar(GtkWidget* widget,gpointer data)
{
		theLineNum=atoi(gtk_entry_get_text((GtkEntry*)widget));
		gotoLine(NULL,(gpointer)(long)theLineNum);
}

int showLineEntry(void)
{
	GtkWidget*	dialog;
	gint		result;
	GtkWidget*	content_area;
	GtkWidget*	entrybox;
	char		line[48];

	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,"Enter Line Number");

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Go To Line");

	content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));	
	entrybox=gtk_entry_new();
	sprintf((char*)&line,"%i",theLineNum);
	gtk_entry_set_text((GtkEntry*)entrybox,line);
	gtk_entry_set_activates_default((GtkEntry*)entrybox,true);
	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_YES);
	gtk_container_add(GTK_CONTAINER(content_area),entrybox);
	gtk_widget_show_all(content_area);
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	theLineNum=atoi(gtk_entry_get_text((GtkEntry*)entrybox));
	
	gtk_widget_destroy(dialog);

	return(result);
}

void jumpToLine(GtkWidget* widget,gpointer data)
{
	if(showLineEntry()==GTK_RESPONSE_YES)
		gotoLine(NULL,(gpointer)(long)theLineNum);
}

void functionSearch(GtkWidget* widget,gpointer data)
{
	if(showFunctionEntry()==GTK_RESPONSE_YES)
	{
		functionData* fdata=getFunctionByName(functionSearchText);
		if(fdata!=NULL)
		{
			goToDefine(fdata);
			destroyData(fdata);
		}
	}
}

void jumpToMark(GtkWidget* widget,gpointer data)
{
	GtkTextMark*	mark;
	pageStruct*		page;
	GtkTextIter		iter;
	int				thistab=currentTabNumber;

	for(int loop=0;loop<gtk_notebook_get_n_pages(notebook);loop++)
		{
			page=getPageStructPtr(loop);
			mark=gtk_text_buffer_get_mark((GtkTextBuffer*)page->buffer,(char*)data);
			if(mark!=NULL)
				{
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
					gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
					gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);
					if(thistab!=loop)
						gtk_notebook_set_current_page(notebook,loop);
				}
		}
}

void addBookmark(GtkWidget* widget,gpointer data)
{
	pageStruct*		page=getPageStructPtr(-1);
	GtkWidget*		menuitem;
	GtkTextMark*	mark;
	GtkTextIter		iter;
	char*			name;
	int				line;
	GtkTextIter		startprev,endprev;
	char*			previewtext;

	mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
	gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);

	line=gtk_text_iter_get_line(&iter);

	gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&startprev,line);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&endprev,line+1);
	previewtext=gtk_text_iter_get_text(&startprev,&endprev);

	previewtext[strlen(previewtext)-1]=0;
	asprintf(&name,"BookMark%i",marknum);

	gtk_text_buffer_create_mark((GtkTextBuffer*)page->buffer,name,&iter,true);
	g_strchug(previewtext);
	g_strchomp(previewtext);

	menuitem=gtk_image_menu_item_new_with_label(previewtext);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubookmarksub),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(jumpToMark),(void*)name);
	marknum++;
	gtk_widget_show_all(menubookmark);
}

void removeBookmark(GtkWidget* widget,gpointer data)
{
	printf("removeBookmark\n");
}
