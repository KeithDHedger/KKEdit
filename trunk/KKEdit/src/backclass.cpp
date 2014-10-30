/******************************************************
*
*     ©keithhedger Wed 18 Jun 17:14:17 BST 2014
*     kdhedger68713@gmail.com
*
*     backclass.cpp
* 
******************************************************/

#include "kkedit-includes.h"

HistoryClass::HistoryClass()
{
	buf=new TextBuffer;
	this->savedPage=NULL;
}

HistoryClass::~HistoryClass()
{
	delete buf;
}

void HistoryClass::getThisPoint(void)
{
	GtkWidget*	pageBox;
	int			tabNum=gtk_notebook_get_current_page(notebook);

	this->goBack=true;
	pageBox=gtk_notebook_get_nth_page(notebook,tabNum);
	if(pageBox==NULL)
		savedPage=NULL;
	else
		savedPage=(pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata");

	if(this->savedPage==NULL)
		this->goBack=false;
}

pageStruct* HistoryClass::getPage(void)
{
	return(savedPage);
}

bool HistoryClass::canGoBack(void)
{
	return(this->goBack);
}

void HistoryClass::setPage(pageStruct* page)
{
	savedPage=page;
}

TextBuffer* HistoryClass::getTextBuffer(void)
{
	return(buf);
}

bool HistoryClass::savePosition(void)
{
	getThisPoint();

	if(this->savedPage!=NULL)
		{
			buf->textBuffer=(GtkTextBuffer*)savedPage->buffer;
			buf->getLineData();
			gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)savedPage->buffer,"back-mark",&buf->cursorPos);
			this->goBack=true;
			setSensitive();
			return(true);
		}

	this->goBack=false;
	setSensitive();
	return(false);
}

int HistoryClass::getTabNumForPage(void)
{
	pageStruct*	page;
	int			numpages=gtk_notebook_get_n_pages(notebook);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if (page==savedPage)
				{
					this->goBack=true;
					return(loop);
				}
		}

	this->savedPage=NULL;
	this->goBack=false;
	return(-1);
}


