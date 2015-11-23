/******************************************************
*
*     ©keithhedger Sat 29 Nov 14:19:35 GMT 2014
*     kdhedger68713@gmail.com
*
*     QT_document.h
* 
******************************************************/
#include "kkedit-includes.h"
#include "QT_highlighter.h"

#ifndef _QT_DOCUMENT_
#define _QT_DOCUMENT_

#include <QPlainTextEdit>
#include <QObject>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

class DocumentClass : public QPlainTextEdit
{
	Q_OBJECT

	public:
		explicit DocumentClass(QWidget *parent = 0);
		~DocumentClass();

		void								setFilename(char* filename);
		const char							*getFilename(void);
		void								setPathname(char* filename);
		const char							*getPathname(void);
		void								setDirname(char* filename);
		const char							*getDirname(void);
		void								setRealpath(char* filename);
		const char							*getRealpath(void);
		void								setTabname(char* filename);
		const char							*getTabname(void);
		pageStruct							*getPage(void);

		void								lineNumberAreaPaintEvent(QPaintEvent *event);
		int									lineNumberAreaWidth(void);

		QList<QTextEdit::ExtraSelection>	extraSelections;
		QList<QTextEdit::ExtraSelection>	hilightSelections;
		
		QTextEdit::ExtraSelection			selection;
		QTextEdit::ExtraSelection			selectedLine;

		void								setXtraSelections(void);
		void								addXtraSelections(void);
		void								clearHilites(void);

	protected:
	    void								resizeEvent(QResizeEvent *event);
		void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;

	private slots:
		void								updateLineNumberAreaWidth(int newBlockCount);
		void								highlightCurrentLine();
		void								updateLineNumberArea(const QRect &, int);

	private:
		void								clearXtraSelections(void);
		QWidget 							*lineNumberArea;
		pageStruct							*page;
		Highlighter							*highlighter;
		QTextCursor							oldCursor;
		QTextBlockFormat					oldBlockFormat;

};

class LineNumberArea : public QWidget
{
	Q_OBJECT

	public:
	    LineNumberArea(DocumentClass *editor) : QWidget(editor)
			{
				codeEditor=editor;
			}

		QSize sizeHint() const
			{
				return QSize(codeEditor->lineNumberAreaWidth(),0);
			}

	protected:
		void paintEvent(QPaintEvent *event)
			{
				codeEditor->lineNumberAreaPaintEvent(event);
			}
 
	private:
		DocumentClass *codeEditor;
};

#endif
