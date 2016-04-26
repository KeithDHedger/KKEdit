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
};


class HistoryClass
{
	public:
		HistoryClass(GtkNotebook *nb,unsigned maxhist);
		~HistoryClass();

		bool		canGoBack(void);
		void		goBack(void);
		void		goForward(void);
		void		goToPos(void);
		void 		saveLastPos(void);
		void 		saveLastPosAndStop(void);
		unsigned	getSaveCnt(void);
		void		setSaveCnt(int num);
		unsigned	getMaxHist(void);
		historyData	*getHistory(int num);
		void		setHistMenu(GtkWidget *menu);
		GtkWidget	*getHistMenu(void);

	private:
		unsigned	maxHist;
		historyData	*savedPages;
		unsigned	saveCnt;		
		bool		canReturn;;
		GtkNotebook	*notebook;
		GtkWidget	*historyMenu;
		void		appendLocation(void);
		//static void		menuJumpBack(GtkWidget *widget,gpointer data);
};

#endif
