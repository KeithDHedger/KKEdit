/******************************************************
*
*     keithhedger Tue 16 Jul 20:46:19 BST 2013
*     kdhedger68713@gmail.com
*
*     spellcheck.cpp
*
*
*	  Parts of this code from aspell-0.60.6.1 example
*	  example-c.c
*
*     Copyright (C) 2000-2001 by Kevin Atkinson under the GNU LGPL
*	  license version 2.0 or 2.1.  You should have received a copy of the
*	  LGPL license along with this library if you did not you can find it
*	  at http://www.gnu.org/. 
*
*
******************************************************/

//#include <stdlib.h>
//#include <gtk/gtk.h>
//#include <string.h>
//#include <sys/stat.h>
//#include <gtksourceview/gtksourcebuffer.h>
//#include <gtksourceview/gtksourceview.h>

//#ifdef _ASPELL_
//#include <aspell.h>
//#endif

//#include "config.h"
#include "globals.h"
#include "guis.h"

#ifdef _ASPELL_

bool	cancelCheck=false;

void doCancelCheck(GtkWidget* widget,gpointer data)
{
	gtk_widget_destroy(spellCheckWord);
	if(badWord!=NULL)
		g_free(badWord);
	cancelCheck=true;
}

void checkTheWord(char* word,int checkDoc)
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
			badWord=word;
			cancelCheck=false;
			buildWordCheck(checkDoc);
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
			gtk_dialog_run((GtkDialog *)spellCheckWord);
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
	checkTheWord(selection,false);
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}

void doChangeWord(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;

	if((long)data==0)
		{
			if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
				{
					badWord=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
					if(badWord==NULL)
						return;
				}

			goodWord=gtk_combo_box_text_get_active_text((GtkComboBoxText*)wordListDropbox);
			gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&start,&end);
			gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,goodWord,-1);
			g_free(goodWord);
		}
	else
		goodWord=gtk_combo_box_text_get_active_text((GtkComboBoxText*)wordListDropbox);

	aspell_speller_store_replacement(spellChecker,badWord,-1,goodWord,-1);

	gtk_widget_destroy(spellCheckWord);
	if(badWord!=NULL)
		g_free(badWord);
}

void doAddIgnoreWord(GtkWidget* widget,gpointer data)
{
	if((long)data==1)
		aspell_speller_add_to_session(spellChecker,badWord,-1);
	else
		{
			aspell_speller_add_to_personal(spellChecker,badWord,-1);
			aspell_speller_save_all_word_lists(spellChecker);
		}

	gtk_widget_destroy(spellCheckWord);
	if(badWord!=NULL)
		g_free(badWord);
}

void doSpellCheckDoc(GtkWidget* widget,gpointer data)
{
	pageStruct*				page=getPageStructPtr(-1);
	gchar*					buffer;
	long					filelen;
	GtkTextIter				start;
	GtkTextIter				end;

	AspellCanHaveError*		ret;
	AspellDocumentChecker*	checker;
	AspellToken				token;
	FILE*					out;
	FILE*					doc;
	char					line[2048];
	char*					tempfile=tempnam(NULL,"KKEd");
	int						diff;
	unsigned int			goodwordlen;
	char*					word_begin;
	char*					filename=(char*)data;
	char*					badword;

	/* Open the file */
	doc=fopen(filename,"r");
	if (doc<=0)
		{
	  		printf("Error: Unable to open the file \"%s\" for reading.",filename);
	  		return;
		}

	/* Open tempfile for writing the results */
	out=fopen(tempfile,"w");
	if(out<=0)
		{
			printf("Error: Unable to open the file \"%s\" for writing.",tempfile);
			fclose(doc);
			return;
		}

	/* Set up the document checker */
	ret=new_aspell_document_checker(spellChecker);
	if (aspell_error(ret)!=0)
		{
			printf("Error: %s\n",aspell_error_message(ret));
			fclose(out);
			fclose(doc);
			return;
		}

	checker=to_aspell_document_checker(ret);
	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);
	while(fgets(line,2048,doc)) 
		{
	  /* First process the line */
			aspell_document_checker_process(checker,line,-1);
	  		diff=0;

	  /* Now find the misspellings in the line */
	 		 while(token=aspell_document_checker_next_misspelling(checker),token.len!=0)
				{
	    /* Pay particular attention to how token.offset and diff is used */
					asprintf(&badword,"%.*s",token.len,(char*)&line[token.offset+diff]);
					goodWord=NULL;
					checkTheWord(badword,1);
					if(cancelCheck==true)
						{
							delete_aspell_document_checker(checker);
							fclose(out);
							fclose(doc);
							remove(tempfile);
							g_free(tempfile);
							gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
							return;
						}
					word_begin=line+token.offset+diff;

					if(goodWord!=NULL)
						{
							goodwordlen=strlen(goodWord);
	    /* Replace the misspelled word with the replacement */
							diff+=goodwordlen-token.len;
							memmove(word_begin+goodwordlen,word_begin+token.len,strlen(word_begin+token.len)+1);
							memcpy(word_begin,goodWord,goodwordlen);
						}
				}

	  /* print the line to filename.checked */
			fputs(line,out);
		}

	delete_aspell_document_checker(checker);
	fclose(out);
	fclose(doc);

	g_file_get_contents(tempfile,&buffer,(gsize*)&filelen,NULL);
	gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
	gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
	gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
	gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
	gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,buffer,filelen);

	remove(tempfile);
	g_free(buffer);
	g_free(tempfile);

	if(page->filePath!=NULL)
		{
			page->itsMe=true;
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end);
			buffer=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer, &start, &end, FALSE);

			out=fopen(page->filePath,"w");
			if (out!=NULL)
				{
					fputs(buffer,out);
					fclose(out);
					gtk_text_buffer_set_modified((GtkTextBuffer*)page->buffer,true);
				}
		}

	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
}
#endif

