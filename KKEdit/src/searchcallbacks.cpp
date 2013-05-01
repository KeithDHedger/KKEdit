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

void webKitGoHome(GtkWidget* widget,gpointer data)
{
	if(g_file_test(htmlFile,G_FILE_TEST_EXISTS)==true)
		webkit_web_view_load_uri((WebKitWebView*)data,htmlURI);
}
#endif

void docSearch(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char*		searchdata[2048][2];
	char		line[1024];
	FILE*		fp;
	FILE*		fd;
	char*		command=NULL;
	char*		ptr=NULL;
	char*		funcname;
	char*		foldername;
	char*		tempstr;
	char*		link;
	int			cnt=0;

	for(int loop=0;loop<2048;loop++)
		{
			searchdata[loop][0]=NULL;
			searchdata[loop][1]=NULL;
		}

	if(data!=NULL)
		{
			selection=(char*)data;
		}
	else
		{
			if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
				{
					selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
				}
		}

	if(selection!=NULL)
		{
			asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %s '{}' \\;",selection);
			fp=popen(command,"r");
			while(fgets(line,1024,fp))
				{
					ptr=strstr(line,"name=\"");
					if(ptr!=NULL)
						{
							funcname=sliceBetween(line,(char*)"name=\"",(char*)"\" link=");
							if(funcname!=NULL)
								{
									if(strstr(funcname,selection)!=NULL)
										{
											if(cnt<2048)
												{
													tempstr=sliceBetween(line,(char*)"",(char*)":");
													if(tempstr!=NULL)
														{
															foldername=g_path_get_dirname(tempstr);
															link=sliceBetween(line,(char*)"link=\"",(char*)"\"");
															if((foldername!=NULL) && (link!=NULL))
																{
																	asprintf(&searchdata[cnt][0],"%s",funcname);
																	asprintf(&searchdata[cnt][1],"%s/%s",foldername,link);
																	g_free(foldername);
																	g_free(link);
																	cnt++;
																}
															g_free(tempstr);
														}
												}
										}
									g_free(funcname);
								}
						}
				}

			if(cnt>1)
				{
					fd=fopen(htmlFile,"w");
					if(fd!=NULL)
						{								
							fprintf(fd,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
							fprintf(fd,"<html>\n");
							fprintf(fd,"<body>\n");

							for(int loop=0;loop<cnt;loop++)
								{
									fprintf(fd,"<a href=\"%s\">%s</a><br>\n",searchdata[loop][1],searchdata[loop][0]);
								}
							fprintf(fd,"</body>\n");
							fprintf(fd,"</html>\n");
							fclose(fd);
							asprintf(&thePage,"%s",htmlURI);
						}
				}
			else
				{
					asprintf(&thePage,"file://%s",searchdata[0][1]);
				}
		}

	for(int loop=0;loop<cnt;loop++)
		{
			if(searchdata[loop][0]!=NULL)
				g_free(searchdata[loop][0]);
			if(searchdata[loop][1]!=NULL)
				g_free(searchdata[loop][1]);
		}
	if((selection!=NULL) && (data==NULL))
		g_free(selection);
}

void showDocView(GtkWidget* widget,gpointer data)
{
	if(data==NULL)
		docSearch(NULL,NULL);

#ifdef BUILDDOCVIEWER
	if(thePage!=NULL)
		{
			webkit_web_view_load_uri(webView,thePage);
			g_free(thePage);
			thePage=NULL;
		}
	else
		{
			webKitGoHome(NULL,(void*)webView);
		}
	gtk_widget_show_all(docView);
	gtk_window_present((GtkWindow*)docView);

#else
	char*	command;

	if(thePage!=NULL)
		{
			asprintf(&command,"xdg-open %s",thePage);
			g_spawn_command_line_async(command,NULL);
			g_free(command);
			g_free(thePage);
			thePage=NULL;
		}
	else
		{
			asprintf(&command,"xdg-open %s",htmlURI);
			g_spawn_command_line_async(command,NULL);
			g_free(command);			
		}
#endif
}

void docSearchFromBar(GtkWidget* widget,gpointer data)
{
	const char* text=gtk_entry_get_text((GtkEntry*)data);
	docSearch(NULL,(void*)text);
	showDocView(NULL,(void*)text);
}

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

void doSearchPrefs(GtkWidget* widget,gpointer data)
{
	GtkWidget*	button;

	switch ((long)data)
		{
			case 1:
				insensitiveSearch=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 2:
				wrapSearch=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 3:
				replaceAll=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				button=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,100);
				if(replaceAll==false)
					gtk_button_set_label((GtkButton*)button,"Replace");
				else
					gtk_button_set_label((GtkButton*)button,"Replace All");
				break;
		}
}

