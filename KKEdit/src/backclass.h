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

	private:
		bool		canReturn;;
		GtkNotebook	*notebook;
		historyData	savedPages[10];
		int			saveCnt;		
};

#endif
