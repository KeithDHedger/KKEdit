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

#include "globals.h"
#include "files.h"
#include "guis.h"

void webKitGoBack(GtkWidget* widget,gpointer data)
{
	webkit_web_view_go_back((WebKitWebView*)data);
}

void webKitGoForward(GtkWidget* widget,gpointer data)
{
	webkit_web_view_go_forward((WebKitWebView*)data);
}

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
	
}


