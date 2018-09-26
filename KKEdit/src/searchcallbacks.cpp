/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:50:31 GMT 2015 kdhedger68713@gmail.com

 * This file(searchcallbacks.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kkedit-includes.h"

struct regexData
{
	int					start;
	int					end;
};

int						currentFindPage=-1;
int						firstPage=-1;
int						pagesChecked=0;
int						itemsReplaced=-1;
pageListData			*pageData=NULL;
bool					fromregexreplace=false;
bool					gotselection=false;
bool					fromregexsinglereplace=false;
bool					wrapFlag=false;
unsigned				currentpageid=0;
bool					finishedpage=false;
unsigned				startchar=0;
unsigned				oldstartchar=0;
#ifdef _USEGTK3_
GtkTextSearchFlags		flags=GTK_TEXT_SEARCH_TEXT_ONLY;
#else
GtkSourceSearchFlags	flags=GTK_SOURCE_SEARCH_TEXT_ONLY;
#endif

char					*searchtext=NULL;
char					*replacetext=NULL;

#ifdef _BUILDDOCVIEWER_

void webKitGoBack(GtkWidget *widget,gpointer data)
{
	webkit_web_view_go_back((WebKitWebView*)data);
}

void webKitGoForward(GtkWidget *widget,gpointer data)
{
	webkit_web_view_go_forward((WebKitWebView*)data);
}

void webKitGoHome(GtkWidget *widget,gpointer data)
{
	if(g_file_test(htmlFile,G_FILE_TEST_EXISTS)==true)
		webkit_web_view_load_uri((WebKitWebView*)data,htmlURI);
}
#endif

PROTECTED void showDocView(int howtodisplay,char *text,const char *title)
{

#ifdef _BUILDDOCVIEWER_
	if(inWindow==true)
		gtk_window_set_title((GtkWindow*)docView,title);

	if(howtodisplay==USEURI)
		{
			if(strcasecmp(thePage,"file://(null)")==0)
				{
					if(doGoogleSearch==true)
						{
							if(inWindow==true)
								gtk_window_set_title((GtkWindow*)docView,DOCVIEW_SEARCHING_GOOGLE_LABEL);
							ERRDATA debugFree(&thePage);
							sinkReturn=asprintf(&thePage,"https://www.google.co.uk/search?q=%s",text);
						}
					else
						return;
				}
			webkit_web_view_load_uri(webView,thePage);		
		}
	if(howtodisplay==USEFILE)
		webkit_web_view_load_uri(webView,htmlURI);

	gtk_widget_show_all(docView);
	if(inWindow==true)
		gtk_window_present((GtkWindow*)docView);
	else
		gtk_notebook_set_current_page(mainNotebook,gtk_notebook_page_num(mainNotebook,docView));

	showHideDocviewer=true;
	gtk_menu_item_set_label((GtkMenuItem*)showDocViewWidget,MENU_HIDE_DOCVIEWER_LABEL);

#else
	char		*command;
	command=NULL;
	if(howtodisplay==USEURI)
		{
			if(strcasecmp(thePage,"file://(null)")!=0)
				sinkReturn=asprintf(&command,"%s %s &",browserCommand,thePage);
			else
				sinkReturn=asprintf(&command,"%s https://www.google.co.uk/search?q=%s",browserCommand,text);
		}

	if(howtodisplay==USEFILE)
		sinkReturn=asprintf(&command,"%s %s",browserCommand,htmlURI);

	if(command!=NULL)
		{
			sinkReturn=system(command);
			ERRDATA debugFree(&command);
		}
#endif

	if(thePage!=NULL)
		{
			ERRDATA debugFree(&thePage);
		}
	thePage=NULL;
	return;
}

VISIBLE bool searchGtkDocs(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=getPageStructByIDFromPage(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char		*selection=NULL;
	char		*searchdata[2048][2];
	char		line[1024];
	FILE		*fp;
	FILE		*fd;
	char		*command=NULL;
	char		*ptr=NULL;
	char		*funcname;
	char		*foldername;
	char		*tempstr;
	char		*link;
	int			cnt=0;
	char		ds[2];
	bool		retval=true;

	ERRDATA

	ds[0]=0xc2;
	ds[1]=0;

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
			sinkReturn=asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %s '{}' \\;",selection);
			fp=popen(command,"r");
			while(fgets(line,1024,fp))
				{
					ptr=strstr(line,"name=\"");
					if(ptr!=NULL)
						{
							funcname=globalSlice->sliceBetween(line,(char*)"name=\"",(char*)"\" link=");
							if(globalSlice->getResult()==0)
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
															if((foldername!=NULL) &&(link!=NULL))
																{
																	searchdata[cnt][0]=strdup(globalSlice->deleteSlice(funcname,ds));
																	sinkReturn=asprintf(&searchdata[cnt][1],"%s/%s",foldername,link);
																	ERRDATA debugFree(&foldername);
																	ERRDATA debugFree(&link);
																	cnt++;
																}
															ERRDATA debugFree(&tempstr);
														}
												}
										}
									ERRDATA debugFree(&funcname);
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
					sinkReturn=asprintf(&thePage,"file://%s",searchdata[0][1]);
				}
			if(strcmp(thePage,"file://(null)")==0)
				retval=false;
			showDocView(USEURI,selection,DOCVIEW_GTK_DOCS_LABEL);
		}

	for(int loop=0;loop<cnt;loop++)
		{
			if(searchdata[loop][0]!=NULL)
				{
					ERRDATA debugFree(&searchdata[loop][0]);
				}
			if(searchdata[loop][1]!=NULL)
				{
					ERRDATA debugFree(&searchdata[loop][1]);
				}
		}
	return(retval);
}

VISIBLE void doDoxy(GtkWidget *widget,long data)
{
	pageStruct	*page=getPageStructByIDFromPage(-1);
	struct stat	sb;
	bool		dorebuild;
	FILE		*fp;
	char		line[1024];

	if(data==1)
		dorebuild=true;
	else
		dorebuild=false;

	if(page==NULL)
		{
			ERRDATA return;
		}

	showBarberPole(DIALOG_POLE_BUILDING_LABEL);
	sinkReturn=chdir(page->dirName);

	stat("./html/index.html",&sb);
	if(!S_ISREG(sb.st_mode))
		dorebuild=true;

	stat("Doxyfile",&sb);
	if(!S_ISREG(sb.st_mode))
		{
			sinkReturn=system("cp " DOXYDOCFILE " .");
			dorebuild=true;
		}

	if(thePage!=NULL)
		{
			ERRDATA debugFree(&thePage);
		}

	sinkReturn=asprintf(&thePage,"file://%s/html/index.xhtml",page->dirName);
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
VISIBLE void doxyDocs(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=getPageStructByIDFromPage(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char		*selection=NULL;
	char		*findcommand=NULL;
	char		line[1024];
	FILE		*findfile;
	int			cnt=0;
	char		*searchdata[2048][2];
	FILE		*fd;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);

	if(selection!=NULL)
		{
			for(int loop=0;loop<2048;loop++)
				{
					searchdata[loop][0]=NULL;
					searchdata[loop][1]=NULL;
				}

			sinkReturn=asprintf(&findcommand,"cat %s/searchdata.xml|grep 'name=\"name\".*%s.*field'|sed 's/^[ \\t]*<field name=\"name\">//;s/<\\/field>//'",page->dirName,selection);

			findfile=popen(findcommand,"r");
			while(fgets(line,1024,findfile))
				{
					searchdata[cnt][0]=strdup(line);
					cnt++;
				}
			pclose(findfile);
			ERRDATA debugFree(&findcommand);

			cnt=0;
			sinkReturn=asprintf(&findcommand,"cat %s/searchdata.xml|grep -C 2 'name=\"name\".*%s.*field'|grep -i url|sed 's/^[ \\t]*<field name=\"url\">//;s/<\\/field>//'",page->dirName,selection);
			findfile=popen(findcommand,"r");
			while(fgets(line,1024,findfile))
				{
					sinkReturn=asprintf(&searchdata[cnt][1],"%s/html/%s",page->dirName,line);
					cnt++;
				}
			pclose(findfile);
			ERRDATA debugFree(&findcommand);

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
					if(thePage!=NULL)
						ERRDATA debugFree(&thePage);
					sinkReturn=asprintf(&thePage,"file://%s",searchdata[0][1]);
				}

			showDocView(USEURI,selection,DOCVIEW_DOXY_DOCS_LABEL);

			for(int loop=0;loop<cnt;loop++)
				{
					if(searchdata[loop][0]!=NULL)
						ERRDATA debugFree(&searchdata[loop][0]);

					if(searchdata[loop][1]!=NULL)
						ERRDATA debugFree(&searchdata[loop][1]);
				}

			if((selection!=NULL) &&(data==NULL))
				ERRDATA debugFree(&selection);
		}
}

//searc qt5 aasistant
void searchQT5Assist(GtkWidget *widget,gpointer data)
{
	printf("search qt5 assistant\n");
}

//showDocViewWidget
VISIBLE bool searchQT5Docs(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=getPageStructByIDFromPage(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char		*selection=NULL;
	FILE		*fp;
	FILE		*fd;
	char		*command=NULL;
	GString		*str;
	char		line[1024];
	char		*func=NULL;
	int			cnt=0;
	bool		retval=true;

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

			sinkReturn=asprintf(&command,"find %s -maxdepth 2 -iname \"%s*.html\"|sed 's/.html$//'|sort",QT5DOCSDIR,str->str);

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
							if((func !=NULL) &&(strcasecmp(func,"")!=0))
								{
									cnt++;
									fprintf(fd,"<a href=\"%s.html\">%s</a><br>\n",line,func);
								}
						}
					fprintf(fd,"</body>\n");
					fprintf(fd,"</html>\n");
					fclose(fd);
					fclose(fp);
					ERRDATA debugFree(&command);
				}
			if(cnt==0)
				{
					retval=false;
					sinkReturn=asprintf(&thePage,"file://(null)");
				}
			else
				thePage=strdup(htmlURI);

			showDocView(USEURI,(char*)str->str,DOCVIEW_QT5_DOCS_LABEL);

			g_string_free(str,true);
			ERRDATA debugFree(&selection);
		}
	return(retval);
}

bool defSearchFromBar(GtkWidget *widget,gpointer data)
{
	functionData	*fdata=NULL;
	bool			searchcase;
	bool			searchwhole;

	if(data!=NULL)
		functionSearchText=strdup(gtk_entry_get_text((GtkEntry*)widget));
	else
		functionSearchText=strdup((char*)widget);

	if(functionSearchText!=NULL)
		{
			searchwhole=true;
			for(int k=0;k<2;k++)
				{
					searchcase=true;
					for(int j=0;j<2;j++)
						{
							fdata=getFunctionByNameOpenFiles(functionSearchText,searchcase,searchwhole);
							if(fdata!=NULL)
								{
									globalHistory->saveLastPosAndStop();
									goToDefine(fdata);
									destroyData(fdata);
									ERRDATA debugFree(&functionSearchText);
									globalHistory->redoMenus();
									ERRDATA return(true);
								}
							searchcase=false;
						}
					searchwhole=false;
				}

			for(int j=0;j<2;j++)
				{
					fdata=getFunctionByName(functionSearchText,true,searchcase);
					if(fdata!=NULL)
						{
							globalHistory->saveLastPosAndStop();
							goToDefine(fdata);
							destroyData(fdata);
							ERRDATA debugFree(&functionSearchText);
							globalHistory->redoMenus();
							ERRDATA return(true);
						}
					searchcase=false;
				}
			ERRDATA debugFree(&functionSearchText);
		}
	globalHistory->redoMenus();
	return(false);
}

void docSearchFromBar(GtkWidget *widget,gpointer data)
{
	const char *text=gtk_entry_get_text((GtkEntry*)data);
	if(text!=NULL && strlen(text)>0)
		searchGtkDocs(NULL,(void*)text);
}

#ifdef _BUILDDOCVIEWER_
void docSearchInPageFoward(GtkWidget *widget,gpointer data)
{
	const char *text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		webkit_web_view_search_text(webView,text,false,true,true);
}

void docSearchInPageBack(GtkWidget *widget,gpointer data)
{
	const char *text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		webkit_web_view_search_text(webView,text,false,false,true);
}
#endif

void qt5DocSearchFromBar(GtkWidget *widget,gpointer data)
{
	const char *text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		searchQT5Docs(NULL,(void*)text);
}

int findNextRegex(pageStruct *page,int charpos,int thisregxnum)
{
	int			retval=-1;
	regexData	*xdata;

	for(int j=thisregxnum;j<(int)g_slist_length(page->regexList);j++)
		{
			xdata=(regexData*)g_slist_nth_data(page->regexList,j);
			if(xdata->start>charpos)
				{
					retval=j;
					ERRDATA return(j);
				}
		}
	ERRDATA return(retval);
}

int findPrevRegex(pageStruct *page,int charpos)
{
	int retval=-1;
	regexData				*xdata;

	for(int j=g_slist_length(page->regexList)-1;j>=0;j--)
		{
			xdata=(regexData*)g_slist_nth_data(page->regexList,j);
			if(xdata->start<charpos)
				{
					retval=j;
					ERRDATA return(j);
				}
		}
	ERRDATA return(retval);
}

int findThisRegex(pageStruct *page,int charpos)
{
	int			retval=-1;
	regexData	*xdata;

	for(int j=0;j<(int)g_slist_length(page->regexList);j++)
		{
			xdata=(regexData*)g_slist_nth_data(page->regexList,j);
			if(xdata->start==charpos)
				{
					retval=j;
					ERRDATA return(j);
				}
		}
	ERRDATA return(retval);
}

void regexFind(int dowhat)
{
	GRegex					*regex;
	GMatchInfo				*match_info=NULL;
	GRegexCompileFlags		compileflags=(GRegexCompileFlags)(G_REGEX_MULTILINE|G_REGEX_EXTENDED);
	GRegexMatchFlags		matchflags=(GRegexMatchFlags)0;
	pageStruct				*page=NULL;
	regexData				*xdata;
	GtkTextIter				startiter,enditer;
	int						charstartpos;
	int						charendpos;
	int						startpos=0,endpos=0;
	char					*text=NULL;
	char					*reptext=NULL;
	TextBuffer				*textbuffer=NULL;
	bool					dofindnext=false;
	bool					dofindprev=false;
	int						testformatch;
	GtkTextIter				hastart,haend;
	GError					*regexError=NULL;
	int						newdowhat;

	if(searchBack==true)
		newdowhat=FINDPREV;
	else
		newdowhat=dowhat;
	page=getPageStructByIDFromPage(currentFindPage);
	if(page==NULL)
		{
			ERRDATA return;
		}
	if(gtk_entry_get_text_length((GtkEntry*)findBox)==0)
		{
			ERRDATA return;
		}

	gotselection=gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&startiter,&enditer);

	if(gotselection==false ||((fromregexsinglereplace==true) &&(gotselection==true)))
		{
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
		}

	if(replaceAll==true)
		{
			if(gotselection==true)
				{
					if(fromregexsinglereplace==true)
						{
							gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
							gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
						}
				}
		}

	textbuffer=new TextBuffer((GtkTextBuffer*)page->buffer);

	if(searchtext!=NULL)
		{
			if((strcmp(searchtext,gtk_entry_get_text((GtkEntry*)findBox))!=0) ||(page->regexMatchNumber==-1))
				page->regexMatchNumber=-1;
			ERRDATA debugFree(&searchtext);
		}

	if(replacetext!=NULL)
		{
			if((strcmp(replacetext,gtk_entry_get_text((GtkEntry*)replaceBox))!=0) ||(page->regexMatchNumber==-1))
				page->regexMatchNumber=-1;
			ERRDATA debugFree(&replacetext);
		}

	searchtext=strdup((char*)gtk_entry_get_text((GtkEntry*)findBox));
	replacetext=strdup((char*)gtk_entry_get_text((GtkEntry*)replaceBox));

	if(insensitiveSearch==true)
		compileflags=(GRegexCompileFlags)(compileflags|G_REGEX_CASELESS);

	regex=g_regex_new(searchtext,(GRegexCompileFlags)compileflags,matchflags,&regexError);
	if(gerrorMessage(&regexError)!=0)
		return;

	text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false);

	if(page->regexMatchNumber==-1)
		{
			if(page->regexList!=NULL)
				{
					g_slist_free_full(page->regexList,free);
					page->regexList=NULL;
				}

			gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&startiter,&enditer);
			g_regex_match_full(regex,text,-1,0,matchflags,&match_info,NULL);
			while(g_match_info_matches(match_info))
				{
					if(g_match_info_fetch_pos(match_info,0,&startpos,&endpos))
						{
							charstartpos=g_utf8_pointer_to_offset(text,&text[startpos]);
							charendpos=g_utf8_pointer_to_offset(text,&text[endpos]);
							xdata=(regexData*)malloc(sizeof(regexData));
							xdata->start=charstartpos;
							xdata->end=charendpos;
							page->regexList=g_slist_append(page->regexList,xdata);
							if(hightlightAll==true)
								{	
									gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&hastart);
									gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&haend);
									gtk_text_iter_set_offset(&hastart,charstartpos);
									gtk_text_iter_set_offset(&haend,charendpos);
									gtk_text_buffer_apply_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&hastart,&haend);
								}
						}
					g_match_info_next(match_info,NULL);
				}
			page->regexMatchNumber=0;
		}

	switch(newdowhat)
		{
//forward search
			case FINDNEXT:
				fromregexreplace=false;
				page->regexMatchNumber=findNextRegex(page,textbuffer->charPos-1,page->regexMatchNumber);
				if(page->regexMatchNumber!=-1)
					{
						xdata=(regexData*)g_slist_nth_data(page->regexList,page->regexMatchNumber);
						if(xdata!=NULL)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_iter_set_offset(&startiter,xdata->start);
								gtk_text_iter_set_offset(&enditer,xdata->end);
								gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&startiter,&enditer);
								scrollToIterInPane(page,&startiter);
								page->regexMatchNumber++;
							}
					}
				else
					{
						if(findInAllFiles==true)
							{
								page->regexMatchNumber=-1;
								currentFindPage++;
								pagesChecked++;
								if(currentFindPage>=gtk_notebook_get_n_pages(mainNotebook))
									currentFindPage=0;
								gtk_notebook_set_current_page(mainNotebook,currentFindPage);
								page=getPageStructByIDFromPage(currentFindPage);
								if(page!=NULL)
									{
										page->regexMatchNumber=-1;
										dofindnext=true;
										gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
										gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&startiter);

										if(pagesChecked>gtk_notebook_get_n_pages(mainNotebook))
											{
												dofindnext=false;
												pagesChecked=0;
											}
									}
							}
						else if(wrapSearch==true)
							{
								testformatch=findNextRegex(page,0,0);
								if(testformatch!=-1)
									dofindnext=true;
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&startiter);
							}
					}
				break;

			case FINDPREV:
				fromregexreplace=false;
				page->regexMatchNumber=findPrevRegex(page,textbuffer->charPos);
				if(page->regexMatchNumber!=-1)
					{
						xdata=(regexData*)g_slist_nth_data(page->regexList,page->regexMatchNumber);
						if(xdata!=NULL)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_iter_set_offset(&startiter,xdata->start);
								gtk_text_iter_set_offset(&enditer,xdata->end);
								gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&startiter,&enditer);
								scrollToIterInPane(page,&startiter);
								page->regexMatchNumber--;
								if(page->regexMatchNumber==-1)
									page->regexMatchNumber=0;
							}
					}
				else
					{
						if(findInAllFiles==true)
							{
								page->regexMatchNumber=-1;
								currentFindPage--;
								if(currentFindPage<0)
									currentFindPage=gtk_notebook_get_n_pages(mainNotebook)-1;
								gtk_notebook_set_current_page(mainNotebook,currentFindPage);
								page=getPageStructByIDFromPage(currentFindPage);
								if(page!=NULL)
									{
										page->regexMatchNumber=-1;
										dofindprev=true;
										gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
										gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&enditer);
										scrollToIterInPane(page,&enditer);
									}
							}
						else if(wrapSearch==true)
							{
								testformatch=findNextRegex(page,0,0);
								if(testformatch!=-1)
									dofindprev=true;
								gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&enditer);
								scrollToIterInPane(page,&startiter);
							}
					}
				break;

			case REPLACENEXT:
				fromregexreplace=false;
				fromregexsinglereplace=false;
				if(replaceAll==true)
					{
						fromregexreplace=true;					
						int startloop,endloop;
						if(findInAllFiles==true)
							{
								if(yesNo((char*)DIALOG_YESNO_REPLACE_IN_ALL_FILES,(char*)"")==GTK_RESPONSE_CANCEL)
									{
										ERRDATA return;
									}
								startloop=0;
								endloop=gtk_notebook_get_n_pages(mainNotebook);
							}
						else
							{
								startloop=gtk_notebook_get_current_page(mainNotebook);
								endloop=gtk_notebook_get_current_page(mainNotebook)+1;
							}

						for(int j=startloop;j<endloop;j++)
							{
								page=getPageStructByIDFromPage(j);
								if(page!=NULL)
									{
								//if(findInAllFiles==true)
								//	{
										gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
										gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
								//	}
										moveBMsForPage(page,UPDATEBMLINE);
										text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false);
										regex=g_regex_new(searchtext,(GRegexCompileFlags)compileflags,(GRegexMatchFlags)matchflags,&regexError);
										if(gerrorMessage(&regexError)!=0)
											return;

										g_regex_match_full(regex,text,-1,0,matchflags,&match_info,NULL);
										while(g_match_info_matches(match_info))
											{
												itemsReplaced++;
												g_match_info_next(match_info,NULL);
											}

										reptext=g_regex_replace(regex,text,-1,0,replacetext,(GRegexMatchFlags)matchflags,&regexError);
										if(gerrorMessage(&regexError)!=0)
											return;

										if((reptext!=NULL) &&(strcmp(reptext,text)!=0))
											{
												gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);
													gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&startiter,&enditer);
													gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&startiter,reptext,-1);
													page->regexMatchNumber=-1;
													ERRDATA debugFree(&reptext);
												gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
											}
										ERRDATA debugFree(&text);
										moveBMsForPage(page,MOVEBM);
									}
							}
					}
				else
					{
						page->regexMatchNumber=findThisRegex(page,textbuffer->charPos);
						xdata=(regexData*)g_slist_nth_data(page->regexList,page->regexMatchNumber);
						if(xdata!=NULL)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_iter_set_offset(&startiter,xdata->start);
								gtk_text_iter_set_offset(&enditer,xdata->end);

								regex=g_regex_new(searchtext,(GRegexCompileFlags)(G_REGEX_CASELESS|G_REGEX_EXTENDED),(GRegexMatchFlags)0,&regexError);
								if(gerrorMessage(&regexError)!=0)
									return;

								text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false);
								reptext=g_regex_replace(regex,text,-1,0,replacetext,(GRegexMatchFlags)0,&regexError);
								if(gerrorMessage(&regexError)!=0)
									return;

								if((reptext!=NULL) &&(strcmp(reptext,text)!=0))
									{
										gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&startiter,&enditer);
										gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&startiter,reptext,-1);
										page->regexMatchNumber=-1;
										ERRDATA debugFree(&reptext);
										dofindnext=true;
										fromregexsinglereplace=true;
									}
								ERRDATA debugFree(&text);
							}
					}				
				break;
		}

	ERRDATA debugFree(&text);
	ERRDATA delete textbuffer;

	if(dofindnext==true)
		regexFind(FINDNEXT);
	if(dofindprev==true)
		regexFind(FINDPREV);
}

void showOnStatus(const char *from,const char *to)
{
	char	*message=NULL;

	if((showStatus==false))
		{
			ERRDATA return;
		}

	loadingSession=true;
	gtk_statusbar_remove_all ((GtkStatusbar*)statusWidget,0);
	sinkReturn=asprintf(&message,REPLACE_INFO_LABEL,itemsReplaced+1,from,to);
	gtk_statusbar_push((GtkStatusbar*)statusWidget,0,message);
	loadingSession=false;
}

bool searchForward(GtkTextIter *start_find,const char *srchtxt,long flags,GtkTextIter *start_match,GtkTextIter *end_match,GtkTextIter *limit)
{
#ifdef _USEGTK3_
	return(gtk_text_iter_forward_search(start_find,srchtxt,(GtkTextSearchFlags)flags,start_match,end_match,limit));
#else
	return(gtk_source_iter_forward_search(start_find,srchtxt,(GtkSourceSearchFlags)flags,start_match,end_match,limit));
#endif
}

bool searchBackward(GtkTextIter *start_find,const char *srchtxt,long flags,GtkTextIter *start_match,GtkTextIter *end_match,GtkTextIter *limit)
{
#ifdef _USEGTK3_
	return(gtk_text_iter_backward_search(start_find,srchtxt,(GtkTextSearchFlags)flags,start_match,end_match,limit));
#else
	return(gtk_source_iter_backward_search(start_find,srchtxt,(GtkSourceSearchFlags)flags,start_match,end_match,limit));
#endif
}

bool findInPage(bool forward)
{
	bool		retval=false;
	GtkTextIter	startiter;
	GtkTextIter	matchstartiter;
	GtkTextIter	matchenditer;
	int			pagenum;
	bool		found=false;


	if(searchPageStruct==NULL)
		return(true);

	gtk_text_buffer_get_iter_at_offset((GtkTextBuffer*)searchPageStruct->buffer,&startiter,startchar);
	if(forward==true)
		found=searchForward(&startiter,searchtext,flags,&matchstartiter,&matchenditer,NULL);
	else
		found=searchBackward(&startiter,searchtext,flags,&matchstartiter,&matchenditer,NULL);

	if(found==true)
		{
			gtk_text_buffer_select_range((GtkTextBuffer*)searchPageStruct->buffer,&matchstartiter,&matchenditer);
			pagenum=gtk_notebook_page_num(mainNotebook,searchPageStruct->tabVbox);

			gtk_notebook_set_current_page(mainNotebook,pagenum);
			scrollToIterInPane(searchPageStruct,&matchstartiter);
			
			if(forward==true)
				startchar=gtk_text_iter_get_offset(&matchenditer);
			else
				startchar=gtk_text_iter_get_offset(&matchstartiter);
			oldstartchar=getCurrentCursorPos();
		}
	else
		{
			pagenum=-1;
			retval=true;
		}

	return(retval);
}

void whatToDo(void)
{
	pageStruct	*page=NULL;
	bool		gotpage=false;
	unsigned	pageid=0;
	GtkTextIter	start;
	GtkTextIter	nulliter;
	bool		found=false;
	int			startfrom;
	int			endat;

	if(getCurrentCursorPos()!=oldstartchar)
		startchar=getCurrentCursorPos();

	found=false;
	if((findInAllFiles==true) & (searchBack==false))
		{
			do
				{
					startfrom=gtk_notebook_get_current_page(mainNotebook)+1;
					endat=gtk_notebook_get_n_pages(mainNotebook);
					if(startfrom>=endat)
						startfrom=0;
					for(int k=0;k<2;k++)
						{
							for(int j=startfrom;j<endat;j++)
								{
									page=getPageStructByIDFromPage(j);
									if(page!=NULL)
										{
											gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
											found=searchForward(&start,searchtext,flags,&nulliter,&nulliter,NULL);
											if(found==true)
												{
													gotpage=true;
													startchar=0;
													searchPageStruct=page;
													currentpageid=searchPageStruct->pageID;
													return;
												}
										}
								}
							endat=startfrom-1;
							startfrom=0;
						}					
					gotpage=true;
				}
			while(gotpage==false);
		}

	found=false;
	if((findInAllFiles==true) & (searchBack==true))
		{
			do
				{
					startfrom=gtk_notebook_get_current_page(mainNotebook)-1;
					endat=0;
					if(startfrom<0)
						startfrom=gtk_notebook_get_n_pages(mainNotebook);
					for(int k=0;k<2;k++)
						{
							for(int j=startfrom;j>=endat;j--)
								{
									page=getPageStructByIDFromPage(j);
									if(page!=NULL)
										{
											gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&start);
											found=searchBackward(&start,searchtext,flags,&nulliter,&nulliter,NULL);
											if(found==true)
												{
													gotpage=true;
													startchar=0;
													startchar=getCharacterPos(LASTCHAR,(GtkTextBuffer*)page->buffer);
													searchPageStruct=page;
													currentpageid=searchPageStruct->pageID;
													return;
												}
										}
								}
							endat=startfrom;
							startfrom=gtk_notebook_get_n_pages(mainNotebook);
						}					
					gotpage=true;
				}
			while(gotpage==false);
		}

	if(wrapSearch==true)
		{
			if(searchBack==false)
				startchar=0;
			else
				startchar=getCharacterPos(LASTCHAR,NULL);
		}
}

void replaceAllTextInBuffer(pageStruct *page)
{
	char		*txt;
	char		*newtxt;
	GtkTextIter	startiter;
	GtkTextIter	enditer;
	StringSlice	*mystr;

	loadingSession=true;
	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);
	moveBMsForPage(page,UPDATEBMLINE);
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
	txt=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false);
	mystr=new StringSlice;
	mystr->setCaseless(insensitiveSearch);
	newtxt=mystr->replaceAllSlice(txt,searchtext,replacetext);
	if(mystr->getResult()<0)
		itemsReplaced+=mystr->getResult()*-1;
	gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&startiter,&enditer);
	gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&startiter,newtxt,-1);
	delete mystr;
	free(txt);
	moveBMsForPage(page,MOVEBM);
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
	loadingSession=false;
	page->isDirty=true;
}

void doReplaceAll(void)
{
	pageStruct	*page=NULL;
	GtkTextIter	startiter;
	GtkTextIter	enditer;
	
	if(findInAllFiles==true)
		if(yesNo((char*)DIALOG_YESNO_REPLACE_IN_ALL_FILES,(char*)"")==GTK_RESPONSE_CANCEL)
			return;
	g_signal_handler_block(mainNotebook,switchPageHandler);

	if(findInAllFiles==true)
		{
			for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
				{
					page=getPageStructByIDFromPage(j);
					if(page!=NULL)
						replaceAllTextInBuffer(page);
				}
		}
	else
		{
			page=getPageStructByIDFromPage(-1);
			if(page!=NULL)
				replaceAllTextInBuffer(page);
		}
	g_signal_handler_unblock(mainNotebook,switchPageHandler);
}

VISIBLE void basicFind(int dowhat)
{
	GtkTextIter	startiter;
	GtkTextIter	enditer;
	pageStruct	*page=NULL;
	GtkTextIter	starthilite;
	GtkTextIter	endhilite;

#ifdef _USEGTK3_
	flags=GTK_TEXT_SEARCH_TEXT_ONLY;
#else
	flags=GTK_SOURCE_SEARCH_TEXT_ONLY;
#endif

	if(insensitiveSearch==true)
#ifdef _USEGTK3_
		flags=(GtkTextSearchFlags)(GTK_TEXT_SEARCH_TEXT_ONLY|GTK_TEXT_SEARCH_CASE_INSENSITIVE);
#else
		flags=(GtkSourceSearchFlags)(GTK_SOURCE_SEARCH_TEXT_ONLY|GTK_SOURCE_SEARCH_CASE_INSENSITIVE);
#endif


	if(pages==NULL)
		return;

	if(gtk_entry_get_text_length((GtkEntry*)findBox)==0)
		return;

	if(replacetext!=NULL)
		free(replacetext);
	if(searchtext!=NULL)
		free(searchtext);
	replacetext=g_strcompress(gtk_entry_get_text((GtkEntry*)replaceBox));
	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)findBox));
	
	if((pageData==NULL) || ((getPageStructByIDFromPage(-1)!=NULL) && (currentpageid!=getPageStructByIDFromPage(-1)->pageID)))
		{
			if(pageData!=NULL)
				free(pageData);
			pageData=getCurrentPageListData();
			if(pageData==NULL)
				return;
			startchar=getCurrentCursorPos();
			oldstartchar=startchar;
			searchPageStruct=pageData->page;
			finishedpage=false;
			currentpageid=searchPageStruct->pageID;
		}

	gtk_text_buffer_get_start_iter((GtkTextBuffer*)searchPageStruct->buffer,&starthilite);
	gtk_text_buffer_get_end_iter((GtkTextBuffer*)searchPageStruct->buffer,&endhilite);
	gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)searchPageStruct->buffer,"highlighttag",&starthilite,&endhilite);

	if(hightlightAll==true)
		{
			while(searchForward(&starthilite,searchtext,flags,&starthilite,&endhilite,NULL))
				{
					gtk_text_buffer_apply_tag_by_name((GtkTextBuffer*)searchPageStruct->buffer,"highlighttag",&starthilite,&endhilite);
					starthilite=endhilite;
				}
		}

	switch(dowhat)
		{
			case REPLACENEXT:
				if(replaceAll==true)
					{
						itemsReplaced=-1;
						doReplaceAll();
						gtk_statusbar_remove_all((GtkStatusbar*)statusWidget,statusID);
						while(gtk_events_pending())
							gtk_main_iteration();
						showOnStatus(searchtext,replacetext);
						return;
					}

				if(gtk_text_buffer_get_has_selection((GtkTextBuffer*)searchPageStruct->buffer))
					{
						gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)searchPageStruct->buffer,&startiter,&enditer);
						if(searchForward(&startiter,searchtext,flags,&startiter,&enditer,&enditer))
							{
								gtk_text_buffer_delete((GtkTextBuffer*)searchPageStruct->buffer,&startiter,&enditer);
								gtk_text_buffer_insert((GtkTextBuffer*)searchPageStruct->buffer,&startiter,replacetext,-1);
							}
						}
			case FINDNEXT:
				if(getCharacterPos(CURRENTCHAR,NULL)!=oldstartchar)
					{
						startchar=getCharacterPos(CURRENTCHAR,NULL);
						oldstartchar=startchar;
					}
				finishedpage=findInPage(!searchBack);
				oldstartchar=getCharacterPos(CURRENTCHAR,NULL);
				if(finishedpage==true)
					{
						whatToDo();
						finishedpage=findInPage(!searchBack);
					}
				break;
		}
}

void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data)
{
	bool		flag=false;
	GSList		*tlist;
	const char	*edata;
	GtkWidget	*entry;
	GtkWidget	*drop;
	GSList		*list;

	if(response_id==GTK_RESPONSE_DELETE_EVENT)
		return;

	if(response_id!=REPLACENEXT)
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

	edata=gtk_entry_get_text((GtkEntry*)entry);
	if(list==NULL)
		{
			list=g_slist_append(list,strdup(edata));
			gtk_combo_box_text_append_text((GtkComboBoxText*)drop,(const char*)edata);
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
					list=g_slist_append(list,strdup(edata));
					gtk_combo_box_text_append_text((GtkComboBoxText*)drop,(const char*)edata);
				}
		}

	currentFindPage=gtk_notebook_get_current_page(mainNotebook);
	pagesChecked=0;

	if(response_id!=REPLACENEXT)
		findList=list;
	else
		replaceList=list;

	if(useRegex==false)
		basicFind(response_id);
	else
		regexFind(response_id);
}

VISIBLE void find(GtkWidget *widget,gpointer data)
{
	gtk_window_present((GtkWindow*)findReplaceDialog);
}

VISIBLE void findNext(GtkWidget *widget,gpointer data)
{
	basicFind(FINDNEXT);
}

void doSearchPrefs(GtkWidget *widget,gpointer data)
{
	GtkWidget	*button;

	switch((long)data)
		{
			case 1:
				insensitiveSearch=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 2:
				wrapSearch=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 3:
				replaceAll=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				button=gtk_dialog_get_widget_for_response((GtkDialog*)findReplaceDialog,REPLACENEXT);
				if(replaceAll==false)
					gtk_button_set_label((GtkButton*)button,FIND_REPLACE_LABEL);
				else
					gtk_button_set_label((GtkButton*)button,FIND_REPLACE_ALL_MNEMONIC);
				break;
			case 4:
				findInAllFiles=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 5:
				useRegex=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 6:
				hightlightAll=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
			case 7:
				searchBack=gtk_toggle_button_get_active((GtkToggleButton*)widget);
				break;
		}
}

void doLiveSearch(GtkWidget *widget,GdkEvent *event,gpointer data)
{
	pageStruct 			*page=getPageStructByIDFromPage(-1);
#ifdef _USEGTK3_
	GtkTextSearchFlags		flags=GTK_TEXT_SEARCH_TEXT_ONLY;
#else
	GtkSourceSearchFlags	flags=GTK_SOURCE_SEARCH_TEXT_ONLY;
#endif
	char					*searchtext;
	int						modkey=((GdkEventKey*)event)->state;

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

#ifdef _USEGTK3_
	flags=(GtkTextSearchFlags)(GTK_TEXT_SEARCH_TEXT_ONLY|GTK_TEXT_SEARCH_CASE_INSENSITIVE);
#else
	flags=(GtkSourceSearchFlags)(GTK_SOURCE_SEARCH_TEXT_ONLY|GTK_SOURCE_SEARCH_CASE_INSENSITIVE);
#endif

	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)widget));

	if(modkey==0)
		{
			if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
			if(searchForward(&page->match_end,searchtext,flags,&page->match_start,&page->match_end,NULL))
				{
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
					scrollToIterInPane(page,&page->match_start);
					page->iter=page->match_end;
				}
			else
				{
					gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
					if(searchForward(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
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
			if(searchBackward(&page->match_start,searchtext,flags,&page->match_start,&page->match_end,NULL))
				{
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
					scrollToIterInPane(page,&page->match_start);
					page->iter=page->match_start;
				}
			else
				{
					gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->iter);
					if(searchBackward(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
						{
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
							scrollToIterInPane(page,&page->match_start);
							page->iter=page->match_start;
						}
				}
		}
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}
