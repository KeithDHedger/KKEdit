/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include "kkedit-includes.h"

int		theLineNum=0;
int		marknum=0;
char	*filebuffer=NULL;
bool	mustBeAClass=false;
int		classLineNumber=1;
char	*classFileName=NULL;

void goToDefine(functionData* fdata)
{
#ifdef _USEQT5_
	if(fdata->intab!=-1)
		{
			qobject_cast<QTabWidget*>(mainNotebook)->setCurrentIndex(fdata->intab);
			gotoLine(NULL,fdata->line);
		}
	else
		{
			openFile(fdata->file,fdata->line-1,true);
		}
#else
	pageStruct*	page;
	TextBuffer*	buf;

	if(fdata->intab==-1)
		{
			openFile(fdata->file,fdata->line-1,true);
			page=getDocumentData(gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)-1);
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);
			buf->scroll2Line((GtkTextView*)page->view,fdata->line-2);
			delete buf;
		}
	else
		{
			page=getDocumentData(fdata->intab);
			gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,fdata->intab);
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);

			if(page->inTop==true)
				buf->scroll2Line((GtkTextView*)page->view,fdata->line-1);
			else
				buf->scroll2Line((GtkTextView*)page->view2,fdata->line-1);

			delete buf;
		}
#endif
}

VISIBLE void goToDefinition(Widget* widget,uPtr data)
{
#ifdef _USEQT5_
	DocumentClass	*document=getDocumentData(-1);
	functionData	*fdata=NULL;
	char			*selection;
	const char		*selectionptr;

	if(document==NULL)
		return;

	selection=strdup(document->textCursor().selectedText().toUtf8().constData());
	selectionptr=selection;

	fdata=getFunctionByName(selectionptr,true,true);
	if(fdata!=NULL)
		{
			//TODO//
//			history->savePosition();
			goToDefine(fdata);
			destroyData(fdata);
		}

	free(selection);
#else
	pageStruct*		page=getDocumentData(-1);
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	functionData*	fdata=NULL;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;
//TODO//
	fdata=getFunctionByName(selection,true,true);
	if(fdata!=NULL)
		{
			history->savePosition();
			goToDefine(fdata);
			destroyData(fdata);
		}
	return;
#endif
}

VISIBLE void findFile(Widget* widget,uPtr data)
{
#ifdef _USEQT5_
	DocumentClass	*document=getDocumentData(-1);
	char			*selection;
	StringSlice		slice;
	char			*filename=NULL;
	char			*filepath=NULL;
	char			*command;
	char			buffer[2048];
	char			*searchdir=NULL;
	FILE			*fp;

	if(document==NULL)
		return;

	selection=strdup(document->textCursor().block().text().toUtf8().constData());
	if(selection[0]!='#')
		return;

	filename=slice.sliceBetween(selection,(char*)"include ",NULL);

	if(slice.getResult()==0)
		{
			if(filename[0]=='<')
				searchdir=strdup("/usr/include");
			else
				searchdir=strdup(document->getDirname());

			asprintf(&filepath,"%s/%s",searchdir,slice.sliceLen(filename,1,strlen(filename)-2));
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
			debugFree(&searchdir,"searchdir filepath");
		}

	free(selection);
#else
	char*			command;
	char			buffer[2048];
	TextBuffer*		buf;
	char*			selection=NULL;
	char*			filename=NULL;
	char*			searchdir=NULL;
	char*			filepath=NULL;
	FILE*			fp;
	StringSlice*	slice;
	pageStruct*		page=getDocumentData(-1);

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
#endif
}

void gotoLine(Widget* widget,uPtr data)
{
	int	line=(int)(long)data;
#ifndef _USEQT5_
	pageStruct*	page=getDocumentData(-1);
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
#else
	DocumentClass	*doc=NULL;
	QTextBlock		block;
	QTextCursor		cursor;

	doc=getDocumentData(-1);
	if(doc==NULL)
		return;
	block=doc->document()->findBlockByNumber(line-1);
	cursor=doc->textCursor();
	cursor.setPosition(block.position());
	//doc->setFocus();
	doc->setTextCursor(cursor);
#endif
}

#ifndef _USEQT5_
void jumpToLineFromBar(GtkWidget* widget,gpointer data)
#else
void jumpToLineFromBar(const QString text)
#endif
{
#ifdef _USEQT5_
//TODO//
	printf("jumpToLineFromBar %s\n",text.toLocal8Bit().constData());
	theLineNum=atoi(text.toLocal8Bit().constData());
	gotoLine(NULL,(long)theLineNum);
#else
	theLineNum=atoi(gtk_entry_get_text((GtkEntry*)widget));
	gotoLine(NULL,(gpointer)(long)theLineNum);
#endif
}

int showLineEntry(void)
{
	gint	result=0;
#ifdef _USEQT5_
	bool	ok;
	QString	text=QInputDialog::getText(mainWindow,gettext("Go To Line"),gettext("Enter Line Number"),QLineEdit::Normal,"0",&ok);

	if ((ok==true) && (!text.isEmpty()))
		{
			theLineNum=text.toUInt();
			result=0;
		}
	else
		result=-1;
#else
	GtkWidget*	dialog;
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

#endif
	return(result);
}

VISIBLE void jumpToLine(Widget* widget,uPtr data)
{
#ifdef _USEQT5_
	if(showLineEntry()==0)
		gotoLine(NULL,(long)theLineNum);
#else
	if(showLineEntry()==GTK_RESPONSE_YES)
		gotoLine(NULL,(gpointer)(long)theLineNum);
#endif
}

VISIBLE void functionSearch(Widget* widget,uPtr data)
{
	functionData*	fdata;
	bool			ok;

#ifdef _USEQT5_
	QString			text=QInputDialog::getText(mainWindow,gettext("Find Function"),gettext("Enter Function Name"),QLineEdit::Normal,"",&ok);

	if ((ok==true) && (!text.isEmpty()))
		{
			if(functionSearchText!=NULL)
				debugFree(&functionSearchText,"functionSearchText functionSearch");
			functionSearchText=strdup(text.toUtf8().constData());

			ok=true;
			for(int j=0;j<2;j++)
				{
					fdata=getFunctionByName(functionSearchText,true,ok);
					if(fdata!=NULL)
						{
							goToDefine(fdata);
							destroyData(fdata);
							return;
						}
					ok=false;
				}
		}
#else

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
									return;
								}
							ok=false;
						}
				}
		}
#endif
}

#ifndef _USEQT5_
void jumpToMark(GtkWidget* widget,gpointer data)
#else
//TODO//
void jumpToMark(void)
#endif
{
#ifndef _USEQT5_
	GtkTextMark*	mark;
	pageStruct*		page;
	pageStruct*		checkpage;
	TextBuffer*		buf;
	history->savePosition();

	page=(pageStruct*)((bookMarksNew*)data)->page;
	mark=(GtkTextMark*)((bookMarksNew*)data)->mark;
	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	buf->scroll2Mark((GtkTextView*)page->view,mark);

	for(int loop=0; loop<gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook); loop++)
		{
			checkpage=getDocumentData(loop);
			if(checkpage==page)
				{
					gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,loop);
					return;
				}
		}
	delete buf;
#endif
}

#ifdef _BUILDDOCVIEWER_

struct docFileData
{
	char*	fileName;
	char*	filePath;
	int		lineNum;
	char*	hashTag;
	char*	sourceFile;
};

bool readFile(char *name)
{
#ifndef _USEQT5_
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
#endif
	return(true);
}

int getLineFromXML(char* xml)
{
	int				retline=1;
#ifndef _USEQT5_
	StringSlice*	slice=new StringSlice;

	char*			data;
	char*			xmldata=xml;

	data=slice->sliceBetween(xmldata,(char*)"Definition at line",(char*)"\">");
	if(slice->getResult()==0)
		retline=atoi(slice->sliceBetween(data,(char*)"#l",NULL));
	delete slice;
#endif
	return(retline);
}

char* getPathFromXML(char* xml)
{
#ifndef _USEQT5_
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
#endif
	return(NULL);
}

docFileData* getDoxyFileData(char* uri)
{
#ifndef _USEQT5_
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
#endif
	return(NULL);
}

#ifndef _USEQT5_
gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigationAction,WebKitWebPolicyDecision* policy_decision, gpointer user_data)
{
#ifndef _USEQT5_
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
			for(int j=0; j<gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook); j++)
				{
					page=getDocumentData(j);
					if((strcmp(page->realFilePath,doxydata->sourceFile)==0) || (strcmp(page->filePath,doxydata->sourceFile)==0))
						{
							gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,j);
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
#endif
}
#endif

#endif
