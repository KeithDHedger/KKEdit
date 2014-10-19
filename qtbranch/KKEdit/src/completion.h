/******************************************************
*
*     ©keithhedger Sat 11 Oct 11:51:45 BST 2014
*     kdhedger68713@gmail.com
*
*     completion.h
* 
******************************************************/

#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>

#ifndef _COMPLETION_
#define _COMPLETION_

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

#ifndef _USEQT5_
void function_provider_iface_init(GtkSourceCompletionProviderIface* iface);
#else
//TODO//
void function_provider_iface_init(void);
#endif
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
