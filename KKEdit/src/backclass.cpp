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
	this->buf=new TextBuffer;
}

HistoryClass::~HistoryClass()
{
	delete this->buf;
}

void HistoryClass::getThisPoint(void)
{
	int			thispage;
	GtkWidget*	pageBox;

	thispage=gtk_notebook_get_current_page(notebook);

	pageBox=gtk_notebook_get_nth_page(notebook,thispage);
	if(pageBox==NULL)
		this->page=NULL;
	else
		this->page=(pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata");

	this->buf->getLineData();
	this->lineNum=this->buf->lineNum;
}

pageStruct* HistoryClass::getPage(void)
{
	return(page);
}

TextBuffer* HistoryClass::getTextBuffer(void)
{
	return(buf);
}
