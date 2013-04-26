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
	char*		endptr=NULL;
	int			startchar;
	int			endchar;

	char*		funcname;
	char*		foldername;
	char*		tempstr;
	char		tmpbuffer[256];
	char*		link;
	int			cnt=0;
	char*		itermediate;

	for(int loop=0;loop<2048;loop++)
		{
			searchdata[loop][0]=NULL;
			searchdata[loop][1]=NULL;
		}

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
					asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %s '{}' \\;",selection);
					fp=popen(command,"r");
					while(fgets(line,1024,fp))
						{
							ptr=strstr(line,"name=\"");
							if(ptr!=NULL)
								{
									funcname=sliceBetween(line,"name=\"","\" link=");
									if(strstr(funcname,selection)!=NULL)
										{
											tempstr=sliceBetween(line,"",":");
											foldername=g_path_get_dirname(tempstr);
											link=sliceBetween(line,"link=\"","\"");

											//printf("%s\n",line);
											//printf("\n%s \n %s \n %s\n",funcname,foldername,link);
											asprintf(&searchdata[cnt][0],"%s",funcname);
											asprintf(&searchdata[cnt][1],"%s/%s",foldername,link);
											g_free(foldername);
											g_free(tempstr);
											g_free(link);
											cnt++;
										}
									g_free(funcname);
								}
						}
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
											fprintf(fd,"<a href=\"%s\">%s</a><br>\n",searchdata[loop][1],searchdata[loop][0]);
										}
									fprintf(fd,"</body>\n");
									fprintf(fd,"</html>\n");
									fclose(fd);
									asprintf(&thePage,"file:///tmp/kkeditsearchfile.html");
								}
						}
					else
						{
							asprintf(&thePage,"file://%s",searchdata[0][1]);
							//printf("file://%s\n",searchdata[0][1]);
						}
				}
		}
}

void docSearchYY(GtkWidget* widget,gpointer data)
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
	char*		endptr=NULL;
	int			startchar;
	int			endchar;

	char*		funcname;
	char*		foldername;
	char*		tempstr;
	char		tmpbuffer[256];
	char*		link;
	int			cnt=0;

	for(int loop=0;loop<2048;loop++)
		{
			searchdata[loop][0]=NULL;
			searchdata[loop][1]=NULL;
		}

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
					asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %s '{}' \\;",selection);
					fp=popen(command,"r");
					while(fgets(line,1024,fp))
						{
							ptr=strstr(line,"name=\"");
							if(ptr!=NULL)
								{
									ptr=(char*)(long)ptr+6;
									endchar=(int)(long)strchr(ptr,'"')-(long)ptr;
									asprintf(&funcname,"%.*s",endchar,ptr);

									endchar=(int)(long)strchr(line,':')-(long)line;
									strncpy(tmpbuffer,line,endchar);
									tmpbuffer[endchar]=0;
									foldername=g_path_get_dirname(tmpbuffer);
									
									ptr=strstr(line,"link=\"");
									ptr=(char*)(long)ptr+6;
									endptr=strstr(ptr,"\"/>");
									endptr=(char*)(long)endptr;
									endchar=(int)(long)endptr-(long)ptr;
									strncpy(tmpbuffer,ptr,endchar);
									tmpbuffer[endchar]=0;
									asprintf(&link,"%s/%s",foldername,tmpbuffer);
									
									printf("%s\n",line);
									printf("\n%s \n %s \n %s\n",funcname,foldername,link);
									asprintf(&searchdata[cnt][0],"%s",funcname);
									asprintf(&searchdata[cnt][1],"%s",link);
									g_free(funcname);
									g_free(foldername);
									g_free(tempstr);
									g_free(link);
									cnt++;
								}
						}
					if(cnt>1)
						{
							fd=fopen("/tmp/kkeditsearchfile.html","w");
							if(fd!=NULL)
								{								
									fprintf(fd,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
									fprintf(fd,"<html>\n");
									fprintf(fd,"<body>\n");

//<a href="/usr/share/gtk-doc/html/glib/glib-IO-Channels.html#g-io-channel-close">g-io-channel-close</a><br>

									for(int loop=0;loop<cnt;loop++)
										{
											fprintf(fd,"<a href=\"%s\">%s</a><br>\n",searchdata[loop][1],searchdata[loop][0]);
										}
									fprintf(fd,"</body>\n");
									fprintf(fd,"</html>\n");
									fclose(fd);
									asprintf(&thePage,"file:///tmp/kkeditsearchfile.html");
								}
						}
					else
						{
							asprintf(&thePage,"file:///usr/share/gtk-doc/html/%s",searchdata[0][0]);
						}
				}
		}
}


void docSearchXX(GtkWidget* widget,gpointer data)
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
	if(data==NULL)
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

