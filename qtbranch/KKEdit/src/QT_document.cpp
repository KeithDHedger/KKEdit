/******************************************************
*
*     ©keithhedger Sat 29 Nov 14:19:35 GMT 2014
*     kdhedger68713@gmail.com
*
*     QT_document.cpp
*
******************************************************/

#include "QT_document.h"

void DocumentClass::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(this->lineNumberArea);
	painter.fillRect(event->rect(),Qt::lightGray);

	QTextBlock block=this->firstVisibleBlock();
	int blockNumber=block.blockNumber();
	int top=(int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom=top+(int) blockBoundingRect(block).height();

	while(block.isValid() && top<=event->rect().bottom())
		{
			if(block.isVisible() && bottom>=event->rect().top())
				{
					QString number=QString::number(blockNumber+1);
					painter.setPen(Qt::black);
					painter.drawText(0,top,lineNumberArea->width(),fontMetrics().height(),Qt::AlignRight,number);
				}

			block=block.next();
			top=bottom;
			bottom=top+(int) blockBoundingRect(block).height();
			blockNumber++;
		}
}

int DocumentClass::lineNumberAreaWidth(void)
{
	int digits=1;
	int max=qMax(1,blockCount());
	while (max>=10)
		{
			max/=10;
			digits++;
		}
	int space=3+fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

void DocumentClass::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr=contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(),cr.top(),lineNumberAreaWidth(),cr.height()));
}

DocumentClass::~DocumentClass()
{
	debugFree(&(this->page->fileName),"~DocumentClass this->page->fileName");
	debugFree(&(this->page->filePath),"~DocumentClass this->page->filePath");
	debugFree(&(this->page->dirName),"~DocumentClass this->page->dirName");
	debugFree(&(this->page->realFilePath),"~DocumentClass this->page->realFilePath");
	debugFree(&(this->page->tabName),"~DocumentClass this->page->tabName");
	debugFree((char**)&(this->page),"~DocumentClass page");
	
}

void DocumentClass::updateLineNumberAreaWidth(int)
{
	setViewportMargins(lineNumberAreaWidth(),0,0,0);
}

void DocumentClass::clearAllBlocks(void)
{
	QList<QTextEdit::ExtraSelection>	extraSelections;
	QTextEdit::ExtraSelection			selection;
	
//	this->setExtraSelections(extraSelections);
//	QTextBlockFormat	block;
//	QTextCursor			cursor;
//return;
//	cursor=this->textCursor();
//	cursor.select(QTextCursor::Document);
//	block=cursor.blockFormat();
//	block.clearBackground();
//	cursor.setBlockFormat(block);
#if 0
	QTextBlockFormat	block;
	QTextCharFormat		format;
	QTextCursor			cursor;
	this->textCursor().beginEditBlock();
	if (!isReadOnly())
		{
			cursor=this->textCursor();
			cursor.select(QTextCursor::Document);
			block=cursor.blockFormat();
			block.clearBackground();
			cursor.setBlockFormat(block);
		}
	this->textCursor().endEditBlock();
#endif
}

void DocumentClass::highlightCurrentLine()
{
	QColor lineColor = QColor(Qt::yellow).lighter(160);

	this->selectedLine.format.setBackground(lineColor);
	this->selectedLine.format.setProperty(QTextFormat::FullWidthSelection, true);
	this->selectedLine.cursor=textCursor();

	this->selectedLine.cursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
	this->selectedLine.cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor);

	this->setXtraSelections();	
return;
this->currentLineSelection.clear();
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);
	
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();

		selection.cursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
		selection.cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor);
        this->currentLineSelection.append(selection);
    }
this->setXtraSelections();
 //   setExtraSelections(extraSelections);



//	QColor lineColor=QColor(Qt::yellow).lighter(160);
//	QTextEdit::ExtraSelection			selection;
//
//	this->currentLineSelection.clear();
//	this->selection.format.setBackground(lineColor);

//	this->currentLineSelection.format.setBackground(currentLineHighlight);
//	this->currentLineSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
//
//	this->currentLineSelection.cursor = textCursor();
//	this->currentLineSelection.cursor.clearSelection();

//es.append(selection);
//setExtraSelections(es);
return;
//	QTextBlockFormat	block;
//	QColor lineColor=QColor(Qt::yellow).lighter(160);
//
//	block=this->textCursor().blockFormat();
//	block.setBackground(lineColor);
//	this->textCursor().setBlockFormat(block);
#if 0
	QTextBlockFormat	block;

	this->textCursor().beginEditBlock();

	if (!isReadOnly())
		{
			QColor lineColor=QColor(Qt::yellow).lighter(160);

			block=this->textCursor().blockFormat();
			block.setBackground(lineColor);
			this->textCursor().setBlockFormat(block);
		}
	
	this->textCursor().endEditBlock();
#endif
}
void DocumentClass::setXtraSelections()
{
	this->extraSelections.clear();
	this->extraSelections=this->extraSelections+this->hilightSelections;
	this->extraSelections.append(this->selectedLine);
	this->setExtraSelections(this->extraSelections);
}

void DocumentClass::addXtraSelections()
{
}

void DocumentClass::clearXtraSelections()
{
	this->extraSelections.clear();
	this->setXtraSelections();
}


void DocumentClass::clearHilites()
{
	this->hilightSelections.clear();
	this->setXtraSelections();
}

void DocumentClass::updateLineNumberArea(const QRect &rect,int dy)
{
return;
	if(dy)
		lineNumberArea->scroll(0,dy);
	else
		lineNumberArea->update(0,rect.y(),lineNumberArea->width(),rect.height());

	if(rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

DocumentClass::DocumentClass(QWidget *parent): QPlainTextEdit(parent)
{
	this->page=new pageStruct;
	this->page->fileName=NULL;
	this->page->filePath=NULL;
	this->page->dirName=NULL;
	this->page->realFilePath=NULL;
	this->page->tabName=NULL;
	this->page->regexList=NULL;
	this->page->doneHighlightAll=false;
	this->page->lastFind=NULL;
	this->page->reserved4=0;

	this->highlighter=new Highlighter(this->document());
	this->setCenterOnScroll(true);
	lineNumberArea=new LineNumberArea(this);

	connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(updateLineNumberAreaWidth(int)));
	connect(this,SIGNAL(updateRequest(QRect,int)),this,SLOT(updateLineNumberArea(QRect,int)));
	connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(clearAllBlocks()));
	connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
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
	if(this->page->dirName!=NULL)
		debugFree(&(this->page->dirName),"DocumentClass::setDirname dirName");
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


