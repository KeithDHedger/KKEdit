/******************************************************
*
*     ©keithhedger Wed 18 Jun 17:14:17 BST 2014
*     kdhedger68713@gmail.com
*
*     backclass.cpp
* 
******************************************************/

#include "globals.h"

HistoryClass::HistoryClass()
{
	buf=new TextBuffer;
}

HistoryClass::~HistoryClass()
{
	delete buf;
}

void HistoryClass::getThisPoint(void)
{
	GtkWidget*	pageBox;
	int			tabNum=gtk_notebook_get_current_page(notebook);

	pageBox=gtk_notebook_get_nth_page(notebook,tabNum);
	if(pageBox==NULL)
		savedPage=NULL;
	else
		savedPage=(pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata");
}

pageStruct* HistoryClass::getPage(void)
{
	return(savedPage);
}

void HistoryClass::setPage(pageStruct* page)
{
	savedPage=page;
}

TextBuffer* HistoryClass::getTextBuffer(void)
{
	return(buf);
}

void HistoryClass::savePosition(void)
{
	getThisPoint();

	buf->textBuffer=(GtkTextBuffer*)savedPage->buffer;
	buf->getLineData();
	gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)savedPage->buffer,"back-mark",&buf->cursorPos);
}

int HistoryClass::getTabNumForPage(void)
{
	pageStruct*	page;
	int			numpages=gtk_notebook_get_n_pages(notebook);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if (page==savedPage)
				return(loop);
		}
	return(0);
}
















