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
	int			pageID;
};


class HistoryClass
{
	public:
		HistoryClass(GtkNotebook *nb);
		~HistoryClass();

		int			getTabNumForPagexx(void);
		bool		canGoBack(void);
		void		goBack(void);
		void		goForward(void);
		void		goToPos(void);
		void 		saveLastPos(void);
		void 		saveLastPosAndStop(void);
		int			saveCnt;		
		historyData	savedPages[MAXHIST+4];

	private:
		bool		canReturn;;
		GtkNotebook	*notebook;
};

#endif
