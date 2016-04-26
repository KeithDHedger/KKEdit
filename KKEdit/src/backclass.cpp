/******************************************************
*
*     ©keithhedger Wed 18 Jun 17:14:17 BST 2014
*     kdhedger68713@gmail.com
*
*     backclass.cpp
* 
******************************************************/

#include "kkedit-includes.h"

HistoryClass::HistoryClass(GtkNotebook *nb,unsigned maxhist)
{
	ERRDATA
	this->notebook=nb;
	this->canReturn=true;
	this->saveCnt=0;
	this->maxHist=maxhist;
	this->savedPages=new historyData[maxhist+4];
	for(unsigned j=0;j<(maxhist+4);j++)
		{
			this->savedPages[j].pageID=0;
			this->savedPages[j].filePath=NULL;
			this->savedPages[j].lineNumber=0;
			this->savedPages[j].tabName=NULL;
		}
	ERRDATA
}

HistoryClass::~HistoryClass()
{
	for(unsigned j=0;j<(this->maxHist+4);j++)
		{
			if(this->savedPages[j].filePath!=NULL)
				debugFree(&(this->savedPages[j].filePath));
		}
}

void HistoryClass::setHistMenu(GtkWidget *menu)
{
	this->historyMenu=menu;
}

GtkWidget* HistoryClass::getHistMenu(void)
{
	return(this->historyMenu);
}

void HistoryClass::goBack(void)
{
	if(this->saveCnt==0)
		return;
	this->saveCnt--;
}

void HistoryClass::goForward(void)
{
	if(this->saveCnt<this->maxHist)
		this->saveCnt++;
}

void HistoryClass::goToPos(void)
{
	TextBuffer	*buf;
	pageStruct	*page;

	if(this->savedPages[this->saveCnt].pageID==0)
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

historyData* HistoryClass::getHistory(int num)
{
	return(&(this->savedPages[num]));
}

unsigned HistoryClass::getMaxHist(void)
{
	return(this->maxHist);
}

unsigned HistoryClass::getSaveCnt(void)
{
	return(this->saveCnt);
}

void HistoryClass::setSaveCnt(int num)
{
	this->saveCnt=num;
}

//void HistoryClass::menuJumpBack(GtkWidget *widget,gpointer data)
//{
//	TextBuffer	*buf;
//	pageStruct	*page=NULL;
//	pageStruct	*checkpage=NULL;
//	historyData	*hist=(historyData*)data;
//	unsigned	pageid;
//
//	pageid=hist->pageID;
//	page=getPageStructByID(pageid);
//	if(page!=NULL)
//		{
//			for(int loop=0;loop<gtk_notebook_get_n_pages(mainNotebook);loop++)
//				{
//					checkpage=getPageStructPtr(loop);
//					if(checkpage==page)
//						{
//							buf=new TextBuffer((GtkTextBuffer*)page->buffer);
//							gtk_notebook_set_current_page(mainNotebook,loop);
//							gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&buf->cursorPos);
//							buf->scroll2Line((GtkTextView*)page->view,hist->lineNumber,true);
//							ERRDATA delete buf;
//							ERRDATA return;
//				}
//
//		}
//
//printf("here....%i\n",(int)(long)data);
//		}
//}

void HistoryClass::appendLocation(void)
{
	GtkWidget	*menu;
	char		*label;

	sinkReturn=asprintf(&label,"%s - %i",this->savedPages[this->saveCnt].tabName,this->savedPages[this->saveCnt].lineNumber);
	menu=gtk_menu_item_new_with_label(label);
	debugFree(&label);
	gtk_menu_shell_append(GTK_MENU_SHELL(this->historyMenu),menu);
	g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuJumpBack),(void*)(long)(this->saveCnt));
	gtk_widget_show_all(this->historyMenu);
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
				sinkReturn=asprintf(&this->savedPages[this->saveCnt].filePath,"%s",page->filePath);
		}
	if(page->fileName!=NULL)
		sinkReturn=asprintf(&this->savedPages[this->saveCnt].tabName,"%s",page->fileName);

	this->appendLocation();
	this->goForward();
	this->canReturn=true;
	delete buf;
}

bool HistoryClass::canGoBack(void)
{
	ERRDATA return(this->canReturn);
}



