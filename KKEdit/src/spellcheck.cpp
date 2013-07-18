/******************************************************
*
*     ©keithhedger Tue 16 Jul 20:46:19 BST 2013
*     kdhedger68713@gmail.com
*
*     spellcheck.cpp
* 
******************************************************/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

#include <aspell.h>

#include "config.h"
#include "globals.h"
#include "guis.h"

char	newWordBuffer[256];

void checkword(char* word)
{
	int							correct;
	AspellWordList*				suggestions;
	AspellStringEnumeration*	elements;
	const char*					suggestedword;
	int							wordcnt=0;
	char*						wordlist[100];

	correct=aspell_speller_check(spellChecker,word,-1);
	if(!correct)
		{
			buildWordCheck(word);
			suggestions=(AspellWordList*)aspell_speller_suggest(spellChecker,word,-1);
			elements=aspell_word_list_elements(suggestions);

			while((suggestedword=aspell_string_enumeration_next(elements))!=NULL)
				{
					wordlist[wordcnt]=strdup(suggestedword);
					gtk_combo_box_text_append_text((GtkComboBoxText*)wordListDropbox,wordlist[wordcnt]);
					wordcnt++;
				}
			delete_aspell_string_enumeration(elements);
			gtk_combo_box_set_active((GtkComboBox*)wordListDropbox,0);
			gtk_widget_show_all(spellCheckWord);
		}
}

void checkWord(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;
	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);
	checkword(selection);
	g_free(selection);
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}

void doChangeWord(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char*		newword;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}

	newword=gtk_combo_box_text_get_active_text((GtkComboBoxText*)wordListDropbox);
	gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&start,&end);
	gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,newword,-1);

	aspell_speller_store_replacement(spellChecker,selection,-1,newword,-1);

	gtk_widget_destroy(spellCheckWord);
}



