/*
 *
 * ©K. D. Hedger. Tue 28 Jun 10:44:42 BST 2016 kdhedger68713@gmail.com
 
 * Projects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * Projects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Projects.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdlib.h>

#include "config.h"
#include "internet.h"

#ifndef _USEQT5_
const char	*moreapps[]={
								"\nMore by the same author\n",
								"KKEdit\nhttp://keithdhedger.github.io/KKEdit/\n",
								"KKTerminal\nhttp://keithhedger.freeddns.org/pages/kkterminal/kkterminal.html\n",
								"KKFileManager\nhttp://github.com/KeithDHedger/KKFileManager\n",
								"XDecorations\nhttp://keithhedger.freeddns.org/pages/xdecorations/xdecorations.html\n",
								"Xfce-Theme-Manager\nhttp://keithhedger.freeddns.org/pages/apps1.html#themeed\n",
								"Xfce4-Composite-Editor\nhttp://keithhedger.freeddns.org/pages/apps1.html#xfcecomp\n",
								"Manpage Editor\nhttp://keithhedger.freeddns.org/pages/manpageeditor/help.html\n",
								"GtkSu\nhttp://keithhedger.freeddns.org/pages/gtksu/gtksu.html\n",
								"ASpell GUI\nhttp://keithhedger.freeddns.org/pages/aspellgui/aspelgui.html\n",
								"Clipboard Viewer\nhttp://keithhedger.freeddns.org/pages/clipviewer/clip.html",
								"\nDevelopment versions can be found here:\nhttp://github.com/KeithDHedger"
							};

const char	*authors[]={"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,moreapps[0],moreapps[1],moreapps[2],moreapps[3],moreapps[4],moreapps[5],moreapps[6],moreapps[7],moreapps[8],moreapps[9],NULL};
#else
const char	*authors="K.D.Hedger ©2013-2014<br><a href=\"mailto:" MYEMAIL "\">Email Me</a><br>" \
				"<a href=\"" GLOBALWEBSITE "\">Homepage</a>" \
				"<br><br>More by the same author<br>" \
				
				"<a href=\"http://keithdhedger.github.io/KKEdit/\">KKEdit<br>" \
				"<a href=\"http://khapplications.freeddns.org/pages/xdecorations/xdecorations.html\">XDecorations<br>" \
				"<a href=\"http://khapplications.freeddns.org/pages/apps.html#themeed\">Xfce-Theme-Manager<br>" \
				"<a href=\"http://khapplications.freeddns.org/pages/apps.html#xfcecomp\">Xfce4-Composite-Editor<br>" \
				"<a href=\"http://khapplications.freeddns.org/pages/manpageeditor/help.html\">Manpage Editor<br>" \
				"<a href=\"http://khapplications.freeddns.org/pages/gtksu/gtksu.html\">GtkSu<br>" \
				"<a href=\"http://khapplications.freeddns.org/pages/aspellgui/aspelgui.html\">ASpell<br>" \
				"<a href=\"http://khapplications.freeddns.org/pages/clipviewer/clip.html\">Clipboard Viewer<br>";
#endif
