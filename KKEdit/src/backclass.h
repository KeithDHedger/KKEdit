/******************************************************
*
*     ©keithhedger Wed 18 Jun 17:14:17 BST 2014
*     kdhedger68713@gmail.com
*
*     backclass.h
* 
******************************************************/

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
