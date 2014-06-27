/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include <gtksourceview/gtksourceiter.h>
#include <libgen.h>
#include <ctype.h>

#include "guis.h"
#include "callbacks.h"
#include "searchcallbacks.h"

int	theLineNum=0;
int marknum=0;

void goToDefine(functionData* fdata)
{
	pageStruct*	page;
	TextBuffer*	buf;

	if(fdata->intab==-1)
		{
			openFile(fdata->file,fdata->line-1,true);
		}
	else
		{
			page=getPageStructPtr(fdata->intab);
			gtk_notebook_set_current_page(notebook,fdata->intab);
			buf=new TextBuffer((GtkTextBuffer*)page->buffer);

			if(page->inTop==true)
				buf->scroll2Line((GtkTextView*)page->view,fdata->line-1);
			else
				buf->scroll2Line((GtkTextView*)page->view2,fdata->line-1);

			delete buf;
		}
}

void goToDefinition(GtkWidget* widget,gpointer data)
{
	pageStruct*		page=getPageStructPtr(-1);
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

	fdata=getFunctionByName(selection,true);
	if(fdata!=NULL)
		{
			history->savePosition();
			goToDefine(fdata);
			destroyData(fdata);
		}
	return;
}

void findFile(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char		strarg[2048];
	char*		filename;
	char*		command;
	gchar*		stdout=NULL;
	gchar*		stderr=NULL;
	gint		retval=0;
	char*		lineptr;
	char		buffer[2048];
	char*		searchdir;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	sscanf(selection,"#include %s",(char*)&strarg);
	strarg[strlen(strarg)-1]=0;

	if(strarg[0]=='<')
		searchdir=strdup("/usr/include");
	else
		searchdir=strdup(g_path_get_dirname(page->filePath));

	filename=g_path_get_basename(&strarg[1]);
	asprintf(&command,"find \"%s\" -name \"%s\"",searchdir,filename);
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			lineptr=stdout;
			while (lineptr!=NULL)
				{
					sscanf (lineptr,"%s",(char*)&buffer);
					lineptr=strchr(lineptr,'\n');
					if (lineptr!=NULL)
						lineptr++;
					openFile(buffer,0,true);
				}
			g_free(stdout);
			g_free(stderr);
		}

	g_free(searchdir);
	g_free(filename);
	g_free(command);
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

	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,"Enter Line Number");

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Go To Line");

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

void jumpToLine(GtkWidget* widget,gpointer data)
{
	if(showLineEntry()==GTK_RESPONSE_YES)
		gotoLine(NULL,(gpointer)(long)theLineNum);
}

void functionSearch(GtkWidget* widget,gpointer data)
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
	for(int loop=0; loop<gtk_notebook_get_n_pages(notebook); loop++)
		{
			checkpage=getPageStructPtr(loop);
			if(checkpage==page)
				{
					gtk_notebook_set_current_page(notebook,loop);
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
};

docFileData* getDoxyFileData(char* uri)
{
	FILE*	file;
	char	line[1024];
	char*	command;
	int		linenumber;
	char*	linetag=NULL;
//	char*	tpath=NULL;
//	char*	pathuri=NULL;

	char*	unhashedline=NULL;
	char*	filepath=NULL;
	char*	document=NULL;
	char*	linestring=NULL;

	globalSlice->setReturnDupString(true);
	docFileData* doc=(docFileData*)malloc(sizeof(docFileData));

	doc->fileName="XXX";
	doc->filePath="XXX";
	doc->lineNum=1;
	doc->hashTag="XXX";

	linetag=globalSlice->sliceInclude(uri,(char*)"#",NULL);
	if(globalSlice->getResult()==0)
		unhashedline=globalSlice->deleteSlice(uri,linetag);
	else
		{
			unhashedline=strdup(uri);
			linetag=NULL;
		}

	filepath=g_filename_from_uri(unhashedline,NULL,NULL);
	asprintf(&command,"head \"%s\"|grep -i \"<title>\"",filepath);
	file=popen(command,"r");
	while(fgets(line,1024,file))
		{
			document=NULL;
			if(strstr(line,(char*)" File Reference<")!=NULL)
				document=globalSlice->sliceBetween(line,(char*)": ",(char*)" File Reference<");
			if(strstr(line,(char*)" Source File<")!=NULL)
				document=globalSlice->sliceBetween(line,(char*)": ",(char*)" Source File<");

			globalSlice->setReturnDupString(false);
			if(document!=NULL)
				{
					asprintf(&doc->filePath,"%s/%s",dirname(dirname(filepath)),globalSlice->decodeHtml(document));
					doc->fileName=strdup(basename(doc->filePath));
				}
			else
				{
//gives
//<p>Definition at line <a class="el" href="bonesGnome_2_3_4PROJ_3_4_2src_2main_8cpp_source.html#l00011">11</a> of file <a class="el" href="bonesGnome_2_3_4PROJ_3_4_2src_2main_8cpp_source.html">main.cpp</a>.</p>
					asprintf(&doc->filePath,"%s",unhashedline);
					doc->fileName=strdup(globalSlice->sliceBetween(line,(char*)": ",(char*)"</title>"));
				}
			break;
		}

	pclose(file);
	globalSlice->setReturnDupString(false);

	if(linetag!=NULL)
		{
			linestring=globalSlice->sliceBetween(linetag,(char*)"#l",NULL);
			if(globalSlice->getResult()==0)
				doc->lineNum=atoi(linestring);
			else
				doc->hashTag=strdup(globalSlice->sliceBetween(linetag,(char*)"#",NULL));
		}

	globalSlice->setReturnDupString(true);
	return(doc);
}

gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigationAction,WebKitWebPolicyDecision* policy_decision, gpointer user_data)
{
	int				mod=-1;
	const char*		uri;
	const char*		linenum=NULL;
	int				line;
	const char*		filepath=NULL;
	pageStruct*		page;
	TextBuffer*		buf;
	char*			convertedname=NULL;
	docFileData*	doxydata;

	mod=webkit_web_navigation_action_get_modifier_state(navigationAction);
	if(mod&GDK_SHIFT_MASK)
		{
			uri=webkit_network_request_get_uri(request);
			doxydata=getDoxyFileData((char*)uri);
			printf("path=%s\nname=%s\nline=%i\nhahtag=%s\n",doxydata->filePath,doxydata->fileName,doxydata->lineNum,doxydata->hashTag);
			return(false);
			convertedname=unEscapeFileNAme((char*)uri);
			linenum=globalSlice->sliceBetween(convertedname,(char*)"#l",NULL);

			if(globalSlice->getResult()==0)
				{
//clicked line number
					line=atoi(linenum);
					filepath=globalSlice->sliceBetween(convertedname,(char*)"file://",(char*)"#l");
				}
			else
				{
					globalSlice->setReturnDupString(false);
					line=1;
					filepath=globalSlice->sliceBetween(convertedname,(char*)"file://",NULL);
				}

			free(convertedname);

			filepath=globalSlice->replaceSlice((char*)filepath,(char*)"/html/",(char*)"/");
			filepath=globalSlice->replaceSlice((char*)filepath,(char*)"Source.html",(char*)"");
			filepath=globalSlice->replaceSlice((char*)filepath,(char*)".html",(char*)"");
			globalSlice->setReturnDupString(true);

//check in open tabs
			buf=new TextBuffer;
			for(int j=0; j<gtk_notebook_get_n_pages(notebook); j++)
				{
					page=getPageStructPtr(j);
					if(strcmp(page->filePath,filepath)==0)
						{
							gtk_notebook_set_current_page(notebook,j);
							buf->textBuffer=(GtkTextBuffer*)page->buffer;
							buf->scroll2Line((GtkTextView*)page->view,line-1);
							delete buf;
							return(false);
						}
				}
//try to open file
			if(openFile(filepath,line,false)==false)
				{
#if 0
					FILE*		headfile;
	char		line[1024];
	char*		command;
	int linenumber;
					if(thePage!=NULL)
						free(thePage);
					globalSlice->setReturnDupString(false);
					filepath=globalSlice->sliceBetween((char*)uri,(char*)"file://",(char*)"#");
printf("XXX\n%s\nXXXX\n",filepath);
//<p>Definition at line <a class="el" href="callbacks_8cpp_source.html#l00180">180</a> of file <a class="el" href="callbacks_8cpp_source.html">callbacks.cpp</a>.</p>

					asprintf(&command,"cat \"%s\"|grep \"Definition at line\"",filepath);
					headfile=popen(command,"r");
					while(fgets(line,1024,headfile))
						{
							printf("%s\n",line);
							filepath=globalSlice->sliceBetween(line,(char*)"href=\"",(char*)"\"");
							linenum=(char*)globalSlice->sliceBetween((char*)filepath,(char*)"#l",NULL);
							linenumber=atoi(linenum);
							filepath=globalSlice->sliceBetween(line,(char*)"href=\"",(char*)"#");
							printf("href='%s' @ line %i\n",filepath,linenumber);
							printf("%s\n",uri);
							printf("%s\n",(char*)dirname((char*)uri));
						}
					pclose(headfile);
					asprintf(&command,"%s/%s#l%i",dirname((char*)uri),filepath,linenumber);
					printf("XXXXXXXXXxx\n\n%s\n\nXXXXXXXXXXX\n",command);
					//asprintf(&thePage,"%s",uri);
					//showDocView(USEURI,thePage);
#endif
				}
			//return(false);

		}
	return(false);
}
#endif

