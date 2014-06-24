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
#include <libgen.h>

#include "globals.h"
#include "files.h"
#include "guis.h"
#include "callbacks.h"
#include "sliceclass.h"

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
	for(int loop=0;loop<gtk_notebook_get_n_pages(notebook);loop++)
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

gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigationAction,WebKitWebPolicyDecision* policy_decision, gpointer user_data)
{

	int				mod=-1;
	const char*		uri;
	const char*		linenum=NULL;
	int				line;
	const char*		filepath=NULL;
	const char*		filename;
	pageStruct*		page;
	TextBuffer*		buf;
	char*			pwd;
	char*			loadfile;

	mod=webkit_web_navigation_action_get_modifier_state(navigationAction);
	if(mod&GDK_SHIFT_MASK)
		{
			uri=webkit_network_request_get_uri(request);
			linenum=globalSlice->sliceBetween((char*)uri,(char*)"#l",NULL);
//clicked line number
			if(linenum!=NULL)
				{
					line=atoi(linenum);
					filepath=globalSlice->sliceBetween((char*)uri,(char*)"file://",(char*)"#l");
					filename=(char*)basename((char*)filepath);
					filename=globalSlice->replaceAllSlice((char*)filename,(char*)"_source",(char*)"",false);
					filename=globalSlice->replaceAllSlice((char*)filename,(char*)".html",(char*)"",false);
					filename=globalSlice->replaceAllSlice((char*)filename,(char*)"_8",(char*)".",false);
//check in open tabs
					buf=new TextBuffer;
					for(int j=0;j<gtk_notebook_get_n_pages(notebook);j++)
						{
							page=getPageStructPtr(j);
							if(strcmp(page->fileName,filename)==0)
								{
									gtk_notebook_set_current_page(notebook,j);
									buf->textBuffer=(GtkTextBuffer*)page->buffer;
									buf->scroll2Line((GtkTextView*)page->view,line-1);
									delete buf;
									return(false);
								}
						}
//try to open file
					pwd=get_current_dir_name();
					if(pwd!=NULL)
						{
							asprintf(&loadfile,"%s/%s",pwd,filename);
							openFile(loadfile,line,false);
							free(loadfile);
							free(pwd);
							return(false);
						}
				}
			else
				{
//just file name
//check if tab open
					filepath=globalSlice->replaceSlice((char*)uri,(char*)"file://",(char*)"");
					filename=(char*)basename((char*)filepath);
					filename=globalSlice->replaceAllSlice((char*)filename,(char*)"_source",(char*)"",false);
					filename=globalSlice->replaceAllSlice((char*)filename,(char*)".html",(char*)"",false);
					filename=globalSlice->replaceAllSlice((char*)filename,(char*)"_8",(char*)".",false);
				
					for(int j=0;j<gtk_notebook_get_n_pages(notebook);j++)
						{
							page=getPageStructPtr(j);
							if((page!=NULL) && (strcmp(page->fileName,filename)==0))
								{
									gtk_notebook_set_current_page(notebook,j);
									return(false);
								}
						}
//try to open file
					pwd=get_current_dir_name();
					if(pwd!=NULL)
						{
							asprintf(&loadfile,"%s/%s",pwd,filename);
							openFile(loadfile,-1,false);
							free(loadfile);
							free(pwd);
						}
				}
		}

	return(false);
}
#endif
