/*
 *
 * ©K. D. Hedger. Fri  6 May 10:50:15 BST 2016 kdhedger68713@gmail.com

 * This file (textbuffer.h) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

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

		int				selectionStart;
		int				selectionEnd;

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
		void scroll2OnScreen(GtkTextView *view);
		void scroll2CentreScreen(GtkTextView *view,bool doupdate);

		char *getLineText(void);
		void getSelectionBounds(void);

	private:
};

#endif