/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:50:41 GMT 2015 kdhedger68713@gmail.com

 * This file (completion.cpp) is part of KKEdit.

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

#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionitem.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "kkedit-includes.h"
////////////////TODO//////////////
struct _FunctionProviderClass
{
	GObjectClass			parent_class;
};

#ifdef _USEGTK3_
#define GTK_TYPE_SOURCE_COMPLETION_PROVIDER (gtk_source_completion_provider_get_type ())
#endif
G_DEFINE_TYPE_WITH_CODE(FunctionProvider,function_provider,G_TYPE_OBJECT,G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,function_provider_iface_init))

FunctionProvider*			funcProv;
FunctionProvider*			varsProv;
FunctionProvider*			customProv;

bool						forcePopup=false;
GtkSourceCompletionWords*	docWordsProv;

gchar* function_provider_get_name(GtkSourceCompletionProvider* provider)
{
	ERRDATA return g_strdup(((FunctionProvider *)provider)->name);
}

gint function_provider_get_priority(GtkSourceCompletionProvider* provider)
{
	ERRDATA return((FunctionProvider*)provider)->priority;
}

gboolean function_provider_match(GtkSourceCompletionProvider* provider,GtkSourceCompletionContext*context)
{
	ERRDATA return TRUE;
}

char* get_word_at_iter(GtkTextIter* iter,GtkTextBuffer *buffer)
{
	ERRDATA
	GtkTextIter*	startiter;
	char*			word;

	if(gtk_text_iter_ends_word(iter)==true)
		{
			startiter=gtk_text_iter_copy(iter);
			gtk_text_iter_backward_word_start(startiter);
			word=gtk_text_buffer_get_text(buffer,startiter,iter,true);
			if(strlen(word)>=autoShowMinChars)
				{
					ERRDATA return(word);
				}
		}
	ERRDATA return(NULL);
}

GdkPixbuf* function_provider_get_icon(GtkSourceCompletionProvider* provider)
{
	ERRDATA
	FunctionProvider* tp=(FunctionProvider*)provider;

	ERRDATA return tp->icon;
}

//completion
GList* addPropsFromWord(pageStruct* page,char* theword,FunctionProvider* prov)
{
	ERRDATA
	GList*	newlist=NULL;
//TODO//
#ifndef _USEGTK3_
	char*	infostr;
	GList*	customlist=customProv->proposals;
	char*	text;

	if(page->filePath==NULL)
		{
			ERRDATA return(newlist);
		}

	customlist=prov->proposals;
	while(customlist!=NULL)
		{
			text=gtk_source_completion_proposal_get_text((GtkSourceCompletionProposal*)customlist->data);
			infostr=gtk_source_completion_proposal_get_info((GtkSourceCompletionProposal*)customlist->data);
			if(text!=NULL)
				{
					if(strncasecmp(text,theword,strlen(theword))==0)
						newlist=g_list_append(newlist,gtk_source_completion_item_new(text,text,NULL,infostr));
					ERRDATA debugFree(&text);
				}
			customlist=customlist->next;
		}
#endif
	ERRDATA return(newlist);
}

void function_provider_populate(GtkSourceCompletionProvider* provider,GtkSourceCompletionContext* context)
{
//TODO//
#ifndef _USEGTK3_
	ERRDATA
	GtkTextIter 	iter;
	gchar*			word=NULL;
	GtkTextBuffer*	buffer;
	GList*			wordlist;
	pageStruct*		page;

	if(forcePopup==true)
		{
			gtk_source_completion_context_add_proposals(context,provider,((FunctionProvider *)provider)->proposals,true);
			ERRDATA return;
		}

	gtk_source_completion_context_get_iter(context,&iter);
	buffer=gtk_text_iter_get_buffer(&iter);

	word=get_word_at_iter(&iter,buffer);

	if(word==NULL || strlen(word)<autoShowMinChars)
		{
			g_free (word);
			gtk_source_completion_context_add_proposals(context,provider,NULL,true);
			ERRDATA return;
		}

	if(word!=NULL)
		{
			page=getPageStructPtr(-1);
			if(page!=NULL)
				{
					wordlist=addPropsFromWord(page,word,(FunctionProvider*)provider);
					gtk_source_completion_context_add_proposals(context,provider,wordlist,true);
					if(wordlist!=NULL)
						g_list_free_full(wordlist,g_object_unref);
				}
		}
	if(word!=NULL)
		ERRDATA debugFree(&word);
	ERRDATA
#endif
}

void function_provider_iface_init(GtkSourceCompletionProviderIface* iface)
{
//TODO//
#ifndef _USEGTK3_
	ERRDATA
	iface->get_name=function_provider_get_name;
	iface->populate=function_provider_populate;
	iface->match=function_provider_match;
	iface->get_priority=function_provider_get_priority;
	iface->get_icon=function_provider_get_icon;
	ERRDATA
#endif
}

void function_provider_class_init(FunctionProviderClass* klass)
{
//TODO//
#ifndef _USEGTK3_
	ERRDATA
#endif
}

void function_provider_init(FunctionProvider* self)
{
	ERRDATA
	self->icon=function_provider_get_icon(GTK_SOURCE_COMPLETION_PROVIDER(self));
	ERRDATA
}

//completion
void addProp(pageStruct* page)
{
//TODO//
#ifndef _USEGTK3_
	ERRDATA
	char*		functions=NULL;
	char		tmpstr[1024];
	char*		lineptr;
	char*		correctedstr;
	char		functype[63];
	char		infostr[1024];
	char*		holdstr;
	FILE*		fd=NULL;
	char		buffer[2048];
	char*		customfile;

	if(page->filePath==NULL)
		{
			ERRDATA return;
		}

//custom
	asprintf(&customfile,"%s/%s",getenv("HOME"),CUSTOMWORDFILE);
	fd=fopen(customfile,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>2)
						{
							buffer[strlen(buffer)-1]=0;
							customProv->proposals=g_list_append(customProv->proposals,gtk_source_completion_item_new(buffer,buffer,NULL,NULL));
						}
				}
			fclose(fd);
		}

//function and variable compitions
	getRecursiveTagList(page->filePath,&functions);
	lineptr=functions;

	while (lineptr!=NULL)
		{
			tmpstr[0]=0;
			sscanf (lineptr,"%s",tmpstr);
			sscanf (lineptr,"%*s %*s %*i %[^\n]s",infostr);
			holdstr=strdup(tmpstr);
			correctedstr=truncateWithElipses(tmpstr,maxFuncDefs);
			sprintf(tmpstr,"%s",correctedstr);
			if(strlen(tmpstr)>0)
				{
					sscanf (lineptr,"%*s %s",functype);	
					if(strcasecmp(functype,"function")==0)
						funcProv->proposals=g_list_append(funcProv->proposals,gtk_source_completion_item_new(tmpstr,holdstr,NULL,infostr));
					if(strcasecmp(functype,"variable")==0)
						varsProv->proposals=g_list_append(varsProv->proposals,gtk_source_completion_item_new(tmpstr,holdstr,NULL,infostr));
				}
			ERRDATA debugFree(&correctedstr);
			ERRDATA debugFree(&holdstr);

			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;
		}
	if(functions!=NULL)
		ERRDATA debugFree(&functions);
	ERRDATA
#endif
}

void removeProps(void)
{
//TODO//
#ifndef _USEGTK3_
	ERRDATA
	g_list_free_full(funcProv->proposals,g_object_unref);
	g_list_free_full(varsProv->proposals,g_object_unref);
	g_list_free_full(customProv->proposals,g_object_unref);
	funcProv->proposals=NULL;
	varsProv->proposals=NULL;
	customProv->proposals=NULL;
	ERRDATA
#endif
}

void createCompletion(pageStruct* page)
{
//TODO//
#ifndef _USEGTK3_
	ERRDATA
	removeProps();

	gtk_source_completion_words_register(docWordsProv,gtk_text_view_get_buffer(GTK_TEXT_VIEW(page->view)));
	g_object_set(docWordsProv,"minimum-word-size",autoShowMinChars,NULL);

	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(docWordsProv),NULL);
	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(funcProv),NULL);
	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(varsProv),NULL);
	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(customProv),NULL);

	addProp(page);
	ERRDATA
#endif
}

void doCompletionPopUp(pageStruct* page)
{
//TODO//
#ifndef _USEGTK3_
	ERRDATA
	GtkSourceCompletionContext*	context;
	GList*						list;

	context=gtk_source_completion_create_context(page->completion,NULL);
	list=gtk_source_completion_get_providers(page->completion);
	gtk_source_completion_show(page->completion,list,context);
	ERRDATA
#endif
}
