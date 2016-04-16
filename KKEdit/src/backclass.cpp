/******************************************************
*
*     ©keithhedger Wed 18 Jun 17:14:17 BST 2014
*     kdhedger68713@gmail.com
*
*     backclass.cpp
* 
******************************************************/

#include "kkedit-includes.h"

HistoryClass::HistoryClass(GtkNotebook *nb)
{
	ERRDATA
	buf=new TextBuffer;
	this->savedPage=NULL;
	this->notebook=nb;
	this->currentPos.pageID=-1;
	this->lastPos.pageID=-1;
	this->currentPos.child=NULL;
	this->lastPos.child=NULL;
	this->goBack=true;
	ERRDATA
}

HistoryClass::~HistoryClass()
{
	ERRDATA delete this->buf;
}

void HistoryClass::swapPos(void)
{
	this->lastPos=this->currentPos;
}

void HistoryClass::saveCurrentPos(void)
{
	pageStruct	*page;
	GtkWidget	*child;
	TextBuffer	*buf;

	child=gtk_notebook_get_nth_page(mainNotebook,gtk_notebook_get_current_page(mainNotebook));
	if(child==NULL)
		return;
	else
		page=(pageStruct*)g_object_get_data((GObject*)child,"pagedata");

	if(page==NULL)
		return;

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	this->currentPos.pageID=page->pageID;
	this->currentPos.tabName=page->fileName;
	this->currentPos.lineNumber=buf->lineNum;
	this->currentPos.child=child;
	this->goBack=true;

	delete buf;
}

void HistoryClass::goBackToPos(void)
{
	int	topagenum=-1;
	TextBuffer	*buf;
	pageStruct	*page;
	
	if(this->lastPos.child==NULL)
		return;

	page=(pageStruct*)g_object_get_data((GObject*)this->lastPos.child,"pagedata");
	if(page==NULL)
		return;

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);
	topagenum=gtk_notebook_page_num(this->notebook,this->lastPos.child);
	if(topagenum!=-1)
		{
			gtk_notebook_set_current_page(this->notebook,topagenum);
			buf->scroll2Line((GtkTextView*)page->view,this->lastPos.lineNumber-1,true);
			this->lastPos=this->currentPos;
		}
	delete buf;
}

void HistoryClass::saveLastPos(void)
{
	pageStruct	*page;
	GtkWidget	*child;
	TextBuffer	*buf;

	child=gtk_notebook_get_nth_page(mainNotebook,gtk_notebook_get_current_page(mainNotebook));
	if(child==NULL)
		return;
	else
		page=(pageStruct*)g_object_get_data((GObject*)child,"pagedata");

	if(page==NULL)
		return;

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	this->lastPos.pageID=page->pageID;
	this->lastPos.tabName=page->fileName;
	this->lastPos.lineNumber=buf->lineNum;
	this->lastPos.child=child;
	this->goBack=true;

	delete buf;
}

void HistoryClass::setCurrentPos(void)
{
	pageStruct	*page;
	GtkWidget	*child;
	TextBuffer	*buf;

	child=gtk_notebook_get_nth_page(mainNotebook,gtk_notebook_get_current_page(mainNotebook));
	if(child==NULL)
		return;
	else
		page=(pageStruct*)g_object_get_data((GObject*)child,"pagedata");

	if(page==NULL)
		return;

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	this->currentPos.pageID=page->pageID;
	this->currentPos.tabName=page->fileName;
	this->currentPos.lineNumber=buf->lineNum;
	this->currentPos.child=child;
	this->goBack=true;
	if(this->lastPos.child==NULL)
		this->lastPos=this->currentPos;
	delete buf;
}

void HistoryClass::getThisPoint(void)
{
	ERRDATA
	GtkWidget	*pageBox;
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

pageStruct *HistoryClass::getPage(void)
{
	ERRDATA return(savedPage);
}

bool HistoryClass::canGoBack(void)
{
	ERRDATA return(this->goBack);
}

void HistoryClass::setPage(pageStruct *page)
{
	ERRDATA
	savedPage=page;
}

TextBuffer *HistoryClass::getTextBuffer(void)
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
			ERRDATA return(true);
		}

	this->goBack=false;
	ERRDATA return(false);
}

int HistoryClass::getTabNumForPage(void)
{
	ERRDATA
	pageStruct	*page;
	int			numpages=gtk_notebook_get_n_pages(mainNotebook);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if(page==savedPage)
				{
					this->goBack=true;
					ERRDATA return(loop);
				}
		}

	this->savedPage=NULL;
	this->goBack=false;
	ERRDATA return(-1);
}


