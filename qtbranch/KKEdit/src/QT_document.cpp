/******************************************************
*
*     ©keithhedger Sat 29 Nov 14:19:35 GMT 2014
*     kdhedger68713@gmail.com
*
*     QT_document.cpp
* 
******************************************************/

#include "QT_document.h"

DocumentClass::~DocumentClass()
{
}

DocumentClass::DocumentClass(QWidget *parent): QTextEdit(parent)
{
	this->page=new pageStruct;
	this->highlighter=new Highlighter(this->document());
}

void DocumentClass::setFilename(char* filename)
{
	this->page->fileName=filename;
}

const char* DocumentClass::getFilename(void)
{
	return((const char*)this->page->fileName);
}

pageStruct* DocumentClass::getPage(void)
{
	return(this->page);
}

