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

void gtkDocSearch(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char*		selectionorig=NULL;
	FILE*		fp;
	FILE*		fd;
	char		line[1024];
	char*		command=NULL;
	char*		searchdata[2048];
	int			cnt=0;
	char*		ptr=NULL;
	long		hashsign;

	for(int loop=0;loop<2048;loop++)
		searchdata[loop]=NULL;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
					selectionorig=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
					selection=g_strdelimit(selection,"_",'-');
					asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.sgml\"  -exec grep -ie %s '{}' \\;|awk -F 'href=\"' '{print $2}'|awk -F '\">' '{print $1}'|sort",selection);
					fp=popen(command,"r");
					while(fgets(line,1024,fp))
						{
							asprintf(&searchdata[cnt],"%s",line);
							searchdata[cnt][strlen(searchdata[cnt])-1]=0;
							cnt++;
							if(cnt>2048)
								break;
						}
					pclose(fp);
					g_free(command);

					if(cnt>1)
						{
							fd=fopen("/tmp/kkeditsearchfile.html","w");
							if(fd!=NULL)
								{								
									fprintf(fd,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
									fprintf(fd,"<html>\n");
									fprintf(fd,"<body>\n");

									for(int loop=0;loop<cnt;loop++)
										{
											ptr=strchr(searchdata[loop],'#');
											if(ptr!=NULL)
												{
													hashsign=(long)ptr-(long)searchdata[loop];
													searchdata[loop][hashsign]=0;
													asprintf(&command,"sed -n '/%s/p' /usr/share/gtk-doc/html/%s  2>/dev/null",selectionorig,searchdata[loop]);
													searchdata[loop][hashsign]='#';
													fp=popen(command,"r");
													line[0]=0;
													fgets(line,1024,fp);
													if(strlen((char*)line)>1)
														{
															ptr=(char*)(long)ptr++;
															fprintf(fd,"<a href=\"/usr/share/gtk-doc/html/%s\">%s</a><br>\n",searchdata[loop],ptr);
														}
													pclose(fp);
													g_free(command);
																									
												}
										}
									fprintf(fd,"</body>\n");
									fprintf(fd,"</html>\n");
									fclose(fd);

									g_spawn_command_line_async("xdg-open file:///tmp/kkeditsearchfile.html",NULL);
								}
						}
					else
						{
							asprintf(&command,"xdg-open file:///usr/share/gtk-doc/html/%s",searchdata[0]);
							g_spawn_command_line_async(command,NULL);
						}
				}
		}

	for(int loop=0;loop<cnt;loop++)
		{
			if(searchdata[loop]!=NULL)
				g_free(searchdata[loop]);
		}

	if(selection!=NULL)
		g_free(selection);
	if(selectionorig!=NULL)
		g_free(selectionorig);
}

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

int	theLineNum=0;

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

