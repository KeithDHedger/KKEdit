/******************************************************
*
*     ©keithhedger Sat 29 Nov 14:19:35 GMT 2014
*     kdhedger68713@gmail.com
*
*     QT_document.h
* 
******************************************************/

#ifndef _QT_DOCUMENT_
#define _QT_DOCUMENT_

#include "kkedit-includes.h"
#include "QT_highlighter.h"

class DocumentClass : public QTextEdit
{
	Q_OBJECT

	public:
		explicit DocumentClass(QWidget *parent = 0);
		~DocumentClass();

		void		setFilename(char* filename);
		const char* getFilename(void);
		pageStruct*	getPage(void);

	private:
		pageStruct* page;
		Highlighter *highlighter;
};

#endif
