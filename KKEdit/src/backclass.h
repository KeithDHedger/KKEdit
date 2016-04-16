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

struct backData
{
	const char	*tabName;
	int			lineNumber;
	int			pageID;
	GtkWidget	*child;
};

class HistoryClass
{
	public:
		HistoryClass(GtkNotebook *nb);
		~HistoryClass();

		void		getThisPoint(void);
		pageStruct	*getPage(void);
		TextBuffer	*getTextBuffer(void);
		bool		savePosition(void);
		void		setPage(pageStruct *page);
		int			getTabNumForPage(void);
		bool		canGoBack(void);
		void		setCurrentPos(void);
		void		saveCurrentPos(void);
		void		goBackToPos(void);
		void 		saveLastPos(void);
		void		swapPos(void);

	private:
		pageStruct	*savedPage;
		TextBuffer	*buf;
		bool		goBack;
		GtkNotebook	*notebook;
		backData	currentPos;
		backData	lastPos;
		backData	savePos;
};

#endif
