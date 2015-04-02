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
	ERRDATA
	buf=new TextBuffer;
	this->savedPage=NULL;
	ERRDATA
}

HistoryClass::~HistoryClass()
{
	ERRDATA delete buf;
}

void HistoryClass::getThisPoint(void)
{
	ERRDATA
	GtkWidget*	pageBox;
	int			tabNum=gtk_notebook_get_current_page(mainNotebook);

	this->goBack=true;
	pageBox=gtk_notebook_get_nth_page(mainNotebook,tabNum);
	if(pageBox==NULL)
		savedPage=NULL;
	else
		savedPage=(pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata");

	if(this->savedPage==NULL)
		this->goBack=false;
	ERRDATA
}

pageStruct* HistoryClass::getPage(void)
{
	ERRDATA return(savedPage);
}

bool HistoryClass::canGoBack(void)
{
	ERRDATA return(this->goBack);
}

void HistoryClass::setPage(pageStruct* page)
{
	ERRDATA
	savedPage=page;
}

TextBuffer* HistoryClass::getTextBuffer(void)
{
	ERRDATA return(buf);
}

bool HistoryClass::savePosition(void)
{
	ERRDATA
	getThisPoint();

	if(this->savedPage!=NULL)
		{
			buf->textBuffer=(GtkTextBuffer*)savedPage->buffer;
			buf->getLineData();
			gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)savedPage->buffer,"back-mark",&buf->cursorPos);
			this->goBack=true;
			setSensitive();
			ERRDATA return(true);
		}

	this->goBack=false;
	setSensitive();
	ERRDATA return(false);
}

int HistoryClass::getTabNumForPage(void)
{
	ERRDATA
	pageStruct*	page;
	int			numpages=gtk_notebook_get_n_pages(mainNotebook);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if (page==savedPage)
				{
					this->goBack=true;
					ERRDATA return(loop);
				}
		}

	this->savedPage=NULL;
	this->goBack=false;
	ERRDATA return(-1);
}


