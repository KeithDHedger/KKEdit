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
	pageStruct*	page;
	TextBuffer*	buf;

	if(fdata->intab==-1)
		{
			openFile(fdata->file,fdata->line-1,true);
			page=getPageStructPtr(gtk_notebook_get_n_pages(mainNotebook)-1);
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->scroll2Line((GtkTextView*)page->view,fdata->line-2);
			delete buf;
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

			delete buf;
		}
}

VISIBLE void goToDefinition(GtkWidget* widget,gpointer data)
{
	pageStruct*		page=getPageStructPtr(-1);
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	functionData*	fdata=NULL;

	if(page==NULL)
		return;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	fdata=getFunctionByName(selection,true);
	if(fdata!=NULL)
		{
			history->savePosition();
			goToDefine(fdata);
			destroyData(fdata);
		}
	return;
}

VISIBLE void findFile(GtkWidget* widget,gpointer data)
{
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
		return;
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
					debugFree(&command,"findFile command");
				}
			debugFree(&filepath,"findFile filepath");
		}
	delete buf;
	delete slice;
}

void gotoLine(GtkWidget* widget,gpointer data)
{
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
			delete buf;
		}
}

void jumpToLineFromBar(GtkWidget* widget,gpointer data)
{
	theLineNum=atoi(gtk_entry_get_text((GtkEntry*)widget));
	gotoLine(NULL,(gpointer)(long)theLineNum);
}

int showLineEntry(void)
{
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

	return(result);
}

VISIBLE void jumpToLine(GtkWidget* widget,gpointer data)
{
	if(showLineEntry()==GTK_RESPONSE_YES)
		gotoLine(NULL,(gpointer)(long)theLineNum);
}

VISIBLE void functionSearch(GtkWidget* widget,gpointer data)
{
	functionData* fdata;

	if(showFunctionEntry()==GTK_RESPONSE_YES)
		{
			if(functionSearchText!=NULL)
				{
					fdata=getFunctionByName(functionSearchText,true);
					if(fdata!=NULL)
						{
							goToDefine(fdata);
							destroyData(fdata);
						}
				}
		}
}

void jumpToMark(GtkWidget* widget,gpointer data)
{
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
					delete buf;
					return;
				}
		}
	delete buf;
}

#ifdef _BUILDDOCVIEWER_

char* unEscapeFileNAme(char* name)
{
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

	return(buffer);
}

struct docFileData
{
	char*	fileName;
	char*	filePath;
	int		lineNum;
	char*	hashTag;
	char*	sourceFile;
};

char*	filebuffer=NULL;

bool readFile(char *name)
{
	FILE*			file;
	unsigned long	fileLen;

	if(filebuffer!=NULL)
		debugFree(&filebuffer,"readFile filebuffer");
	//Open file
	file=fopen(name,"rb");
	if (!file)
		{
			fprintf(stderr,gettext("Unable to open file %s"),name);
			return(false);
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
			return(false);
		}

	//Read file contents into buffer
	fread(filebuffer,fileLen,1,file);
	fclose(file);
	return(true);
}

int getLineFromXML(char* xml)
{
	StringSlice*	slice=new StringSlice;

	char*			data;
	char*			xmldata=xml;
	int				retline=1;

	data=slice->sliceBetween(xmldata,(char*)"Definition at line",(char*)"\">");
	if(slice->getResult()==0)
		retline=atoi(slice->sliceBetween(data,(char*)"#l",NULL));
	delete slice;
	return(retline);
}

bool	mustBeAClass=false;
int		classLineNumber=1;
char*	classFileName=NULL;

char* getPathFromXML(char* xml)
{
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
					delete slice;
					return(tfile);
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
										delete slice;
										return(classFileName);
									}
							}
						}
				}
		}
	delete slice;
	return(buffer);
}

docFileData* getDoxyFileData(char* uri)
{
	char*	linetag=NULL;
	bool	gotline=false;

	char*	unhashedline=NULL;
	char*	filepath=NULL;

	StringSlice*	slice=new StringSlice;

	slice->setReturnDupString(true);
	docFileData* doxydata=(docFileData*)malloc(sizeof(docFileData));

	doxydata->lineNum=1;
	doxydata->fileName=(char*)"";
	doxydata->filePath=(char*)"";
	doxydata->hashTag=(char*)"";
	doxydata->sourceFile=(char*)"";

	linetag=slice->sliceBetween(uri,(char*)"#",NULL);
	if(slice->getResult()==0)
		{
			if(linetag[0]=='l')
				{
					doxydata->lineNum=atoi(&linetag[1]);
					gotline=true;
				}
			unhashedline=slice->sliceBetween(uri,NULL,(char*)"#");
		}
	else
		{
			unhashedline=strdup(uri);
			linetag=strdup("");
		}

	filepath=g_filename_from_uri(unhashedline,NULL,NULL);
	if(readFile(filepath)==true)
		{
			doxydata->filePath=slice->sliceBetween(filepath,NULL,(char*)"/html/");
			doxydata->fileName=getPathFromXML(filebuffer);
			if(mustBeAClass==false)
				{
					asprintf(&doxydata->sourceFile,"%s%s",doxydata->filePath,doxydata->fileName);
					if(gotline==false)
						{
							doxydata->lineNum=getLineFromXML(filebuffer);
						}
				}
			else
				{
					doxydata->lineNum=classLineNumber;
					asprintf(&doxydata->sourceFile,"%s%s",doxydata->filePath,classFileName);
					
				}
		}
	else
		{
			debugFree((char**)&doxydata,"getDoxyFileData doxydata");
			doxydata=NULL;
		}
	debugFree(&unhashedline,"getDoxyFileData unhashedline");
	debugFree(&linetag,"getDoxyFileData linetag");
	debugFree(&filepath,"getDoxyFileData filepath");
	return(doxydata);
}

gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigationAction,WebKitWebPolicyDecision* policy_decision, gpointer user_data)
{
	int				mod=-1;
	const char*		uri;
	pageStruct*		page;
	TextBuffer*		buf;
	docFileData*	doxydata;

	mod=webkit_web_navigation_action_get_modifier_state(navigationAction);
	if(mod&GDK_SHIFT_MASK)
		{
			uri=webkit_network_request_get_uri(request);
			doxydata=getDoxyFileData((char*)uri);
			if(doxydata==NULL)
				return(false);

//check in open tabs
			buf=new TextBuffer;
			for(int j=0; j<gtk_notebook_get_n_pages(mainNotebook); j++)
				{
					page=getPageStructPtr(j);
					if((strcmp(page->realFilePath,doxydata->sourceFile)==0) || (strcmp(page->filePath,doxydata->sourceFile)==0))
						{
							gtk_notebook_set_current_page(mainNotebook,j);
							buf->textBuffer=(GtkTextBuffer*)page->buffer;
							buf->scroll2LineM(page,doxydata->lineNum-1);
							delete buf;
							debugFree((char**)&doxydata,"docLinkTrap doxydata");
							return(false);
						}
				}
//try to open file f not in tabs
			openFile(doxydata->sourceFile,doxydata->lineNum,false);
			debugFree((char**)&doxydata,"docLinkTrap doxydata");
		}
	return(false);
}

#endif

