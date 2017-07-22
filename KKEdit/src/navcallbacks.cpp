/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:50:25 GMT 2015 kdhedger68713@gmail.com

 * This file(navcallbacks.cpp) is part of KKEdit.

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

int	theLineNum=0;
int marknum=0;

void goToDefine(functionData *fdata)
{
	ERRDATA
	pageStruct	*page;
	TextBuffer	*buf;

	globalHistory->saveLastPosAndStop();
	if(fdata->intab==-1)
		{
			openFile(fdata->file,fdata->line-1,true);
			page=getPageStructByIDFromPage(-1);

		if(page!=NULL)
			{
				buf=new TextBuffer((GtkTextBuffer*)page->buffer);
				while(gtk_events_pending())
					gtk_main_iteration();
				buf->scroll2Line((GtkTextView*)page->view,fdata->line-2,true);
				ERRDATA delete buf;
			}
		}
	else
		{
			page=getPageStructByIDFromPage(fdata->intab);
			gtk_notebook_set_current_page(mainNotebook,fdata->intab);
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->scroll2Line((GtkTextView*)page->view,fdata->line-1);
			ERRDATA delete buf;
		}
	globalHistory->redoMenus();
}

VISIBLE void goToDefinition(GtkWidget *widget,gpointer data)
{
	ERRDATA
	pageStruct		*page=getPageStructByIDFromPage(-1);
	GtkTextIter		start;
	GtkTextIter		end;
	char			*selection=NULL;

	if(page==NULL)
		return;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		{
			return;
		}
	doGoogleSearch=false;
	if(autoSearchDocs==true)
		{
			if(defSearchFromBar((GtkWidget*)selection,NULL)==false)
				{
					if(searchGtkDocs(NULL,(void*)selection)==false)
						if(searchQT5Docs(NULL,(void*)selection)==false)
							{
								doGoogleSearch=true;
								showDocView(USEURI,selection,DOCVIEW_SEARCHING_GOOGLE_LABEL);
							}
				}
		}
	else
		defSearchFromBar((GtkWidget*)selection,NULL);
	doGoogleSearch=true;
	ERRDATA 
}

VISIBLE void findFile(GtkWidget *widget,gpointer data)
{
	ERRDATA
	char			*command;
	char			buffer[2048];
	TextBuffer		*buf;
	char			*selection=NULL;
	char			*filename=NULL;
	char			*searchdir=NULL;
	char			*filepath=NULL;
	FILE			*fp;
	StringSlice	*slice;
	pageStruct		*page=getPageStructByIDFromPage(-1);

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);
	slice= new StringSlice;

	selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&buf->lineStart,&buf->lineEnd,false);
	if(selection[0]!='#')
		{
			ERRDATA return;
		}
	slice->setReturnDupString(false);
	filename=slice->sliceBetween(selection,(char*)"include ",NULL);

	if(slice->getResult()==0)
		{
			if(filename[0]=='<')
				searchdir=strdup("/usr/include");
			else
				searchdir=strdup(page->dirName);

			sinkReturn=asprintf(&filepath,"%s/%s",searchdir,slice->sliceLen(filename,1,strlen(filename)-3));
			if(openFile(filepath,0,false)==false)
				{
					sinkReturn=asprintf(&command,"find \"%s\" -name \"%s\"",searchdir,basename(filepath));
					fp=popen(command, "r");
					if(fp!=NULL)
						{
							while(fgets(buffer,1024,fp))
								{
									buffer[strlen(buffer)-1]=0;
									openFile(buffer,0,false);
								}
							pclose(fp);
						}
					ERRDATA debugFree(&command);
				}
			ERRDATA debugFree(&filepath);
		}
	ERRDATA delete buf;
	ERRDATA delete slice;
}

void gotoLineSavePos(GtkWidget *widget,gpointer data)
{
	ERRDATA
	int			line=(long)data;
	TextBuffer	*buf;
	pageStruct	*page=getPageStructByIDFromPage(-1);

	if(page!=NULL)
		{
			globalHistory->saveLastPosAndStop();
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->scroll2Line((GtkTextView*)page->view,line-1,true);
			ERRDATA delete buf;
		}
	ERRDATA
	globalHistory->redoMenus();
}

void gotoLineNoSavePos(GtkWidget *widget,gpointer data)
{
	ERRDATA
	int			line=(long)data;
	TextBuffer	*buf;
	pageStruct	*page=getPageStructByIDFromPage(-1);

	if(page!=NULL)
		{
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->scroll2Line((GtkTextView*)page->view,line-1,true);
			ERRDATA delete buf;
		}
	ERRDATA
}

VISIBLE void gotoLine(GtkWidget *widget,gpointer data)
{
	ERRDATA
	int			line=(long)data;
	TextBuffer	*buf;
	pageStruct	*page=getPageStructByIDFromPage(-1);

	if(page!=NULL)
		{
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->scroll2Line((GtkTextView*)page->view,line-1);
			ERRDATA delete buf;
		}
	ERRDATA
}

void jumpToLineFromBar(GtkWidget *widget,gpointer data)
{
	ERRDATA
	theLineNum=atoi(gtk_entry_get_text((GtkEntry*)widget));
	gotoLineNoSavePos(NULL,(gpointer)(long)theLineNum);
	ERRDATA
}

int showLineEntry(void)
{
	ERRDATA
	GtkWidget	*dialog;
	gint		result;
	GtkWidget	*content_area;
	GtkWidget	*entrybox;
	char		line[48];

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,DIALOG_ENTER_LINE_LABEL);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),DIALOG_GOTO_LINE_LABEL);

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

	ERRDATA return(result);
}

VISIBLE void jumpToLine(GtkWidget *widget,gpointer data)
{
	ERRDATA
	if(showLineEntry()==GTK_RESPONSE_YES)
		gotoLine(NULL,(gpointer)(long)theLineNum);
	ERRDATA
}

VISIBLE void functionSearch(GtkWidget *widget,gpointer data)
{
	ERRDATA

	if(showFunctionEntry()==GTK_RESPONSE_YES)
		{
			if(functionSearchText!=NULL)
				{
					defSearchFromBar((GtkWidget*)functionSearchText,NULL);
				}
		}
	ERRDATA
}

void jumpToMark(GtkWidget *widget,gpointer data)
{
	ERRDATA
	GtkTextMark	*mark;
	pageStruct	*page;
	pageStruct	*checkpage;
	TextBuffer	*buf;

	globalHistory->saveLastPosAndStop();
	page=(pageStruct*)((bookMarksNew*)data)->page;
	mark=(GtkTextMark*)((bookMarksNew*)data)->mark;
	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	buf->scroll2Mark((GtkTextView*)page->view,mark);

	for(int loop=0; loop<gtk_notebook_get_n_pages(mainNotebook); loop++)
		{
			checkpage=getPageStructByIDFromPage(loop);
			if(checkpage==page)
				{
					gtk_notebook_set_current_page(mainNotebook,loop);
					gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&buf->cursorPos);
					ERRDATA delete buf;
					globalHistory->redoMenus();
					ERRDATA return;
				}
		}
	ERRDATA delete buf;
	globalHistory->redoMenus();
}

#ifdef _BUILDDOCVIEWER_

char *unEscapeFileNAme(char *name)
{
	ERRDATA
	char	*buffer;
	int		charpos;
	unsigned int		namepos;

	buffer=(char*)calloc(strlen(name),1);
	charpos=0;
	namepos=0;

	while(namepos<strlen(name))
		{
			if(name[namepos]!='_')
				{
					buffer[charpos]=name[namepos];
				}
			else
				{
					namepos++;
					switch(name[namepos])
						{
						case '_':
							buffer[charpos]='_';
							break;
						case '1':
							buffer[charpos]=':';
							break;
						case '2':
							buffer[charpos]='/';
							break;
						case '3':
							buffer[charpos]='<';
							break;
						case '4':
							buffer[charpos]='>';
							break;
						case '5':
							buffer[charpos]='*';
							break;
						case '6':
							buffer[charpos]='&';
							break;
						case '7':
							buffer[charpos]='|';
							break;
						case '8':
							buffer[charpos]='.';
							break;
						case '9':
							buffer[charpos]='!';
							break;
						case '0':
							namepos++;
							switch(name[namepos])
								{
								case '0':
									buffer[charpos]=',';
									break;
								case '1':
									buffer[charpos]=' ';
									break;
								case '2':
									buffer[charpos]='{';
									break;
								case '3':
									buffer[charpos]='}';
									break;
								case '4':
									buffer[charpos]='?';
									break;
								case '5':
									break;
									buffer[charpos]='^';
								case '6':
									buffer[charpos]='%';
									break;
								case '7':
									buffer[charpos]='(';
									break;
								case '8':
									buffer[charpos]=')';
									break;
								case '9':
									buffer[charpos]='+';
									break;
								case 'A':
									buffer[charpos]='=';
								case 'B':
									buffer[charpos]='$';
									break;
								case 'C':
									buffer[charpos]='\\';
									break;
								}
						default:
							buffer[charpos]=toupper(name[namepos]);
							break;
						}
				}
			namepos++;
			charpos++;
		}

	ERRDATA return(buffer);
}

char	*filebuffer=NULL;

bool readFile(char *name)
{
	ERRDATA
	FILE			*file;
	unsigned long	fileLen;

	if(filebuffer!=NULL)
		{
			ERRDATA debugFree(&filebuffer);
		}
	//Open file
	file=fopen(name,"rb");
	if(!file)
		{
			fprintf(stderr,"Unable to open file %s",name);
			ERRDATA return(false);
		}

	//Get file length
	fseek(file,0,SEEK_END);
	fileLen=ftell(file);
	fseek(file,0,SEEK_SET);

	//Allocate memory
	filebuffer=(char*)malloc(fileLen+1);
	if(!filebuffer)
		{
			fprintf(stderr,"Memory error!");
			fclose(file);
			ERRDATA return(false);
		}

	//Read file contents into buffer
	sinkReturn=fread(filebuffer,fileLen,1,file);
	fclose(file);
	ERRDATA return(true);
}

int getLineFromXML(char *xml)
{
	ERRDATA
	StringSlice	*slice=new StringSlice;

	char			*data;
	char			*xmldata=xml;
	int				retline=1;

	data=slice->sliceBetween(xmldata,(char*)"Definition at line",(char*)"\">");
	if(slice->getResult()==0)
		retline=atoi(slice->sliceBetween(data,(char*)"#l",NULL));
	ERRDATA delete slice;
	ERRDATA return(retline);
}

bool	mustBeAClass=false;
int		classLineNumber=1;
char	*classFileName=NULL;

char *getPathFromXML(char *xml)
{
	ERRDATA
	StringSlice	*slice=new StringSlice;
	bool			done=false;
	char			*data;
	unsigned int	bufferlen=1024;
	char			*buffer=(char*)calloc(bufferlen,1);
	char			*xmldata=xml;
	char			*portion;

	buffer[0]=0;
	mustBeAClass=false;

	xmldata=strstr(xml,(char*)"<li class=\"navelem\"");
	while(done==false)
		{
			data=slice->sliceInclude(xmldata,(char*)"<li class=\"navelem\"",(char*)"</a>");
			if(slice->getResult()==0)
				{
					portion=slice->sliceBetween(data,(char*)"html\">",(char*)"</a>");
					if(strlen(buffer)+strlen(portion)+1<bufferlen)
						{
							bufferlen=bufferlen+strlen(portion)+1024;
							buffer=(char*)realloc(buffer,bufferlen);
						}
					strcat(buffer,"/");
					strcat(buffer,portion);
					xmldata=strstr(xmldata,portion);
					xmldata=(char*)(long)xmldata+strlen(data);
				}
			else
				done=true;
		}

	xmldata=xml;
	done=false;
// Class Reference
	data=slice->sliceBetween(xmldata,(char*)"\"title\">",(char*)" Class Reference");
	if(slice->getResult()==0)
		{
			xmldata=strstr(xml,(char*)"<p>Definition at line ");
			if(xmldata!=NULL)
				{
					char	*tfile;
					sinkReturn=asprintf(&tfile,"/%s",slice->sliceBetween(xmldata,(char*)".html\">",(char*)"</a>"));					
					ERRDATA delete slice;
					ERRDATA return(tfile);
				}
		}

	xmldata=xml;
	data=slice->sliceBetween(xmldata,(char*)"\"title\">",(char*)" File Reference");
	if(slice->getResult()==0)
		{
			if(strlen(buffer)+strlen(data)+1<bufferlen)
				{
					bufferlen=bufferlen+strlen(data)+1024;
					buffer=(char*)realloc(buffer,bufferlen);
				}
			strcat(buffer,"/");
			strcat(buffer,data);
		}
	else
		{
			data=slice->sliceBetween(xmldata,(char*)"\"title\">",(char*)"</div>");
			if(slice->getResult()==0)
				{
					if(strlen(buffer)+strlen(data)+1<bufferlen)
						{
							bufferlen=bufferlen+strlen(data)+1024;
							buffer=(char*)realloc(buffer,bufferlen);
						}
					strcat(buffer,"/");
					strcat(buffer,data);
				}
			else
				{
					data=slice->sliceBetween(xmldata,(char*)"<title>",(char*)"</title>");
				if(slice->getResult()==0)
						{
						if(strstr(data,"::")!=NULL)
							{
								xmldata=strstr(xml,(char*)"<p>Definition at line ");
								if(xmldata!=NULL)
									{
										mustBeAClass=false;
										classLineNumber=atoi(slice->sliceBetween(xmldata,(char*)"#l",(char*)"\">"));
										sinkReturn=asprintf(&classFileName,"/%s",slice->sliceBetween(xmldata,(char*)".html\">",(char*)"</a>"));
										ERRDATA delete slice;
										ERRDATA return(classFileName);
									}
							}
						}
				}
		}
	ERRDATA delete slice;
	ERRDATA return(buffer);
}

gboolean docLinkTrap(WebKitWebView *web_view,WebKitWebFrame *frame,WebKitNetworkRequest *request,WebKitWebNavigationAction *navigationAction,WebKitWebPolicyDecision *policy_decision, gpointer user_data)
{
	ERRDATA
	int				mod=-1;
	const char		*uri;
	StringSlice		slce;
	char			*htmlpage;
	FILE			*fp;
	char			*command;
	char			line[4096];
	char			*filep;
	int				linenum;

	mod=webkit_web_navigation_action_get_modifier_state(navigationAction);
	if(mod&GDK_SHIFT_MASK)
		{
			uri=webkit_network_request_get_uri(request);
			slce.setReturnDupString(true);
			htmlpage=slce.sliceBetween((char*)uri,(char*)"file://",(char*)"#");
			memset(line,0,4096);
			sinkReturn=asprintf(&command,"cat %s 2>/dev/null|grep \"<p>Definition at line \"",htmlpage);
			fp=popen(command,"r");
			free(command);
			sinkReturnStr=fgets(line,4096,fp);
			pclose(fp);
			if(strlen(line)>0)
				{
					linenum=atoi(slce.sliceBetween(line,(char*)"#l",(char*)"\">"));
					filep=slce.sliceBetween(htmlpage,NULL,(char*)"/html/");
					slce.setReturnDupString(false);
					sinkReturn=asprintf(&command,"%s/%s",filep,slce.sliceBetween(line,(char*)"html\">",(char*)"</a>"));
					if(openFile(command,linenum,false)==true)
						{
							if(mainWindow!=NULL)
								gtk_window_present((GtkWindow*)mainWindow);
							gotoLine(NULL,(gpointer)(long)linenum);
						}
					free(command);
					free(filep);
					free(htmlpage);
					ERRDATA return(false);
				}

			slce.setReturnDupString(false);
			if(strstr(uri,(char*)"#l")!=NULL)
				{
					linenum=atoi(slce.sliceBetween((char*)uri,(char*)"#l",NULL));
					command=(char*)slce.sliceBetween((char*)uri,(char*)"file://",(char*)"#l");
					command=(char*)slce.sliceBetween((char*)command,NULL,(char*)".html");
					command=(char*)slce.sliceBetween((char*)command,NULL,(char*)"_source");
					command=(char*)slce.deleteSlice((char*)command,(char*)"html/");
					command=(char*)slce.replaceAllSlice((char*)command,(char*)"_8",(char*)".");
					const char *upperdata[]={"_a","A","_b","B","_c","C","_d","D","_e","E","_f","F","_g","G","_h","H","_i","I","_j","J","_k","K","_l","L","_m","M","_n","N","_o","O","_p","P","_q","Q","_r","R","_s","S","_t","T","_u","U","_v","V","_w","W","_x","X","_y","Y","_z","Z"};
					for(int j=0;j<52;j+=2)
						command=(char*)slce.replaceAllSlice((char*)command,(char*)upperdata[j],(char*)upperdata[j+1]);
					if(openFile(command,linenum,true)==true)
						{
							if(mainWindow!=NULL)
								gtk_window_present((GtkWindow*)mainWindow);
							gotoLine(NULL,(gpointer)(long)linenum);
						}
				}
			else
				{
					command=(char*)slce.sliceBetween((char*)uri,(char*)"file://",(char*)".html");
					command=(char*)slce.sliceBetween((char*)command,NULL,(char*)"_source");
					command=(char*)slce.deleteSlice((char*)command,(char*)"html/");
					command=(char*)slce.replaceAllSlice((char*)command,(char*)"_8",(char*)".");
					if(openFile(command,-1,false)==false)
						{
							slce.setReturnDupString(true);
							htmlpage=slce.sliceBetween((char*)uri,(char*)"file://",NULL);
							memset(line,0,4096);
							sinkReturn=asprintf(&command,"cat %s 2>/dev/null|grep \"<p>Definition at line \"",htmlpage);
							fp=popen(command,"r");
							free(command);
							sinkReturnStr=fgets(line,4096,fp);
							pclose(fp);
							if(strlen(line)>0)
								{
									linenum=atoi(slce.sliceBetween(line,(char*)"#l",(char*)"\">"));
									filep=slce.sliceBetween(htmlpage,NULL,(char*)"/html/");
									slce.setReturnDupString(false);
									sinkReturn=asprintf(&command,"%s/%s",filep,slce.sliceBetween(line,(char*)"html\">",(char*)"</a>"));
									if(openFile(command,linenum,false)==true)
										{
											if(mainWindow!=NULL)
												gtk_window_present((GtkWindow*)mainWindow);
											gotoLine(NULL,(gpointer)(long)linenum);
										}
									free(command);
									free(filep);
									free(htmlpage);
									ERRDATA return(false);
								}
						}
				}
		}
	ERRDATA return(false);
}
#endif

//jump to tab
void selectTab(GtkWidget *widget,gpointer data)
{
	ERRDATA
	GdkModifierType	mask;
	int				newpage;
	int				thispage;
	GtkWidget		*child;
	TextBuffer		*buf;
	pageStruct		*page=NULL;

#ifdef _USEGTK3_
#if GTK_MINOR_VERSION >=22

	GdkSeat	*seat=gdk_display_get_default_seat(gdk_display_get_default());
	GdkDevice			*device=gdk_seat_get_pointer(seat);
	GdkWindow			*window;

	window=gdk_window_get_device_position (NULL,device,NULL,NULL,&mask);
#else
	gdk_window_get_pointer(NULL,NULL,NULL,&mask);
#endif
#else
	gdk_window_get_pointer(NULL,NULL,NULL,&mask);
#endif

	if(GDK_SHIFT_MASK & mask)
		{
			thispage=gtk_notebook_get_current_page(mainNotebook);
			child=gtk_notebook_get_nth_page(mainNotebook,(int)(long)data);
			if((int)(long)data<thispage)
				newpage=thispage;
			else
				newpage=thispage+1;
				
			gtk_notebook_reorder_child(mainNotebook,child,newpage);
			gtk_notebook_set_current_page(mainNotebook,newpage);
		}
	else
		{
			gtk_notebook_set_current_page(mainNotebook,(int)(long)data);
		}

	page=getPageStructByIDFromPage(-1);
	if(page==NULL)
		return;

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);
	buf->scroll2CentreScreen((GtkTextView*)page->view,false);
	page->hidden=false;
	delete buf;

	ERRDATA
}

void rebuildTabsMenu(void)
{
	ERRDATA
	GtkWidget	*menuitem;
	GtkWidget	*submenu;
	pageStruct	*page;

	int			numtabs=gtk_notebook_get_n_pages(mainNotebook);

	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)viewTabMenu);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)viewTabMenu,NULL);

	ERRDATA
	viewTabSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewTabMenu),viewTabSubMenu);

	ERRDATA
	for(int j=0;j<numtabs;j++)
		{
			page=getPageStructByIDFromPage(j);
			if(page!=NULL)
				{
					menuitem=gtk_menu_item_new_with_label(gtk_label_get_text((GtkLabel *)page->tabName));
					gtk_menu_shell_append(GTK_MENU_SHELL(viewTabSubMenu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(selectTab),(gpointer)(long)j);
				}
		}
	ERRDATA
	gtk_widget_show_all(viewTabMenu);
	ERRDATA
}

void menuJumpBack(GtkWidget *widget,gpointer data)
{
	TextBuffer	*buf;
	pageStruct	*page=NULL;
	pageStruct	*checkpage=NULL;
	unsigned	pageid;
	int			savecnt=(int)((long)data);
	historyData	*hist=globalHistory->getHistory(savecnt);
	unsigned	line=hist->lineNumber;

	pageid=hist->pageID;
	page=getPageStructByIDFromList(pageid);
	if(page!=NULL)
		{
			for(int loop=0;loop<gtk_notebook_get_n_pages(mainNotebook);loop++)
				{
					checkpage=getPageStructByIDFromPage(loop);
					if((checkpage!=NULL) && (checkpage->pageID==pageid))
						{
							globalHistory->saveLastPos();
							buf=new TextBuffer((GtkTextBuffer*)page->buffer);
							gtk_notebook_set_current_page(mainNotebook,loop);
							gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&buf->cursorPos);
							buf->scroll2Line((GtkTextView*)page->view,line-1,true);

							ERRDATA delete buf;
							globalHistory->setSaveCnt((unsigned)((long)data));
							globalHistory->redoMenus();
							ERRDATA return;
						}
				}
		}
}



