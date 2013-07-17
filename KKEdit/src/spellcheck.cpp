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

void checkword(char* word)
{
	AspellConfig*				spell_config;
	AspellCanHaveError*			possible_err;
	int							correct;
	AspellSpeller*				spell_checker=0;
	AspellWordList*				suggestions;
	AspellStringEnumeration*	elements;
	const char*					suggestedword;
	int							wordcnt=0;
	char*						wordlist[100];

	spell_config=new_aspell_config();
	aspell_config_replace(spell_config,"lang","en_GB");

	possible_err=new_aspell_speller(spell_config);
	if(aspell_error_number(possible_err)!= 0)
		puts(aspell_error_message(possible_err));
	else
		spell_checker=to_aspell_speller(possible_err);

	correct=aspell_speller_check(spell_checker,word,-1);
	if(correct)
		printf("correct\n");
	else
		{
			printf(":( %i\n",correct);
			buildWordCheck();
			suggestions=(AspellWordList*)aspell_speller_suggest(spell_checker,word,-1);
			elements=aspell_word_list_elements(suggestions);

			while((suggestedword=aspell_string_enumeration_next(elements))!=NULL)
				{
					printf("  %s\n", suggestedword);
					wordlist[wordcnt]=strdup(suggestedword);
					gtk_combo_box_text_append_text((GtkComboBoxText*)wordListDropbox,wordlist[wordcnt]);
					wordcnt++;
				}
			delete_aspell_string_enumeration(elements);
		}
	gtk_widget_show_all(spellCheckWord);

	delete_aspell_config(spell_config);
	delete_aspell_speller(spell_checker);
}

void checkWord(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char*		command;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	printf("word=%s\n",selection);

	checkword(selection);
	

	asprintf(&command,"echo -n \"%s\"|ispell -a|grep \":\"",selection);
	system(command);
	g_free(selection);

}