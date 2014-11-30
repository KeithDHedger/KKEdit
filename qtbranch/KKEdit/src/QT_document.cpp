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

void DocumentClass::setPathname(char* pathname)
{
	this->page->filePath=pathname;
}

const char* DocumentClass::getPathname(void)
{
	return((const char*)this->page->filePath);
}

void DocumentClass::setDirname(char* dirname)
{
	this->page->dirName=dirname;
}

const char* DocumentClass::getDirname(void)
{
	return((const char*)this->page->dirName);
}

void DocumentClass::setRealpath(char* realpath)
{
	this->page->realFilePath=realpath;
}

const char* DocumentClass::getRealpath(void)
{
	return((const char*)this->page->realFilePath);
}

void DocumentClass::setTabname(char* tabname)
{
	this->page->tabName=tabname;
}

const char* DocumentClass::getTabname(void)
{
	return((const char*)this->page->tabName);
}


pageStruct* DocumentClass::getPage(void)
{
	return(this->page);
}

