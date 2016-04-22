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
	this->notebook=nb;
	this->canReturn=true;
	this->saveCnt=0;
	for(int j=0;j<(MAXHIST+4);j++)
		{
			this->savedPages[j].pageID=-1;
			this->savedPages[j].filePath=NULL;
			this->savedPages[j].lineNumber=0;
		}
	ERRDATA
}

HistoryClass::~HistoryClass()
{
	for(int j=0;j<(MAXHIST+4);j++)
		{
			if(this->savedPages[j].filePath!=NULL)
				debugFree(&(this->savedPages[j].filePath));
		}
}

void HistoryClass::goBack(void)
{
	this->saveCnt--;
	if(this->saveCnt<0)
		this->saveCnt=0;
}

void HistoryClass::goForward(void)
{
	this->saveCnt++;
	if(this->saveCnt>=MAXHIST)
		this->saveCnt=MAXHIST-1;
}

void HistoryClass::goToPos(void)
{
	TextBuffer	*buf;
	pageStruct	*page;

	if(this->savedPages[this->saveCnt].pageID==-1)
		return;

	for(int j=0;j<gtk_notebook_get_n_pages(this->notebook);j++)
		{
			page=(pageStruct*)g_object_get_data((GObject*)gtk_notebook_get_nth_page(this->notebook,j),"pagedata");
			if(page!=NULL)
				{
					if(page->pageID==this->savedPages[this->saveCnt].pageID)
						{
							gtk_notebook_set_current_page(this->notebook,j);
							buf=new TextBuffer((GtkTextBuffer*)page->buffer);
							buf->scroll2Line((GtkTextView*)page->view,this->savedPages[this->saveCnt].lineNumber-1,true);
							delete buf;
							return;
						}
				}
		}
}

void HistoryClass::saveLastPosAndStop(void)
{
	this->saveLastPos();
	this->savedPages[this->saveCnt+1].pageID=-1;
	this->savedPages[this->saveCnt].pageID=-1;
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

	if(saveCnt>0)
		{
			if((this->savedPages[this->saveCnt-1].pageID==page->pageID) && (this->savedPages[this->saveCnt-1].lineNumber==buf->lineNum))
				{
					delete buf;
					return;
				}
		}

	this->savedPages[this->saveCnt].pageID=page->pageID;
	this->savedPages[this->saveCnt].lineNumber=buf->lineNum;
	if(this->savedPages[this->saveCnt].filePath!=NULL)
		{
			debugFree(&(this->savedPages[this->saveCnt].filePath));
			if(page->filePath!=NULL)
				asprintf(&this->savedPages[this->saveCnt].filePath,"%s",page->filePath);
		}

	this->goForward();
	this->canReturn=true;
	delete buf;
}

bool HistoryClass::canGoBack(void)
{
	ERRDATA return(this->canReturn);
}



