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
	this->backOK=false;
	this->forwardOK=false;
	this->saveCnt=0;
	this->maxHist=maxhist;
	this->maxHist=4;
	this->savedPages=new historyData[maxhist+4];
	for(unsigned j=0;j<(maxhist+4);j++)
		{
			this->savedPages[j].pageID=0;
			this->savedPages[j].filePath=NULL;
			this->savedPages[j].lineNumber=0;
			this->savedPages[j].tabName=NULL;
			this->savedPages[j].menuLabel=NULL;
		}
	this->historyBackMenu=NULL;
	this->historyForwardMenu=NULL;
}

HistoryClass::~HistoryClass()
{
	for(unsigned j=0;j<(this->maxHist+4);j++)
		{
			if(this->savedPages[j].filePath!=NULL)
				debugFree(&(this->savedPages[j].filePath));
		}
}

void HistoryClass::setHistForwardMenu(GtkWidget *menu)
{
	this->historyForwardMenu=menu;
}

GtkWidget* HistoryClass::getHistForwardMenu(void)
{
	return(this->historyForwardMenu);
}

void HistoryClass::setHistBackMenu(GtkWidget *menu)
{
	this->historyBackMenu=menu;
}

GtkWidget* HistoryClass::getHistBackMenu(void)
{
	return(this->historyBackMenu);
}

void HistoryClass::goBack(void)
{
	if(this->saveCnt==0)
		return;
	this->saveCnt--;
}

void HistoryClass::copyPage(int topage,int frompage)
{
	this->savedPages[topage].filePath=this->savedPages[frompage].filePath;
	this->savedPages[topage].tabName=this->savedPages[frompage].tabName;
	this->savedPages[topage].menuLabel=this->savedPages[frompage].menuLabel;
	this->savedPages[topage].pageID=this->savedPages[frompage].pageID;
	this->savedPages[topage].lineNumber=this->savedPages[frompage].lineNumber;

	this->savedPages[frompage].pageID=0;
	this->savedPages[frompage].lineNumber=0;
	this->savedPages[frompage].filePath=NULL;
	this->savedPages[frompage].tabName=NULL;
	this->savedPages[frompage].menuLabel=NULL;
}

void HistoryClass::goForward(void)
{
	unsigned	j;

	this->saveCnt++;
	if(this->saveCnt>this->maxHist)
		{
			debugFree(&(this->savedPages[0].filePath));
			debugFree(&(this->savedPages[0].tabName));
			debugFree(&(this->savedPages[0].menuLabel));
			for(j=1;j<this->maxHist+2;j++)
				this->copyPage(j-1,j);

			this->savedPages[j].pageID=0;
			this->savedPages[j].lineNumber=0;
			this->savedPages[j].filePath=NULL;
			this->savedPages[j].tabName=NULL;
			this->savedPages[j].menuLabel=NULL;

			this->saveCnt=this->maxHist;
		}
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
	for(unsigned j=this->saveCnt;j<this->maxHist+2;j++)
		{
			this->savedPages[j].pageID=0;
			debugFree(&(this->savedPages[j].menuLabel));
			debugFree(&(this->savedPages[j].tabName));
			debugFree(&(this->savedPages[j].filePath));
		}
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
	if(num>=0)
		this->saveCnt=num;
	else
		this->saveCnt=0;
}

void HistoryClass::redoMenus(void)
{
	GtkWidget	*menu;
	GList		*childs;
	GtkWidget	*widgdata;
	char		*label;
	unsigned	cntup;

//back menu
	childs=gtk_container_get_children ((GtkContainer*)this->historyBackMenu);
	for(unsigned j=0;j<g_list_length(childs);j++)
		{
			widgdata=(GtkWidget*)g_list_nth_data(childs,j);
			gtk_widget_destroy(widgdata);
		}

	if(this->historyBackMenu!=NULL)
		{
			for(unsigned j=0;j<this->saveCnt;j++)
				{
					sinkReturn=asprintf(&label,"%i:%s - %s",this->savedPages[j].lineNumber,this->savedPages[j].tabName,this->savedPages[j].menuLabel);
					menu=gtk_menu_item_new_with_label(label);
					debugFree(&label);
					gtk_menu_shell_append(GTK_MENU_SHELL(this->historyBackMenu),menu);
					g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuJumpBack),(void*)(long)(j));
				}
			gtk_widget_show_all(this->historyBackMenu);
		}

//forward menu
	childs=gtk_container_get_children ((GtkContainer*)this->historyForwardMenu);
	for(unsigned j=0;j<g_list_length(childs);j++)
		{
			widgdata=(GtkWidget*)g_list_nth_data(childs,j);
			gtk_widget_destroy(widgdata);
		}

	if(this->historyForwardMenu!=NULL)
		{
			cntup=this->saveCnt+1;
			while(this->savedPages[cntup].pageID!=0)
				{
					sinkReturn=asprintf(&label,"%i:%s - %s",this->savedPages[cntup].lineNumber,this->savedPages[cntup].tabName,this->savedPages[cntup].menuLabel);
					menu=gtk_menu_item_new_with_label(label);
					debugFree(&label);
					gtk_menu_shell_append(GTK_MENU_SHELL(this->historyForwardMenu),menu);
					g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuJumpBack),(void*)(long)(cntup));
					cntup++;
				}
			gtk_widget_show_all(this->historyForwardMenu);
		}

	if(this->saveCnt==0)
		this->backOK=false;
	else
		this->backOK=true;

	if(this->savedPages[this->saveCnt+1].pageID==0)
		this->forwardOK=false;
	else
		this->forwardOK=true;

	gtk_widget_set_sensitive((GtkWidget*)backButton,this->backOK);
	gtk_widget_set_sensitive((GtkWidget*)forwardButton,this->forwardOK);
	gtk_widget_set_sensitive((GtkWidget*)goBackMenu,this->backOK);
	gtk_widget_set_sensitive((GtkWidget*)goForwardMenu,this->forwardOK);
}

void HistoryClass::saveLastPos(void)
{
	pageStruct	*page;
	GtkWidget	*child;
	TextBuffer	*buf;
	char		*linetext;

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
	debugFree(&(this->savedPages[this->saveCnt].menuLabel));
	debugFree(&(this->savedPages[this->saveCnt].tabName));
	debugFree(&(this->savedPages[this->saveCnt].filePath));

//TODO//
	if(this->savedPages[this->saveCnt].filePath!=NULL)
		{
			if(page->filePath!=NULL)
				sinkReturn=asprintf(&this->savedPages[this->saveCnt].filePath,"%s",page->filePath);
		}
	if(page->fileName!=NULL)
		sinkReturn=asprintf(&this->savedPages[this->saveCnt].tabName,"%s",page->fileName);

	linetext=buf->getLineText();
	linetext[strlen(linetext)-1]=0;
	g_strchug(linetext);
	g_strchomp(linetext);
	this->savedPages[this->saveCnt].menuLabel=truncateWithElipses(linetext,maxFuncDefs);
	debugFree(&linetext);

	this->goForward();
	delete buf;
}

bool HistoryClass::canGoBack(void)
{
	return(this->backOK);
}

bool HistoryClass::canGoForward(void)
{
	return(this->forwardOK);
}
