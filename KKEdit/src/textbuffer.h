/******************************************************
*
*     ©keithhedger Thu  5 Jun 14:36:43 BST 2014
*     kdhedger68713@gmail.com
*
*     textbuffer.h
* 
******************************************************/

#ifndef _TEXTBUFFERCLASS_
#define _TEXTBUFFERCLASS_

class TextBuffer
{
	public:
		TextBuffer();
		~TextBuffer();
		TextBuffer(GtkTextBuffer *buffer);

		GtkTextIter		lineStart;
		GtkTextIter		lineEnd;
		GtkTextIter		visibleLineEnd;
		GtkTextIter		cursorPos;
		GtkTextBuffer	*textBuffer;

		unsigned		lineNum;
		int				numChars;
		int				column;
		int				charPos;

		void getLineData(void);

		void getVisibleLine(void);
		void selectVisibleLine(void);
		void getLine(void);
		void selectLine(void);

		void getCursorIter(void);

		void getToLineEnd(void);
		void getToVisibleLineEnd(void);
		void deleteFromCursor(GtkTextIter *iter);

		void getToLineStart(void);
		void deleteToCursor(GtkTextIter *iter);

		void selectToLineStart(void);
		void selectToLineEnd(void);
		bool selectWord(void);
		char *getSelectedText(void);
		void selectRange(GtkTextIter *start,GtkTextIter *end);

		void scroll2Line(GtkTextView *view,int linenum,bool doupdate=false);
		void scroll2Mark(GtkTextView *view,GtkTextMark *mark);
		void scroll2Iter(GtkTextView *view,GtkTextIter iter);
		void scroll2OnScreen(GtkTextView *view,bool doupdate);
		void scroll2CentreScreen(GtkTextView *view,bool doupdate);
	private:
};

#endif