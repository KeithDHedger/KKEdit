/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * functioncallbacks.cpp
 *
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceiter.h>
#include <unique/unique.h>

#include "config.h"
#ifdef BUILDDOCVIEWER
#include <webkit/webkit.h>
#endif

#include "globals.h"
#include "files.h"
#include "guis.h"

#ifdef BUILDDOCVIEWER
void webKitGoBack(GtkWidget* widget,gpointer data)
{
	webkit_web_view_go_back((WebKitWebView*)data);
}

void webKitGoForward(GtkWidget* widget,gpointer data)
{
	webkit_web_view_go_forward((WebKitWebView*)data);
}
#endif

char* thePage=NULL;

void docSearch(GtkWidget* widget,gpointer data)
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
															ptr=(char*)(long)ptr+1;
															fprintf(fd,"<a href=\"/usr/share/gtk-doc/html/%s\">%s</a><br>\n",searchdata[loop],ptr);
														}
													pclose(fp);
													g_free(command);
												}
										}
									fprintf(fd,"</body>\n");
									fprintf(fd,"</html>\n");
									fclose(fd);
									asprintf(&thePage,"file:///tmp/kkeditsearchfile.html");
								}
						}
					else
						{
							asprintf(&thePage,"file:///usr/share/gtk-doc/html/%s",searchdata[0]);
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

#ifdef BUILDDOCVIEWER
void showDocView(GtkWidget* widget,gpointer data)
{
	docSearch(NULL,NULL);


	if(thePage!=NULL)
		{
			webkit_web_view_load_uri(webView,thePage);
			g_free(thePage);
			thePage=NULL;
		}

	gtk_widget_show_all(docView);
	gtk_window_present((GtkWindow*)docView);
}
#endif

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data)
{
	pageStruct* 			page=getPageStructPtr(-1);
	gchar*					selectedtext=NULL;
	GtkSourceSearchFlags	flags=GTK_SOURCE_SEARCH_TEXT_ONLY;
	char*					searchtext;
	char*					replacetext;
	bool					replaceAllFlag;

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	if(insensitiveSearch==true)
		flags=(GtkSourceSearchFlags)(GTK_SOURCE_SEARCH_TEXT_ONLY|GTK_SOURCE_SEARCH_CASE_INSENSITIVE);

	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)findBox));
	replacetext=g_strcompress(gtk_entry_get_text((GtkEntry*)replaceBox));

	switch (response_id)
		{
//forward search
			case FINDNEXT:
				
				if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));

				if(gtk_source_iter_forward_search(&page->match_end,searchtext,flags,&page->match_start,&page->match_end,NULL))
					{
						gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
						gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
						page->iter=page->match_end;
					}
				else
					{
						if(wrapSearch==true)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
								if(gtk_source_iter_forward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
									{
										gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
										page->iter=page->match_end;
									}
							}
					}

				break;
//backward search
			case FINDPREV:
				if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
				if(gtk_source_iter_backward_search(&page->match_start,searchtext,flags,&page->match_start,&page->match_end,NULL))
					{
						gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
						gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
						page->iter=page->match_start;
					}
				else
					{
						if(wrapSearch==true)
							{
								gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->iter);
								if(gtk_source_iter_backward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
									{
										gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
										page->iter=page->match_start;
									}
							}
					}

				break;
//replace and search
			case REPLACE:
				if(replaceAll==true)
					{
					replaceAllFlag=true;
					gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
					gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&page->iter);
					if(gtk_source_iter_forward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
						{
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
							gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
							page->iter=page->match_end;
						}
					else
						break;
					}
				else
					replaceAllFlag=false;
				do
				{
				if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
					{
						selectedtext=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end,false);
	
	
						if((insensitiveSearch==true && strcasecmp(selectedtext,searchtext)==0) ||(insensitiveSearch==false && strcmp(selectedtext,searchtext)==0))
							{
								gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
								gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&page->match_start,replacetext,-1);
								gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
								if(gtk_source_iter_forward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
									{
										gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&page->match_start,0,true,0,0.5);
										page->iter=page->match_end;
									}
								else
									{
									replaceAllFlag=false;
									}
							}
						if(selectedtext!=NULL)
							g_free(selectedtext);
					}
					}
					while(replaceAllFlag==true);
				break;

			default:
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
				page->isFirst=true;
				page->match_start=page->iter;
				page->match_end=page->iter;
				break;
		}
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
	g_free(searchtext);
	g_free(replacetext);
}

void find(GtkWidget* widget,gpointer data)
{
	gtk_widget_show(findReplaceDialog);
	gtk_dialog_run((GtkDialog *)findReplaceDialog);
}

