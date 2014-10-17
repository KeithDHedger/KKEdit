/******************************************************
*
*     ©keithhedger Thu  5 Jun 14:36:43 BST 2014
*     kdhedger68713@gmail.com
*
*     textbuffer.cpp
* 
******************************************************/

#include "globals.h"

TextBuffer::TextBuffer()
{
	this->textBuffer=NULL;
}

TextBuffer::~TextBuffer()
{
}

TextBuffer::TextBuffer(GtkTextBuffer* buffer)
{
	this->textBuffer=buffer;
	this->getLineData();
	this->getVisibleLine();
	this->getLine();
}

void TextBuffer::getLineData()
{
	gtk_text_buffer_get_iter_at_mark(textBuffer,&lineStart,gtk_text_buffer_get_insert(textBuffer));
	gtk_text_buffer_get_iter_at_mark(textBuffer,&lineEnd,gtk_text_buffer_get_insert(textBuffer));
	gtk_text_iter_forward_visible_line(&lineEnd);
	gtk_text_iter_set_line_offset(&lineStart,0);
	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,gtk_text_buffer_get_insert(textBuffer));
	lineNum=gtk_text_iter_get_line(&cursorPos)+1;
	column=gtk_text_iter_get_line_offset(&cursorPos)+1;
}

void TextBuffer::getCursorIter()
{
	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,gtk_text_buffer_get_insert(textBuffer));
}

void TextBuffer::getToLineEnd()
{
	this->getCursorIter();
	lineEnd=cursorPos;
	gtk_text_iter_forward_to_line_end(&lineEnd);
}

void TextBuffer::getToVisibleLineEnd()
{
	this->getCursorIter();
	visibleLineEnd=cursorPos;
	gtk_text_iter_forward_to_line_end(&visibleLineEnd);
}

void TextBuffer::getToLineStart()
{
	this->getCursorIter();
	lineStart=cursorPos;
	gtk_text_iter_set_line_offset(&lineStart,0);
}

void TextBuffer::deleteFromCursor(GtkTextIter* iter)
{
	this->getCursorIter();
	if(!gtk_text_iter_ends_line(&cursorPos))
		gtk_text_buffer_delete(textBuffer,&cursorPos,iter);
}

void TextBuffer::deleteToCursor(GtkTextIter* iter)
{
	this->getCursorIter();
	if(!gtk_text_iter_starts_line(&cursorPos))
		this->deleteFromCursor(iter);
}

bool TextBuffer::selectWord()
{
	this->getCursorIter();
	lineStart=cursorPos;
	if(gtk_text_iter_inside_word(&cursorPos))
		{
			gtk_text_iter_backward_word_start(&lineStart);
			gtk_text_iter_forward_word_end(&cursorPos);
			return(true);
		}
	return(false);
}

char* TextBuffer::getSelectedText()
{
	return(gtk_text_buffer_get_text(textBuffer,&lineStart,&lineEnd,true));
}

void TextBuffer::selectRange(GtkTextIter* start,GtkTextIter* end)
{
	gtk_text_buffer_select_range(textBuffer,start,end);
}


void TextBuffer::getVisibleLine()
{
	this->getCursorIter();
	lineStart=cursorPos;
	visibleLineEnd=cursorPos;
	gtk_text_iter_set_line_offset(&lineStart,0);
	gtk_text_iter_forward_to_line_end(&visibleLineEnd);
}

void TextBuffer::selectVisibleLine()
{
	gtk_text_buffer_select_range(textBuffer,&lineStart,&visibleLineEnd);
}

void TextBuffer::getLine()
{
	this->getCursorIter();
	lineStart=cursorPos;
	gtk_text_iter_set_line_offset(&lineStart,0);
	lineEnd=cursorPos;
	gtk_text_iter_forward_visible_line(&lineEnd);
}

void TextBuffer::selectLine()
{
	gtk_text_buffer_select_range(textBuffer,&lineStart,&lineEnd);
}

void TextBuffer::selectToLineStart()
{
	this->getToLineStart();
	gtk_text_buffer_select_range(textBuffer,&lineStart,&cursorPos);
}

void TextBuffer::selectToLineEnd()
{
	this->getToVisibleLineEnd();
	gtk_text_buffer_select_range(textBuffer,&cursorPos,&visibleLineEnd);
}

void TextBuffer::scroll2Line(GtkTextView* view,int linenum)
{
	GtkTextMark*	mark;
	GtkTextIter		iter;

	gtk_text_buffer_get_iter_at_line_offset(textBuffer,&iter,linenum,0);
	gtk_text_buffer_place_cursor(textBuffer,&iter);
	if(!gtk_text_view_scroll_to_iter(view,&iter,0,true,0,0.5))
		{
			mark=gtk_text_buffer_get_mark(textBuffer,"insert");
			this->getCursorIter();
			if(!gtk_text_view_scroll_to_iter(view,&cursorPos,0,true,0,0.5))
				gtk_text_view_scroll_to_mark(view,mark,0,true,0,0.5);
		}
}

void TextBuffer::scroll2LineM(pageStruct* page,int linenum)
{
	GtkTextIter		iter;

	gtk_text_buffer_get_iter_at_line_offset(textBuffer,&iter,linenum,0);
	gtk_text_buffer_place_cursor(textBuffer,&iter);
	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);

	for(int j=0;j<10;j++)
		{
			gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
			gtk_main_iteration_do(false);
		}
}


void TextBuffer::scroll2Mark(GtkTextView* view,GtkTextMark* mark)
{
	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,mark);
	gtk_text_view_scroll_to_iter(view,&cursorPos,0,true,0,0.5);
	gtk_text_view_scroll_to_mark(view,mark,0,true,0,0.5);
}

void TextBuffer::scroll2Iter(GtkTextView* view,GtkTextIter iter)
{
	gtk_text_view_scroll_to_iter(view,&iter,0,true,0,0.5);
}


