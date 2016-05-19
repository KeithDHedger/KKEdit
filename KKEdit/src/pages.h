/*
 *
 * ©K. D. Hedger. Tue 17 May 11:57:12 BST 2016 kdhedger68713@gmail.com

 * This file (pages.h) is part of KKEdit.

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

#ifndef _PAGES_
#define _PAGES_

extern GList	*pages;

pageStruct *getPageStructByIDFromList(unsigned pageid);
pageStruct *getPageStructByIDFromPage(int pagenum);

#endif
