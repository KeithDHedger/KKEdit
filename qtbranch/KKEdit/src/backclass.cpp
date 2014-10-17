/******************************************************
*
*     ©keithhedger Wed 18 Jun 17:14:17 BST 2014
*     kdhedger68713@gmail.com
*
*     backclass.cpp
* 
******************************************************/
#if 0
#include "kkedit-includes.h"

HistoryClass::HistoryClass()
{
#if 0
	buf=new TextBuffer;
	this->savedPage=NULL;
#endif
}

HistoryClass::~HistoryClass()
{
#if 0
	delete buf;
#endif
}

void HistoryClass::getThisPoint(void)
{
#if 0
	Widget*	pageBox;
	int			tabNum=gtk_notebook_get_current_page(notebook);

	pageBox=gtk_notebook_get_nth_page(notebook,tabNum);
	if(pageBox==NULL)
		savedPage=NULL;
	else
		savedPage=(pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata");
#endif
}

pageStruct* HistoryClass::getPage(void)
{
#if 0
	return(savedPage);
#endif
}

void HistoryClass::setPage(pageStruct* page)
{
#if 0
	savedPage=page;
#endif
}

TextBuffer* HistoryClass::getTextBuffer(void)
{
#if 0
	return(buf);
#endif
}

void HistoryClass::savePosition(void)
{
#if 0
	getThisPoint();

	buf->textBuffer=(GtkTextBuffer*)savedPage->buffer;
	buf->getLineData();
	gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)savedPage->buffer,"back-mark",&buf->cursorPos);
#endif
}

int HistoryClass::getTabNumForPage(void)
{
#if 0
	pageStruct*	page;
	int			numpages=gtk_notebook_get_n_pages(notebook);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if (page==savedPage)
				return(loop);
		}
	return(0);
#endif
}

#endif
