/******************************************************
*
*     ©keithhedger Sat 11 Oct 11:51:45 BST 2014
*     kdhedger68713@gmail.com
*
*     completion.h
* 
******************************************************/

#ifndef _USEQT5_
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#endif

#ifndef _COMPLETION_
#define _COMPLETION_
#if 0
typedef struct _FunctionProvider FunctionProvider;
typedef struct _FunctionProviderClass FunctionProviderClass;

struct _FunctionProvider
{
	GObject						parent;
	GList*						proposals;
	gint						priority;
	const char*					name;
	GtkSourceCompletionContext*	context;
	GdkPixbuf*					icon;
};

void function_provider_iface_init(GtkSourceCompletionProviderIface* iface);
GType function_provider_get_type(void);

extern FunctionProvider*			funcProv;
extern FunctionProvider*			varsProv;
extern GtkSourceCompletionWords*	docWordsProv;
extern FunctionProvider*			customProv;
extern bool							forcePopup;

void doCompletionPopUp(pageStruct* page);
void createCompletion(pageStruct* page);
void addProp(pageStruct* page);
void removeProps(void);

#endif
#endif
