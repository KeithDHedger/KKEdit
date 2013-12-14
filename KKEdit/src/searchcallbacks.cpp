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
#include "callbacks.h"
#include "searchcallbacks.h"

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
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;

	if(data==NULL)
		docSearch(NULL,NULL);

	if(strcasecmp("file://(null)",thePage)==0)
		{
			g_free(thePage);
			if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
				{
					selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
					//asprintf(&thePage,"xdg-open https://www.google.co.uk/search?q=%s",selection);
					asprintf(&thePage,"https://www.google.co.uk/search?q=%s",selection);
					webkit_web_view_load_uri(webView,thePage);
					//g_free(thePage);
					//g_free(selection);
					//return;
				}
		}

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

int		currentFindPage=-1;
int		firstPage=-1;
int		pagesChecked=0;


void doAllFiles(int dowhat,bool found)
{
	pageStruct*	page=NULL;

	if((findInAllFiles==true) && (found==false))
		{
			if(dowhat==FINDNEXT)
				{
					currentFindPage=gtk_notebook_get_current_page(notebook)+1;
					if(currentFindPage==gtk_notebook_get_n_pages(notebook))
						currentFindPage=0;
				}
			else
				{
					currentFindPage=gtk_notebook_get_current_page(notebook)-1;
					if(currentFindPage==-1)
						currentFindPage=gtk_notebook_get_n_pages(notebook)-1;
				}

			pagesChecked++;
			if(pagesChecked>gtk_notebook_get_n_pages(notebook))
				{
					currentFindPage=-1;
					gtk_notebook_set_current_page(notebook,firstPage);
					pagesChecked=0;
					return;
				}
		}

	gtk_notebook_set_current_page(notebook,currentFindPage);
	page=getPageStructPtr(currentFindPage);

	if(dowhat==FINDNEXT)
		{
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->match_start);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->match_end);
			gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
		}
	else
		{
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->iter);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->match_start);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->match_end);
			gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
		}

	if(useRegex==false)
		basicFind(dowhat);
	else
		regexFind(dowhat);
}

void regexFind(int dowhat)
{
	char*					searchtext=NULL;
	char*					replacetext=NULL;
	GtkTextIter				start,end;
	pageStruct*				page=NULL;
	pageStruct*				tmppage=NULL;
	int						pageloop;

	char*					text=NULL;
	int						startpos,endpos;
	char*					reptext=NULL;
	GRegex*					regex;
	GMatchInfo*				match_info=NULL;
	GRegexCompileFlags		compileflags=(GRegexCompileFlags)(G_REGEX_MULTILINE|G_REGEX_EXTENDED);
	GRegexMatchFlags		matchflags=(GRegexMatchFlags)(G_REGEX_MATCH_NOTBOL|G_REGEX_MATCH_NOTEOL);
	int						charstartpos;
	int						charendpos;
	bool					found=false;

	if(findInAllFiles==false)
		currentFindPage=-1;
	else
		{
			if(currentFindPage!=-1)
				{
					currentFindPage=gtk_notebook_get_current_page(notebook);
					firstPage=currentFindPage;
				}
			else
				pagesChecked=0;
		}

	page=getPageStructPtr(currentFindPage);

	searchtext=(char*)gtk_entry_get_text((GtkEntry*)findBox);
	replacetext=(char*)gtk_entry_get_text((GtkEntry*)replaceBox);

	if(insensitiveSearch==true)
		compileflags=(GRegexCompileFlags)(compileflags|G_REGEX_CASELESS);

	regex=g_regex_new(searchtext,(GRegexCompileFlags)compileflags,matchflags,NULL);
	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	switch (dowhat)
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
						found=true;
						g_match_info_fetch_pos(match_info,0,&startpos,&endpos);
						charstartpos=g_utf8_pointer_to_offset(text,&text[startpos]);
						charendpos=g_utf8_pointer_to_offset(text,&text[endpos]);
						gtk_text_iter_set_offset(&page->match_start,charstartpos);
						page->match_end=page->match_start;

						gtk_text_iter_set_offset(&page->match_end,charendpos);
						gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
						scrollToIterInPane(page,&page->match_start);
						page->iter=page->match_end;
					}
				else
					{
						if((wrapSearch==true) && (findInAllFiles==false))
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->match_end);
								if(g_regex_match_full(regex,text,-1,gtk_text_iter_get_offset(&page->match_end),matchflags,&match_info,NULL))
									{
										found=true;
										g_match_info_fetch_pos(match_info,0,&startpos,&endpos);
										charstartpos=g_utf8_pointer_to_offset(text,&text[startpos]);
										charendpos=g_utf8_pointer_to_offset(text,&text[endpos]);

										gtk_text_iter_set_offset(&page->match_start,charstartpos);
										page->match_end=page->match_start;
										gtk_text_iter_set_offset(&page->match_end,charendpos);
										gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										scrollToIterInPane(page,&page->match_start);
										page->iter=page->match_end;
									}
								else
									break;
							}
					if((wrapSearch==true) && (findInAllFiles==true))
						doAllFiles(dowhat,found);
					}
				break;

			case REPLACE:
				if((findInAllFiles==true) && (replaceAll==true))
					{
						if(yesNo((char*)"Do you want to replace in ALL open files?",(char*)"")==GTK_RESPONSE_YES)
							{
								for(pageloop=0;pageloop<gtk_notebook_get_n_pages(notebook);pageloop++)
									{
										gtk_notebook_set_current_page(notebook,currentFindPage);
										tmppage=getPageStructPtr(pageloop);
										gtk_text_buffer_get_start_iter((GtkTextBuffer*)tmppage->buffer,&start);
										gtk_text_buffer_get_end_iter((GtkTextBuffer*)tmppage->buffer,&end);
										text=gtk_text_buffer_get_text((GtkTextBuffer*)tmppage->buffer,&start,&end,false);
										reptext=g_regex_replace(regex,text,-1,0,replacetext,matchflags,NULL);
										if(strcmp(reptext,text)!=0)
											{
												gtk_text_buffer_get_start_iter((GtkTextBuffer*)tmppage->buffer,&start);
												gtk_text_buffer_get_end_iter((GtkTextBuffer*)tmppage->buffer,&end);
												gtk_text_buffer_delete((GtkTextBuffer*)tmppage->buffer,&start,&end);
												gtk_text_buffer_insert((GtkTextBuffer*)tmppage->buffer,&start,reptext,-1);
											}
										if(text!=NULL)
											{
												g_free(text);
												text=NULL;
											}
										if(reptext!=NULL)
											{
												g_free(reptext);
												reptext=NULL;
											}
									}
							}
						break;
					}
			
				if((replaceAll==true) && (findInAllFiles==false))
					{
						gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
						gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
						text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
						reptext=g_regex_replace(regex,text,-1,0,replacetext,matchflags,NULL);
						if(strcmp(reptext,text)!=0)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
								gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
								gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&start,&end);
								gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,reptext,-1);
							}
						break;
					}

				if(replaceAll==false)
					{
						if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
							{
								regex=g_regex_new(searchtext,(GRegexCompileFlags)(G_REGEX_CASELESS|G_REGEX_EXTENDED),(GRegexMatchFlags)0,NULL);
								text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end,false);
								reptext=g_regex_replace(regex,text,-1,0,replacetext,(GRegexMatchFlags)0,NULL);
								if(strcmp(reptext,text)!=0)
									{
										gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&page->match_start,reptext,-1);
										regexFind(FINDNEXT);
									}
							}
					}
				break;
		}
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
	g_regex_unref(regex);
	if(text!=NULL)
		g_free(text);
	if(reptext!=NULL)
	g_free(reptext);
}

void basicFind(int dowhat)
{
	pageStruct*				page=NULL;
	char*					searchtext=NULL;
	char*					replacetext;
	gchar*					selectedtext=NULL;
	GtkSourceSearchFlags	flags=GTK_SOURCE_SEARCH_TEXT_ONLY;
	bool					replaceAllFlag;
	bool					found=false;

	if(findInAllFiles==false)
		currentFindPage=-1;
	else
		{
			if(currentFindPage!=-1)
				{
					currentFindPage=gtk_notebook_get_current_page(notebook);
					firstPage=currentFindPage;
				}
			else
				pagesChecked=0;
		}

	page=getPageStructPtr(currentFindPage);

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	if(insensitiveSearch==true)
		flags=(GtkSourceSearchFlags)(GTK_SOURCE_SEARCH_TEXT_ONLY|GTK_SOURCE_SEARCH_CASE_INSENSITIVE);

	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)findBox));
	replacetext=g_strcompress(gtk_entry_get_text((GtkEntry*)replaceBox));

	if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
		gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));

	if(dowhat==FINDNEXT)
		{
			if(gtk_source_iter_forward_search(&page->match_end,searchtext,flags,&page->match_start,&page->match_end,NULL))
				{
					found=true;
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
					scrollToIterInPane(page,&page->match_start);
					page->iter=page->match_end;
				}
			else
				{
					if((wrapSearch==true) && (findInAllFiles==false))
						{
							gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
							if(gtk_source_iter_forward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
								{
									found=true;
									gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
									scrollToIterInPane(page,&page->match_start);
									page->iter=page->match_end;
								}
							gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
							g_free(searchtext);
							g_free(replacetext);
							return;
						}
					if((wrapSearch==true) || (findInAllFiles==true))
						doAllFiles(dowhat,found);
				}
		}

		if(dowhat==FINDPREV)
			{
				if(gtk_source_iter_backward_search(&page->match_start,searchtext,flags,&page->match_start,&page->match_end,NULL))
					{
						found=true;
						gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
						scrollToIterInPane(page,&page->match_start);
						page->iter=page->match_start;
					}
				else
					{
						if((wrapSearch==true) && (findInAllFiles==false))
							{
								gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->iter);
								if(gtk_source_iter_backward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
									{
										found=true;
										gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
										scrollToIterInPane(page,&page->match_start);
										page->iter=page->match_start;
									}
								gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
								g_free(searchtext);
								g_free(replacetext);
								return;
							}
					if((wrapSearch==true) || (findInAllFiles==true))
						doAllFiles(dowhat,found);
					}
			}

		if((dowhat==REPLACE) && (replaceAll==false))
			{
				if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
					{
						if(gtk_source_iter_forward_search(&page->match_start,searchtext,flags,&page->match_start,&page->match_end,&page->match_end))
							{
								gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
								gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&page->match_start,replacetext,-1);
							}
					}
				basicFind(FINDNEXT);
			}

		if((dowhat==REPLACE) && (findInAllFiles==true) && (replaceAll==true))
			{
				findInAllFiles=false;
				for(int j=0;j<gtk_notebook_get_n_pages(notebook);j++)
					{
						gtk_notebook_set_current_page(notebook,j);
						basicFind(REPLACE);
					}
				findInAllFiles=true;
			}

		if((dowhat==REPLACE) && (replaceAll==true))
			{
				if(replaceAll==true)
					{
						replaceAllFlag=true;
						gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
						gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&page->iter);
						if(gtk_source_iter_forward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
							{
								gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
								page->iter=page->match_end;
							}
						else
							replaceAllFlag=false;
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

			}

	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
	g_free(searchtext);
	g_free(replacetext);
}

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data)
{
	if(useRegex==false)
		basicFind(response_id);
	else
		regexFind(response_id);
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
				button=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACE);
				if(replaceAll==false)
					gtk_button_set_label((GtkButton*)button,"Replace");
				else
					gtk_button_set_label((GtkButton*)button,"Replace All");
				break;
			case 4:
				findInAllFiles=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 5:
				useRegex=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				button=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,FINDPREV);
				gtk_widget_set_sensitive(button,!useRegex);
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








