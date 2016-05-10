/*
 *
 * ©K. D. Hedger. Fri  6 May 10:52:58 BST 2016 kdhedger68713@gmail.com

 * This file (completion.h) is part of KKEdit.

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
#ifdef _USEGTK3_
#include <gtksourceview/gtksource.h>
#endif

#ifndef _COMPLETION_
#define _COMPLETION_

typedef struct _FunctionProvider FunctionProvider;
typedef struct _FunctionProviderClass FunctionProviderClass;

struct _FunctionProvider
{
	GObject						parent;
	GList						*proposals;
	gint						priority;
	const char					*name;
	GtkSourceCompletionContext	*context;
	GdkPixbuf					*icon;
};

void function_provider_iface_init(GtkSourceCompletionProviderIface *iface);
GType function_provider_get_type(void);

extern FunctionProvider			*funcProv;
extern FunctionProvider			*varsProv;
extern GtkSourceCompletionWords	*docWordsProv;
extern FunctionProvider			*customProv;
extern bool							forcePopup;

void doCompletionPopUp(pageStruct *page);
void createCompletion(pageStruct *page);
void addProp(pageStruct *page);
void removeProps(void);
void addtoCustomWordList(GtkWidget *widget,gpointer data);

#endif
