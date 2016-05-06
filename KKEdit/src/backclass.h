/*
 *
 * ©K. D. Hedger. Fri  6 May 10:52:41 BST 2016 kdhedger68713@gmail.com

 * This file (backclass.h) is part of KKEdit.

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

#ifndef _BACKCLASS_
#define _BACKCLASS_

struct historyData
{
	char		*filePath;
	unsigned	lineNumber;
	unsigned	pageID;
	char		*tabName;
	char		*menuLabel;
};


class HistoryClass
{
	public:
		HistoryClass(GtkNotebook *nb,unsigned maxhist);
		~HistoryClass();

		bool		canGoBack(void);
		bool		canGoForward(void);
		void		goBack(void);
		void		goForward(void);
		void		goToPos(void);
		void 		saveLastPos(void);
		void 		saveLastPosAndStop(void);
		unsigned	getSaveCnt(void);
		void		setSaveCnt(int num);
		unsigned	getMaxHist(void);
		historyData	*getHistory(int num);
		void		setHistBackMenu(GtkWidget *menu);
		GtkWidget	*getHistBackMenu(void);
		void		setHistForwardMenu(GtkWidget *menu);
		GtkWidget	*getHistForwardMenu(void);
		void		redoMenus(void);

	private:
		unsigned	maxHist;
		historyData	*savedPages;
		unsigned	saveCnt;		
		bool		backOK;
		bool		forwardOK;
		GtkNotebook	*notebook;
		GtkWidget	*historyBackMenu;
		GtkWidget	*historyForwardMenu;

		void		copyPage(int topage,int frompage);
};

#endif
