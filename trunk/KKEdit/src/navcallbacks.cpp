/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include "kkedit-includes.h"

int	theLineNum=0;
int marknum=0;

void goToDefine(functionData* fdata)
{
	ERRDATA
	pageStruct*	page;
	TextBuffer*	buf;

	if(fdata->intab==-1)
		{
			openFile(fdata->file,fdata->line-1,true);
			page=getPageStructPtr(gtk_notebook_get_n_pages(mainNotebook)-1);
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->scroll2Line((GtkTextView*)page->view,fdata->line-2);
			ERRDATA delete buf;
		}
	else
		{
			page=getPageStructPtr(fdata->intab);
			gtk_notebook_set_current_page(mainNotebook,fdata->intab);
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);

			if(page->inTop==true)
				buf->scroll2Line((GtkTextView*)page->view,fdata->line-1);
			else
				buf->scroll2Line((GtkTextView*)page->view2,fdata->line-1);

			ERRDATA delete buf;
		}
	ERRDATA
}

VISIBLE void goToDefinition(GtkWidget* widget,gpointer data)
{
	ERRDATA
	pageStruct*		page=getPageStructPtr(-1);
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;

	if(page==NULL)
		{
			ERRDATA return;
		}

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				{
					ERRDATA return;
				}
		}
	else
		{
			ERRDATA return;
		}
	ERRDATA defSearchFromBar((GtkWidget*)selection,NULL);
}

VISIBLE void findFile(GtkWidget* widget,gpointer data)
{
	ERRDATA
	char*			command;
	char			buffer[2048];
	TextBuffer*		buf;
	char*			selection=NULL;
	char*			filename=NULL;
	char*			searchdir=NULL;
	char*			filepath=NULL;
	FILE*			fp;
	StringSlice*	slice;
	pageStruct*		page=getPageStructPtr(-1);

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

			asprintf(&filepath,"%s/%s",searchdir,slice->sliceLen(filename,1,strlen(filename)-3));
			if(openFile(filepath,0,false)==false)
				{
					asprintf(&command,"find \"%s\" -name \"%s\"",searchdir,basename(filepath));
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

void gotoLine(GtkWidget* widget,gpointer data)
{
	ERRDATA
	int			line=(long)data;
	pageStruct*	page=getPageStructPtr(-1);
	TextBuffer*	buf;

	if(page!=NULL)
		{
			history->savePosition();
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			if(page->inTop==true)
				buf->scroll2Line((GtkTextView*)page->view,line-1);
			else
				buf->scroll2Line((GtkTextView*)page->view2,line-1);
			ERRDATA delete buf;
		}
	ERRDATA
}

void jumpToLineFromBar(GtkWidget* widget,gpointer data)
{
	ERRDATA
	theLineNum=atoi(gtk_entry_get_text((GtkEntry*)widget));
	gotoLine(NULL,(gpointer)(long)theLineNum);
	ERRDATA
}

int showLineEntry(void)
{
	ERRDATA
	GtkWidget*	dialog;
	gint		result;
	GtkWidget*	content_area;
	GtkWidget*	entrybox;
	char		line[48];

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,gettext("Enter Line Number"));

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),gettext("Go To Line"));

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

VISIBLE void jumpToLine(GtkWidget* widget,gpointer data)
{
	ERRDATA
	if(showLineEntry()==GTK_RESPONSE_YES)
		gotoLine(NULL,(gpointer)(long)theLineNum);
	ERRDATA
}

VISIBLE void functionSearch(GtkWidget* widget,gpointer data)
{
	ERRDATA
	functionData	*fdata;
	bool			ok;

	if(showFunctionEntry()==GTK_RESPONSE_YES)
		{
			if(functionSearchText!=NULL)
				{
					ok=true;
					for(int j=0;j<2;j++)
						{
							fdata=getFunctionByName(functionSearchText,true,ok);
							if(fdata!=NULL)
								{
									goToDefine(fdata);
									destroyData(fdata);
									ERRDATA return;
								}
							ok=false;
						}
				}
		}
	ERRDATA
}

void jumpToMark(GtkWidget* widget,gpointer data)
{
	ERRDATA
	GtkTextMark*	mark;
	pageStruct*		page;
	pageStruct*		checkpage;
	TextBuffer*		buf;
	history->savePosition();

	page=(pageStruct*)((bookMarksNew*)data)->page;
	mark=(GtkTextMark*)((bookMarksNew*)data)->mark;
	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	buf->scroll2Mark((GtkTextView*)page->view,mark);

	for(int loop=0; loop<gtk_notebook_get_n_pages(mainNotebook); loop++)
		{
			checkpage=getPageStructPtr(loop);
			if(checkpage==page)
				{
					gtk_notebook_set_current_page(mainNotebook,loop);
					gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&buf->cursorPos);
					ERRDATA delete buf;
					ERRDATA return;
				}
		}
	ERRDATA delete buf;
}

#ifdef _BUILDDOCVIEWER_

char* unEscapeFileNAme(char* name)
{
	ERRDATA
	char*	buffer;
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

char*	filebuffer=NULL;

bool readFile(char *name)
{
	ERRDATA
	FILE*			file;
	unsigned long	fileLen;

	if(filebuffer!=NULL)
		{
			ERRDATA debugFree(&filebuffer);
		}
	//Open file
	file=fopen(name,"rb");
	if (!file)
		{
			fprintf(stderr,gettext("Unable to open file %s"),name);
			ERRDATA return(false);
		}

	//Get file length
	fseek(file,0,SEEK_END);
	fileLen=ftell(file);
	fseek(file,0,SEEK_SET);

	//Allocate memory
	filebuffer=(char*)malloc(fileLen+1);
	if (!filebuffer)
		{
			fprintf(stderr,gettext("Memory error!"));
			fclose(file);
			ERRDATA return(false);
		}

	//Read file contents into buffer
	fread(filebuffer,fileLen,1,file);
	fclose(file);
	ERRDATA return(true);
}

int getLineFromXML(char* xml)
{
	ERRDATA
	StringSlice*	slice=new StringSlice;

	char*			data;
	char*			xmldata=xml;
	int				retline=1;

	data=slice->sliceBetween(xmldata,(char*)"Definition at line",(char*)"\">");
	if(slice->getResult()==0)
		retline=atoi(slice->sliceBetween(data,(char*)"#l",NULL));
	ERRDATA delete slice;
	ERRDATA return(retline);
}

bool	mustBeAClass=false;
int		classLineNumber=1;
char*	classFileName=NULL;

char* getPathFromXML(char* xml)
{
	ERRDATA
	StringSlice*	slice=new StringSlice;
	bool			done=false;
	char*			data;
	unsigned int	bufferlen=1024;
	char*			buffer=(char*)calloc(bufferlen,1);
	char*			xmldata=xml;
	char*			portion;

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
					char*	tfile;
					asprintf(&tfile,"/%s",slice->sliceBetween(xmldata,(char*)".html\">",(char*)"</a>"));					
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
										asprintf(&classFileName,"/%s",slice->sliceBetween(xmldata,(char*)".html\">",(char*)"</a>"));
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

gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigationAction,WebKitWebPolicyDecision* policy_decision, gpointer user_data)
{
	ERRDATA
	int				mod=-1;
	const char*		uri;
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
			asprintf(&command,"cat %s 2>/dev/null|grep \"<p>Definition at line \"",htmlpage);
			fp=popen(command,"r");
			free(command);
			fgets(line,4096,fp);
			pclose(fp);
			if(strlen(line)>0)
				{
					linenum=atoi(slce.sliceBetween(line,(char*)"#l",(char*)"\">"));
					filep=slce.sliceBetween(htmlpage,NULL,(char*)"/html/");
					slce.setReturnDupString(false);
					asprintf(&command,"%s/%s",filep,slce.sliceBetween(line,(char*)"html\">",(char*)"</a>"));
					if(openFile(command,linenum,false)==true)
						gotoLine(NULL,(gpointer)(long)linenum);
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
					command=(char*)slce.sliceBetween((char*)uri,(char*)"file://",(char*)".html");
					command=(char*)slce.sliceBetween((char*)command,NULL,(char*)"_source");
					command=(char*)slce.deleteSlice((char*)command,(char*)"html/");
					command=(char*)slce.replaceAllSlice((char*)command,(char*)"_8",(char*)".");
					if(openFile(command,linenum,true)==true)
						gotoLine(NULL,(gpointer)(long)linenum);
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
							asprintf(&command,"cat %s 2>/dev/null|grep \"<p>Definition at line \"",htmlpage);
							fp=popen(command,"r");
							free(command);
							fgets(line,4096,fp);
							pclose(fp);
							if(strlen(line)>0)
								{
									linenum=atoi(slce.sliceBetween(line,(char*)"#l",(char*)"\">"));
									filep=slce.sliceBetween(htmlpage,NULL,(char*)"/html/");
									slce.setReturnDupString(false);
									asprintf(&command,"%s/%s",filep,slce.sliceBetween(line,(char*)"html\">",(char*)"</a>"));
									if(openFile(command,linenum,false)==true)
										gotoLine(NULL,(gpointer)(long)linenum);
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
void selectTab(GtkWidget* widget,gpointer data)
{
	ERRDATA
	gtk_notebook_set_current_page(mainNotebook,(int)(long)data);
	ERRDATA
}

//rebuild tabs menu
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

	viewTabSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(viewTabMenu),viewTabSubMenu);

	for(int j=0;j<numtabs;j++)
		{
			page=getPageStructPtr(j);
			if(page!=NULL)
				{
					menuitem=gtk_menu_item_new_with_label(gtk_label_get_text((GtkLabel *)page->tabName));
					gtk_menu_shell_append(GTK_MENU_SHELL(viewTabSubMenu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(selectTab),(gpointer)(long)j);
				}
		}
	gtk_widget_show_all(viewTabMenu);
	ERRDATA
}

