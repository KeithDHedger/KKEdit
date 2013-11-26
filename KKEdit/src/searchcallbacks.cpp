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
#include "navcallbacks.h"

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
																	searchdata[cnt][0]=strdup(funcname);
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
							thePage=strdup(htmlURI);
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

void defSearchFromBar(GtkWidget* widget,gpointer data)
{
	functionSearchText=strdup(gtk_entry_get_text((GtkEntry*)widget));
	if(functionSearchText!=NULL)
		{
			functionData* fdata=getFunctionByName(functionSearchText,true);
			if(fdata!=NULL)
				{
					goToDefine(fdata);
					destroyData(fdata);
				}
			g_free(functionSearchText);
		}
}

void docSearchFromBar(GtkWidget* widget,gpointer data)
{
	const char* text=gtk_entry_get_text((GtkEntry*)data);
	docSearch(NULL,(void*)text);
	showDocView(NULL,(void*)text);
}

#include <regex.h>


void doFindReplacezz(GtkDialog *dialog,gint response_id,gpointer user_data)
{
regex_t regex;
        int reti;
        char msgbuf[100];
	char*		searchtext;
	char*		replacetext;
	GtkTextIter	start,end;
	pageStruct*	page=getPageStructPtr(-1);
	char*		text=NULL;

	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)findBox));
	replacetext=g_strcompress(gtk_entry_get_text((GtkEntry*)replaceBox));

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
	text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);

/* Compile regular expression */
        reti = regcomp(&regex, searchtext, 0);
        if( reti ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }

printf("regex=%s\n",searchtext);
/* Execute regular expression */
        reti = regexec(&regex, text, 0, NULL, 0);
        if( !reti ){
                puts("Match");
        }
        else if( reti == REG_NOMATCH ){
                puts("No match");
        }
        else{
                regerror(reti, &regex, msgbuf, sizeof(msgbuf));
                fprintf(stderr, "Regex match failed: %s\n", msgbuf);
                exit(1);
        }

/* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);

}

char*	texttosearch=" \
0.0.23 \
Fixed bug when GtkSu not installed. \
Set document dirty after spellcheck. \
\
0.0.22\
Added check for installed manpageeditor, if succeeds menu item added in file menu.\
Code clean.\
Undo bugfix.\
Added Manpage Editor Icon for menuitem.\
Added Always Show In Popup Men to external tools ( no selection needed ).\
Redone some installed tools and example tools.\
\
0.0.21\
Added Split View to tab menu, click once to split, again to un-split.\
Live Search is now always case insensitive.\
Removed dirty flag from buffer when first opening as hexdump.\
Added configure and associated files to archive so no longer needs auto-tools.\
Updated INSTALL file for Debian/fedora users.\
Added drop down for syntax highlighting.\
Added New Editor to file menu.\
Added New Admin Editor to file menu.\
Updated help file.\
Fixed close tab bug\
Code clean.\
Now allows pipes in external tool command.";


void doFindReplacezzzzzzz(GtkDialog *dialog,gint response_id,gpointer user_data)
{
	char*		searchtext;
	char*		replacetext;
	GtkTextIter	starti,endi;
	pageStruct*	page=getPageStructPtr(-1);
	char*		buffer=NULL;

	regmatch_t matches[2];
	int start;
	int end;  
	regex_t re;
	int count=-100;

	searchtext=(char*)gtk_entry_get_text((GtkEntry*)findBox);
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&starti);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&endi);
	buffer=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&starti,&endi,false);
	if(buffer!=NULL)
		{
		int reti = regcomp(&re, searchtext, REG_EXTENDED|REG_ICASE);
			while (1) {
  reti = regexec(&re, buffer, 2, matches, 0);

  /* rm_so is the start index of the match */
  start = matches[1].rm_so;
  /* rm_eo is the end index of the match */
  end = matches[1].rm_eo;
  /* break if we didn't find a match */
  if (reti) break;

  /* print the substring that contains the match */
  printf("%.*s, \n", (end - start), (buffer + start));
  /* increment the count of matches */
  count = count + 1;

  /* This is really important!
     Move the start of the string forward so we don't keep matching the same number! */
  buffer = buffer + end;
} 

/* print the count */
printf("count = %d\n", count);
		}
}

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data)
{
	char*					searchtext=NULL;
	char*					replacetext=NULL;
	GtkTextIter				start,end;
	pageStruct*				page=getPageStructPtr(-1);
	char*					text=NULL;
	int						startpos,endpos;

	GRegex*					regex;
	GMatchInfo*				match_info=NULL;
	GRegexCompileFlags		compileflags=(GRegexCompileFlags)(G_REGEX_MULTILINE|G_REGEX_EXTENDED);
	GRegexMatchFlags		matchflags=(GRegexMatchFlags)(G_REGEX_MATCH_NOTBOL|G_REGEX_MATCH_NOTEOL);
	GtkSourceSearchFlags	flags=GTK_SOURCE_SEARCH_TEXT_ONLY;

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	replacetext=g_strcompress(gtk_entry_get_text((GtkEntry*)replaceBox));
	searchtext=(char*)gtk_entry_get_text((GtkEntry*)findBox);

	if(insensitiveSearch==true)
		{
			compileflags=(GRegexCompileFlags)(compileflags|G_REGEX_CASELESS);
			flags=(GtkSourceSearchFlags)(GTK_SOURCE_SEARCH_TEXT_ONLY|GTK_SOURCE_SEARCH_CASE_INSENSITIVE);
		}

	regex=g_regex_new(searchtext,(GRegexCompileFlags)compileflags,matchflags,NULL);

	switch (response_id)
		{
//forward search
			case FINDNEXT:
					if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
						gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));

					gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
					gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
					text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);

					if(g_regex_match_full(regex,text,-1,gtk_text_iter_get_offset(&page->match_end),matchflags,&match_info,NULL))
						{
							g_match_info_fetch_pos(match_info,0,&startpos,&endpos);

							gtk_text_iter_set_offset(&page->match_start,startpos);
							page->match_end=page->match_start;

							gtk_text_iter_set_offset(&page->match_end,endpos);
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
							scrollToIterInPane(page,&page->match_start);
							page->iter=page->match_end;
						}
					else
						if(wrapSearch==true)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->match_end);

								if(g_regex_match_full(regex,text,-1,gtk_text_iter_get_offset(&page->match_end),matchflags,&match_info,NULL))
									{
										g_match_info_fetch_pos(match_info,0,&startpos,&endpos);
										gtk_text_iter_set_offset(&page->match_start,startpos);
										page->match_end=page->match_start;
										gtk_text_iter_set_offset(&page->match_end,endpos);
										gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										scrollToIterInPane(page,&page->match_start);
										page->iter=page->match_end;
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
						scrollToIterInPane(page,&page->match_start);
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
										scrollToIterInPane(page,&page->match_start);
										page->iter=page->match_start;
									}
							}
					}
				break;

//replace and search
			case REPLACE:
				gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
				gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&page->match_start,replacetext,-1);
				break;
			default:
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
				page->isFirst=true;
				page->match_start=page->iter;
				page->match_end=page->iter;
				break;
		}

	if(match_info!=NULL)
		g_match_info_free(match_info);

	g_regex_unref(regex);

	if(text!=NULL)
		g_free(text);

	if(replacetext!=NULL)
		g_free(replacetext);

	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}

void doFindReplaceX(GtkDialog *dialog,gint response_id,gpointer user_data)
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
						scrollToIterInPane(page,&page->match_start);
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
										scrollToIterInPane(page,&page->match_start);
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
						scrollToIterInPane(page,&page->match_start);
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
										scrollToIterInPane(page,&page->match_start);
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
							scrollToIterInPane(page,&page->match_start);
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
										scrollToIterInPane(page,&page->match_start);
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

void doLiveSearch(GtkWidget* widget,GdkEvent *event,gpointer data)
{
	pageStruct* 			page=getPageStructPtr(-1);
	GtkSourceSearchFlags	flags;
	char*					searchtext;
	int						modkey=((GdkEventKey*)event)->state;

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	flags=(GtkSourceSearchFlags)(GTK_SOURCE_SEARCH_TEXT_ONLY|GTK_SOURCE_SEARCH_CASE_INSENSITIVE);

	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)widget));

	if(modkey==0)
		{
			if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));

			if(gtk_source_iter_forward_search(&page->match_end,searchtext,flags,&page->match_start,&page->match_end,NULL))
				{
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
					scrollToIterInPane(page,&page->match_start);
					page->iter=page->match_end;
				}
			else
				{
					gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
					if(gtk_source_iter_forward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
						{
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
							scrollToIterInPane(page,&page->match_start);
							page->iter=page->match_end;
						}
				}
		}
	else
		{
			if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
			if(gtk_source_iter_backward_search(&page->match_start,searchtext,flags,&page->match_start,&page->match_end,NULL))
				{
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
					scrollToIterInPane(page,&page->match_start);
					page->iter=page->match_start;
				}
			else
				{
					gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->iter);
					if(gtk_source_iter_backward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
						{
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
							scrollToIterInPane(page,&page->match_start);
							page->iter=page->match_start;
						}
				}
		}
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}








