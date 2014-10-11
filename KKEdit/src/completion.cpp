/******************************************************
*
*     ©keithhedger Sat 11 Oct 11:51:45 BST 2014
*     kdhedger68713@gmail.com
*
*     completion.cpp
* 
******************************************************/
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionitem.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "kkedit-includes.h"

typedef struct _TestProvider TestProvider;
typedef struct _TestProviderClass TestProviderClass;

struct _TestProvider
{
	GObject parent;

	GList *proposals;
	gint priority;
	gchar *name;
GtkSourceCompletionContext  *context;
	GdkPixbuf *icon;
};

struct _TestProviderClass
{
	GObjectClass parent_class;
};

static void test_provider_iface_init (GtkSourceCompletionProviderIface *iface);
GType test_provider_get_type (void);

G_DEFINE_TYPE_WITH_CODE (TestProvider,
			 test_provider,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
				 		test_provider_iface_init))

static gchar *
test_provider_get_name (GtkSourceCompletionProvider *provider)
{
	return g_strdup (((TestProvider *)provider)->name);
}

static gint
test_provider_get_priority (GtkSourceCompletionProvider *provider)
{
	return ((TestProvider *)provider)->priority;
}

static gboolean
test_provider_match (GtkSourceCompletionProvider *provider,
                     GtkSourceCompletionContext  *context)
{
	return TRUE;
}

static void
test_provider_populate (GtkSourceCompletionProvider *provider,
                        GtkSourceCompletionContext  *context)
{
	gtk_source_completion_context_add_proposals (context,
	                                             provider,
	                                             ((TestProvider *)provider)->proposals,
	                                             TRUE);
((TestProvider *)provider)->context=context;
}

static GdkPixbuf *
test_provider_get_icon (GtkSourceCompletionProvider *provider)
{
	TestProvider *tp = (TestProvider *)provider;

	if (tp->icon == NULL)
	{
		GtkIconTheme *theme = gtk_icon_theme_get_default ();
		tp->icon = gtk_icon_theme_load_icon (theme, GTK_STOCK_DIALOG_INFO, 16, (GtkIconLookupFlags)0, NULL);
	}

	return tp->icon;
}

static void
test_provider_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = test_provider_get_name;

	iface->populate = test_provider_populate;
	iface->match = test_provider_match;
	iface->get_priority = test_provider_get_priority;

	//iface->get_icon = test_provider_get_icon;
}

static void
test_provider_class_init (TestProviderClass *klass)
{
}

void test_provider_init (TestProvider *self)
{
	GList *proposals = NULL;
	GdkPixbuf *icon = test_provider_get_icon(GTK_SOURCE_COMPLETION_PROVIDER(self));

	proposals = g_list_prepend (proposals,
	                            gtk_source_completion_item_new ("Proposal 3", "Proposal 3", icon, NULL));

	proposals = g_list_prepend (proposals,
	                            gtk_source_completion_item_new ("Proposal 2", "Proposal 2", icon, NULL));

	proposals = g_list_prepend (proposals,
	                            gtk_source_completion_item_new ("Proposal 1", "Proposal 1", icon, NULL));

	self->proposals = proposals;
}

void createCompletion(pageStruct* page)
{
	GtkSourceCompletionWords *prov_words;
	GtkSourceCompletion *completion;

	completion=gtk_source_view_get_completion(page->view);
	prov_words=gtk_source_completion_words_new(NULL,NULL);
	
	gtk_source_completion_words_register(prov_words,gtk_text_view_get_buffer(GTK_TEXT_VIEW(page->view)));
//	
	gtk_source_completion_add_provider(completion,GTK_SOURCE_COMPLETION_PROVIDER(prov_words),NULL);
//
	g_object_set(prov_words, "priority",10, NULL);

	TestProvider* tp=(TestProvider*)g_object_new(test_provider_get_type(),NULL);
	tp->priority=1;
	tp->name="Test Provider 1";

	gtk_source_completion_add_provider(completion,GTK_SOURCE_COMPLETION_PROVIDER(tp),NULL);

//	tp = (TestProvider*)g_object_new (test_provider_get_type (), NULL);
//	tp->priority = 5;
//	tp->name = "Test Provider 5";
//
//	gtk_source_completion_add_provider (completion,
//	                                    GTK_SOURCE_COMPLETION_PROVIDER (tp),
//	                                    NULL);


}


//completion
void removeProv(gpointer data,gpointer user_data)
{
		GtkSourceCompletion * completion=(GtkSourceCompletion *)user_data;
char* name=gtk_source_completion_provider_get_name((GtkSourceCompletionProvider *)data);
printf("%s\n",name);
if(strcasecmp(name,"Document Words")!=0)
	gtk_source_completion_remove_provider(completion,(GtkSourceCompletionProvider *)data,NULL);
}

void addProposals(gpointer data,gpointer user_data)
{
GList *proposals = NULL;
GtkSourceCompletionContext *context=(GtkSourceCompletionContext *)user_data;
TestProvider * prov=(TestProvider *)data;

printf("%s\n",gtk_source_completion_provider_get_name((GtkSourceCompletionProvider *)data));

 g_list_prepend (proposals,gtk_source_completion_item_new ("add 1", "add1", NULL, NULL));
 g_list_prepend (proposals,gtk_source_completion_item_new ("add 2", "add2", NULL, NULL));
 
	gtk_source_completion_context_add_proposals(prov->context,(GtkSourceCompletionProvider *)data,proposals,TRUE);

}

void doCompletionPopUp(pageStruct* page)
{
printf("XXXXXXX\n");
		GtkSourceCompletion *completion;
	completion=gtk_source_view_get_completion(page->view);
//	gtk_source_completion_unblock_interactive(completion);
GtkSourceCompletionContext *context= gtk_source_completion_create_context(completion,NULL);
//	GtkSourceCompletionWords *prov_words;
//prov_words=gtk_source_completion_words_new(NULL,NULL);

GList *          list=   gtk_source_completion_get_providers (completion);
if(list!=NULL)
	{
		g_list_foreach(list,removeProv,completion);
		createCompletion(page);
//			doCompletionPopUp(page);

//		completion=gtk_source_view_get_completion(page->view);
//		prov_words=gtk_source_completion_words_new(NULL,NULL);
//		gtk_source_completion_words_register(prov_words,gtk_text_view_get_buffer(GTK_TEXT_VIEW(page->view)));
//		gtk_source_completion_add_provider(completion,GTK_SOURCE_COMPLETION_PROVIDER(prov_words),NULL);
//		g_object_set(prov_words, "priority",10, NULL);
	}
else
	{
		createCompletion(page);
//		completion=gtk_source_view_get_completion(page->view);
//		prov_words=gtk_source_completion_words_new(NULL,NULL);
//		gtk_source_completion_words_register(prov_words,gtk_text_view_get_buffer(GTK_TEXT_VIEW(page->view)));
//		gtk_source_completion_add_provider(completion,GTK_SOURCE_COMPLETION_PROVIDER(prov_words),NULL);
//		g_object_set(prov_words, "priority",10, NULL);
//		list=   gtk_source_completion_get_providers (completion);
	}

//		completion=gtk_source_view_get_completion(page->view);
//		context= gtk_source_completion_create_context(completion,NULL);
		list=   gtk_source_completion_get_providers (completion);

gtk_source_completion_show(completion,list,context);

printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
//return;
	//g_signal_emit_by_name ((gpointer)completion,"activate-proposal",completion,NULL);
//	gtk_source_completion_block_interactive(completion);
}


