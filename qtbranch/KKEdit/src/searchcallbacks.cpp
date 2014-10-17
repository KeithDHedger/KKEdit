/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * functioncallbacks.cpp
 *
*/

#include "kkedit-includes.h"

int currentFindPage=-1;
int firstPage=-1;
int pagesChecked=0;
int	itemsReplaced=-1;

#ifdef _BUILDDOCVIEWER_

void webKitGoBack(Widget* widget,gpointer data)
{
	webkit_web_view_go_back((WebKitWebView*)data);
}

void webKitGoForward(Widget* widget,gpointer data)
{
	webkit_web_view_go_forward((WebKitWebView*)data);
}

void webKitGoHome(Widget* widget,gpointer data)
{
	if(g_file_test(htmlFile,G_FILE_TEST_EXISTS)==true)
		webkit_web_view_load_uri((WebKitWebView*)data,htmlURI);
}
#endif

PROTECTED void showDocView(int howtodisplay,char* text,const char* title)
{

#ifdef _BUILDDOCVIEWER_

	gtk_window_set_title((GtkWindow*)docView,title);

	if(howtodisplay==USEURI)
		{
			if(strcasecmp(thePage,"file://(null)")==0)
				{
					debugFree(thePage,"showDocView thePage");
					asprintf(&thePage,"https://www.google.co.uk/search?q=%s",text);
				}
			webkit_web_view_load_uri(webView,thePage);
		}

	if(howtodisplay==USEFILE)
		webkit_web_view_load_uri(webView,htmlURI);

	gtk_widget_show_all(docView);
	gtk_window_present((GtkWindow*)docView);

	showHideDocviewer=true;
	gtk_menu_item_set_label((GtkMenuItem*)showDocViewWidget,gettext("Hide Docviewer"));

#else
	char*		command;
	command=NULL;
	if(howtodisplay==USEURI)
		{
			if(strcasecmp(thePage,"file://(null)")!=0)
				asprintf(&command,"%s %s &",browserCommand,thePage);
			else
				asprintf(&command,"%s https://www.google.co.uk/search?q=%s",browserCommand,text);
		}

	if(howtodisplay==USEFILE)
		asprintf(&command,"%s %s",browserCommand,htmlURI);

	if(command!=NULL)
		{
			system(command);
			debugFree(command,"showDocView command");
		}
#endif

	if(thePage!=NULL)
		debugFree(thePage,"showDocView thePage");
	thePage=NULL;
		
	return;
}

VISIBLE void searchGtkDocs(Widget* widget,gpointer data)
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
		selection=strdup((char*)data);
	else
		{
			if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
				selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
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
							funcname=globalSlice->sliceBetween(line,(char*)"name=\"",(char*)"\" link=");
							if(funcname!=NULL)
								{
									if(strstr(funcname,selection)!=NULL)
										{
											if(cnt<2048)
												{
													tempstr=globalSlice->sliceBetween(line,(char*)"",(char*)":");
													if(tempstr!=NULL)
														{
															foldername=g_path_get_dirname(tempstr);
															link=globalSlice->sliceBetween(line,(char*)"link=\"",(char*)"\"");
															if((foldername!=NULL) && (link!=NULL))
																{
																	searchdata[cnt][0]=strdup(funcname);
																	asprintf(&searchdata[cnt][1],"%s/%s",foldername,link);
																	debugFree(foldername,"seachGtkDocs foldername");
																	debugFree(link,"seachGtkDocs link");
																	cnt++;
																}
															debugFree(tempstr,"seachGtkDocs tempstr");
														}
												}
										}
									debugFree(funcname,"seachGtkDocs funcname");
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

			showDocView(USEURI,selection,gettext("Gtk Docs"));
		}

	for(int loop=0;loop<cnt;loop++)
		{
			if(searchdata[loop][0]!=NULL)
				debugFree(searchdata[loop][0],"seachGtkDocs searchdata[loop][0]");
			if(searchdata[loop][1]!=NULL)
				debugFree(searchdata[loop][1],"seachGtkDocs searchdata[loop][1]");
		}
	if((selection!=NULL) && (data==NULL))
		debugFree(selection,"seachGtkDocs selection");
}

VISIBLE void doDoxy(Widget* widget,long data)
{
	pageStruct*	page=getPageStructPtr(-1);
	struct stat	sb;
	bool		dorebuild;
	FILE*		fp;
	char		line[1024];

	if(data==1)
		dorebuild=true;
	else
		dorebuild=false;

	if(page==NULL)
		return;

	showBarberPole(gettext("Building Documentaion ..."));
	chdir(page->dirName);

	stat("./html/index.html",&sb);
	if(!S_ISREG(sb.st_mode))
		dorebuild=true;

	stat("Doxyfile",&sb);
	if(!S_ISREG(sb.st_mode))
		{
			system("cp " DATADIR "/docs/Doxyfile .");
			dorebuild=true;
		}

	if(thePage!=NULL)
		debugFree(thePage,"doDoxy thePage");

	asprintf(&thePage,"file://%s/html/index.html",page->dirName);
	if(dorebuild==true)
		{
			fp=popen("doxygen Doxyfile","r");
			while(fgets(line,1024,fp))
				gtk_main_iteration_do(false);
			pclose(fp);
		}
	showDocView(USEURI,thePage,"Doxygen Documentation");
	killBarberPole();
}

//find in doxy docs
VISIBLE void doxyDocs(Widget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char*		findcommand=NULL;
	char*		headcommand=NULL;
	char		line[1024];
	char		titleline[1024];
	FILE*		findfile;
	FILE*		headfile;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);

	if(selection!=NULL)
		{
			showBarberPole(gettext("Searching ..."));

			asprintf(&findcommand,"find %s/html -name \"*.html\"",page->dirName);
			findfile=popen(findcommand,"r");
			while(fgets(line,1024,findfile))
				{
					gtk_main_iteration_do(false);
					line[strlen(line)-1]=0;
					asprintf(&headcommand,"head \"%s\"|grep title",line);
					headfile=popen(headcommand,"r");
					while(fgets(titleline,1024,headfile))
						{
							if(strstr(titleline,selection)!=NULL)
								{
									if(thePage!=NULL)
										debugFree(thePage,"doxyDocs thePage");
									asprintf(&thePage,"file://%s",line);
									while(fgets(titleline,1024,headfile));
									while(fgets(titleline,1024,findfile));
									pclose(headfile);
									pclose(findfile);
									debugFree(headcommand,"doxyDocs headcommand 1");
									showDocView(USEURI,thePage,"Doxygen Documentation");
									killBarberPole();
									return;
								}
						}
					pclose(headfile);
				}
			pclose(findfile);
			if(headcommand!=NULL)
				debugFree(headcommand,"doxyDocs headcommand 2");
			debugFree(findcommand,"doxyDocs findcommand 2");
			killBarberPole();
		}
}

//showDocViewWidget
VISIBLE void searchQT5Docs(Widget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	FILE*		fp;
	FILE*		fd;
	char*		command=NULL;
	GString*	str;
	char		line[1024];
	char*		func=NULL;
	int			cnt=0;

	if(data!=NULL)
		selection=strdup((char*)data);
	else
		{
			if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
				selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
		}

	if(selection!=NULL)
		{
			str=g_string_new(selection);
			str=g_string_ascii_down(str);

			asprintf(&command,"find %s -maxdepth 2 -iname \"%s*.html\"|sed 's/.html$//'|sort",QT5DOCSDIR,str->str);

			fd=fopen(htmlFile,"w");
			if(fd!=NULL)
				{								
					fprintf(fd,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
					fprintf(fd,"<html>\n");
					fprintf(fd,"<body>\n");

					fp=popen(command,"r");
					while(fgets(line,1024,fp))
						{
							func=g_path_get_basename(line);
							if((func !=NULL) && (strcasecmp(func,"")!=0))
								{
									cnt++;
									fprintf(fd,"<a href=\"%s.html\">%s</a><br>\n",line,func);
								}
						}
					fprintf(fd,"</body>\n");
					fprintf(fd,"</html>\n");
					fclose(fd);
					fclose(fp);
					debugFree(command,"searchQT5Docs command");
				}
			if(cnt==0)
				asprintf(&thePage,"file://(null)");
			else
				thePage=strdup(htmlURI);

			showDocView(USEURI,(char*)str->str,gettext("Qt5 Docs"));

			g_string_free(str,true);
			debugFree(selection,"searchQT5Docs selection");
		}
}

void defSearchFromBar(Widget* widget,gpointer data)
{
	functionData* fdata;

	functionSearchText=strdup(gtk_entry_get_text((GtkEntry*)widget));
	if(functionSearchText!=NULL)
		{
			fdata=getFunctionByName(functionSearchText,true);
			if(fdata!=NULL)
				{
					goToDefine(fdata);
					destroyData(fdata);
				}
			debugFree(functionSearchText,"defSearchFromBar functionSearchText");
		}
}

void docSearchFromBar(Widget* widget,gpointer data)
{
	const char* text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		searchGtkDocs(NULL,(void*)text);
}

#ifdef _BUILDDOCVIEWER_
void docSearchInPageFoward(Widget* widget,gpointer data)
{
	const char* text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		webkit_web_view_search_text(webView,text,false,true,true);
}

void docSearchInPageBack(Widget* widget,gpointer data)
{
	const char* text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		webkit_web_view_search_text(webView,text,false,false,true);
}
#endif

void qt5DocSearchFromBar(Widget* widget,gpointer data)
{
	const char* text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		searchQT5Docs(NULL,(void*)text);
}

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
	GtkTextIter				hastart,haend;
	int						matchedcnt=-1;

	if(gtk_entry_get_text_length((GtkEntry*)findBox)==0)
		return;

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

				gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&hastart);
				gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&haend);
				gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&hastart,&haend);  

				if(hightlightAll==true)
					{
						g_regex_match_full(regex,text,-1,0,matchflags,&match_info,NULL);
						while (g_match_info_matches(match_info))
							{

								g_match_info_fetch_pos(match_info,0,&startpos,&endpos);
								charstartpos=g_utf8_pointer_to_offset(text,&text[startpos]);
								charendpos=g_utf8_pointer_to_offset(text,&text[endpos]);
								gtk_text_iter_set_offset(&hastart,charstartpos);
								haend=hastart;
								gtk_text_iter_set_offset(&haend,charendpos);
								gtk_text_buffer_apply_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&hastart,&haend);
								g_match_info_next (match_info,NULL);
							}
					}

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
						if(yesNo((char*)gettext("Do you want to replace in ALL open files?"),(char*)"")==GTK_RESPONSE_YES)
							{
								matchedcnt=-1;
								for(pageloop=0;pageloop<gtk_notebook_get_n_pages(notebook);pageloop++)
									{
										gtk_notebook_set_current_page(notebook,currentFindPage);
										tmppage=getPageStructPtr(pageloop);
										gtk_text_buffer_get_start_iter((GtkTextBuffer*)tmppage->buffer,&start);
										gtk_text_buffer_get_end_iter((GtkTextBuffer*)tmppage->buffer,&end);
										text=gtk_text_buffer_get_text((GtkTextBuffer*)tmppage->buffer,&start,&end,false);
	
										if(g_regex_match_full(regex,text,-1,0,matchflags,&match_info,NULL))
											{
												while (g_match_info_matches(match_info))
													{
														matchedcnt++;
														g_match_info_next(match_info,NULL);
													}
												itemsReplaced=matchedcnt;
											}
										g_match_info_free(match_info);

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
												debugFree(text,"regexFind text");
												text=NULL;
											}
										if(reptext!=NULL)
											{
												debugFree(reptext,"regexFind reptext");
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

						if(g_regex_match_full(regex,text,-1,0,matchflags,&match_info,NULL))
							{
								matchedcnt=-1;
								while (g_match_info_matches(match_info))
									{
										matchedcnt++;
										g_match_info_next(match_info,NULL);
									}
								itemsReplaced=matchedcnt;
							}
						g_match_info_free(match_info);

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
								if((reptext!=NULL) && (strcmp(reptext,text)!=0))
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
		debugFree(text,"regexFind text");
	if(reptext!=NULL)
	debugFree(reptext,"regexFind reptext");
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
	GtkTextIter				start_find,end_find;
	GtkTextIter				start_match,end_match;
	int						offset;

	if(gtk_entry_get_text_length((GtkEntry*)findBox)==0)
		return;

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

	start_match=page->match_start;
	end_match=page->match_end;

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer, &start_find);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer, &end_find);
	gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&start_find, &end_find);  

	if(hightlightAll==true)
		{
			while(gtk_source_iter_forward_search(&start_find,searchtext,flags,&start_match,&end_match,NULL))
				{
					gtk_text_buffer_apply_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&start_match,&end_match);
					offset=gtk_text_iter_get_offset(&end_match);
					gtk_text_buffer_get_iter_at_offset((GtkTextBuffer*)page->buffer,&start_find,offset);
				}
		}

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
							debugFree(searchtext,"basicFind searchtext");
							debugFree(replacetext,"basicFind replacetext");
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
								debugFree(searchtext,"basicFind searchtext");
								debugFree(replacetext,"basicFind replacetext");
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
				if(yesNo((char*)gettext("Do you want to replace in ALL open files?"),(char*)"")==GTK_RESPONSE_CANCEL)
					return;
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
										itemsReplaced++;
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
									debugFree(selectedtext,"basicFind selectedtext");
							}
					}
				while(replaceAllFlag==true);

			}

	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
	debugFree(searchtext,"basicFind searchtext");
	debugFree(replacetext,"basicFind replacetext");
}

void pasteFRClip(Widget* widget,gpointer data)
{
	gtk_entry_set_text((GtkEntry*)data,gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget));
}

void showOnStatus(const char* from,const char* to)
{
	char*	message=NULL;

	if( (showStatus==false))
			return;

	gtk_statusbar_pop((GtkStatusbar*)statusWidget,0);
	asprintf(&message,gettext("Replaced %i instances of '%s' with '%s'"),itemsReplaced+1,from,to);
	gtk_statusbar_pop((GtkStatusbar*)statusWidget,0);
	gtk_statusbar_push((GtkStatusbar*)statusWidget,0,message);
	statusMessage=message;
}

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data)
{
	bool		flag=false;
	GSList*		tlist;
	const char*	edata;
	Widget*	entry;
	Widget*	drop;
	GSList*		list;

	if(response_id!=REPLACE)
		{
			drop=findDropBox;
			entry=findBox;
			list=findList;
		}
	else
		{
			entry=replaceBox;
			drop=replaceDropBox;
			list=replaceList;
		}

	itemsReplaced=-1;

	edata=gtk_entry_get_text((GtkEntry*)entry);

	if(list==NULL)
		{
			list=g_slist_append(list,strdup(edata));
			gtk_combo_box_text_append_text((GtkComboBoxText*)drop,(const char*)list->data);
			gtk_combo_box_set_active((GtkComboBox*)drop,0);
		}
	else
		{
			tlist=list;
			flag=false;
			do
				{
					if(strcmp((const gchar*)tlist->data,edata)==0)
						flag=true;
					tlist=tlist->next;
				}
			while(tlist!=NULL);

			if(flag==false)
				{
					list=g_slist_prepend(list,strdup(edata));
					gtk_combo_box_text_append_text((GtkComboBoxText*)drop,(const char*)list->data);
					gtk_combo_box_set_active((GtkComboBox*)drop,g_slist_length(list)-1);
				}
		}

	if(response_id!=REPLACE)
		findList=list;
	else
		replaceList=list;

	if(useRegex==false)
		basicFind(response_id);
	else
		regexFind(response_id);

	if(itemsReplaced>-1)
		showOnStatus(gtk_entry_get_text((GtkEntry*)findBox),gtk_entry_get_text((GtkEntry*)replaceBox));
}

VISIBLE void find(Widget* widget,gpointer data)
{
	gtk_widget_show(findReplaceDialog);
	gtk_dialog_run((GtkDialog *)findReplaceDialog);
}

void doSearchPrefs(Widget* widget,gpointer data)
{
	Widget*	button;

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
					gtk_button_set_label((GtkButton*)button,gettext("Replace"));
				else
					gtk_button_set_label((GtkButton*)button,gettext("Replace All"));
				break;
			case 4:
				findInAllFiles=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 5:
				useRegex=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				button=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,FINDPREV);
				gtk_widget_set_sensitive(button,!useRegex);
				break;
			case 6:
				hightlightAll=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
		}
}

void doLiveSearch(Widget* widget,GdkEvent *event,gpointer data)
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