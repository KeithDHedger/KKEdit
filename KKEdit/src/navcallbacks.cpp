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
	FILE*		fp;
	FILE*		fd;
	char		line[1024];
	char*		command;
	char*		searchdata[2048];
	int			cnt=0;
	char*		ptr=NULL;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
					selection=g_strdelimit(selection,"_",'-');
					asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.sgml\"  -exec grep -ie %s '{}' \\;|awk -F 'href=\"' '{print $2}'|awk -F '\">' '{print $1}'|sort",selection);
					fp=popen(command,"r");
					while(fgets(line,1024,fp))
						{
							asprintf(&searchdata[cnt],"%s",line);
							searchdata[cnt][strlen(searchdata[cnt])-1]=0;
							cnt++;
						}
					pclose(fp);

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
											ptr=(char*)(long)ptr++;
											fprintf(fd,"<a href=\"/usr/share/gtk-doc/html/%s\">%s</a><br>\n",searchdata[loop],ptr);
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

}
