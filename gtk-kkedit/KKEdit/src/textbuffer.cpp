/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:53:40 GMT 2015 kdhedger68713@gmail.com

 * This file (textbuffer.cpp) is part of KKEdit.

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

#include "globals.h"

TextBuffer::TextBuffer()
{
	this->textBuffer=NULL;
	ERRDATA
}

TextBuffer::~TextBuffer()
{
	ERRDATA
}

TextBuffer::TextBuffer(GtkTextBuffer* buffer)
{
	ERRDATA
	this->textBuffer=buffer;
	this->getLineData();
	this->getVisibleLine();
	this->getLine();
	ERRDATA
}

void TextBuffer::getLineData()
{
	ERRDATA
	gtk_text_buffer_get_iter_at_mark(textBuffer,&lineStart,gtk_text_buffer_get_insert(textBuffer));
	gtk_text_buffer_get_iter_at_mark(textBuffer,&lineEnd,gtk_text_buffer_get_insert(textBuffer));
	gtk_text_iter_forward_visible_line(&lineEnd);
	gtk_text_iter_set_line_offset(&lineStart,0);
	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,gtk_text_buffer_get_insert(textBuffer));
	lineNum=gtk_text_iter_get_line(&cursorPos)+1;
	column=gtk_text_iter_get_line_offset(&cursorPos)+1;
	charPos=gtk_text_iter_get_offset(&cursorPos);
	ERRDATA
}

void TextBuffer::getCursorIter()
{
	ERRDATA gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,gtk_text_buffer_get_insert(textBuffer));
}

void TextBuffer::getToLineEnd()
{
	ERRDATA
	this->getCursorIter();
	lineEnd=cursorPos;
	gtk_text_iter_forward_to_line_end(&lineEnd);
	ERRDATA
}

void TextBuffer::getToVisibleLineEnd()
{
	ERRDATA
	this->getCursorIter();
	visibleLineEnd=cursorPos;
	gtk_text_iter_forward_to_line_end(&visibleLineEnd);
	ERRDATA
}

void TextBuffer::getToLineStart()
{
	ERRDATA
	this->getCursorIter();
	lineStart=cursorPos;
	gtk_text_iter_set_line_offset(&lineStart,0);
	ERRDATA
}

void TextBuffer::deleteFromCursor(GtkTextIter* iter)
{
	ERRDATA
	this->getCursorIter();
	if(!gtk_text_iter_ends_line(&cursorPos))
		gtk_text_buffer_delete(textBuffer,&cursorPos,iter);
	ERRDATA
}

void TextBuffer::deleteToCursor(GtkTextIter* iter)
{
	ERRDATA
	this->getCursorIter();
	if(!gtk_text_iter_starts_line(&cursorPos))
		this->deleteFromCursor(iter);
	ERRDATA
}

bool TextBuffer::selectWord()
{
	ERRDATA
	this->getCursorIter();
	lineStart=cursorPos;
	if(gtk_text_iter_inside_word(&cursorPos))
		{
			gtk_text_iter_backward_word_start(&lineStart);
			gtk_text_iter_forward_word_end(&cursorPos);
			ERRDATA return(true);
		}
	ERRDATA return(false);
}

char* TextBuffer::getSelectedText()
{
	ERRDATA return(gtk_text_buffer_get_text(textBuffer,&lineStart,&lineEnd,true));
}

void TextBuffer::selectRange(GtkTextIter* start,GtkTextIter* end)
{
	ERRDATA gtk_text_buffer_select_range(textBuffer,start,end);
}


void TextBuffer::getVisibleLine()
{
	ERRDATA
	this->getCursorIter();
	lineStart=cursorPos;
	visibleLineEnd=cursorPos;
	gtk_text_iter_set_line_offset(&lineStart,0);
	gtk_text_iter_forward_to_line_end(&visibleLineEnd);
	ERRDATA
}

void TextBuffer::selectVisibleLine()
{
	ERRDATA gtk_text_buffer_select_range(textBuffer,&lineStart,&visibleLineEnd);
}

void TextBuffer::getLine()
{
	ERRDATA
	this->getCursorIter();
	lineStart=cursorPos;
	gtk_text_iter_set_line_offset(&lineStart,0);
	lineEnd=cursorPos;
	gtk_text_iter_forward_visible_line(&lineEnd);
	ERRDATA
}

void TextBuffer::selectLine()
{
	ERRDATA gtk_text_buffer_select_range(textBuffer,&lineStart,&lineEnd);
}

void TextBuffer::selectToLineStart()
{
	ERRDATA
	this->getToLineStart();
	gtk_text_buffer_select_range(textBuffer,&lineStart,&cursorPos);
	ERRDATA
}

void TextBuffer::selectToLineEnd()
{
	ERRDATA
	this->getToVisibleLineEnd();
	gtk_text_buffer_select_range(textBuffer,&cursorPos,&visibleLineEnd);
	ERRDATA
}

void TextBuffer::scroll2Line(GtkTextView* view,int linenum,bool doupdate)
{
	ERRDATA
	GtkTextMark*	mark;
	GtkTextIter		iter;

	gtk_text_buffer_get_iter_at_line_offset(textBuffer,&iter,linenum,0);
	gtk_text_buffer_place_cursor(textBuffer,&iter);
	mark=gtk_text_buffer_get_insert(textBuffer);
	gtk_text_view_scroll_to_mark(view,mark,0,true,0,0.5);
	if(doupdate==true)
		{
			while(gtk_events_pending())
				gtk_main_iteration();
		}
	ERRDATA
}

void TextBuffer::scroll2Mark(GtkTextView* view,GtkTextMark* mark)
{
	ERRDATA
	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,mark);
	gtk_text_view_scroll_to_iter(view,&cursorPos,0,true,0,0.5);
	gtk_text_view_scroll_to_mark(view,mark,0,true,0,0.5);
	ERRDATA
}

void TextBuffer::scroll2Iter(GtkTextView* view,GtkTextIter iter)
{
	ERRDATA gtk_text_view_scroll_to_iter(view,&iter,0,true,0,0.5);
}


