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

class HistoryClass
{
	public:
		HistoryClass();
		~HistoryClass();

		void		getThisPoint(void);
		pageStruct*	getPage(void);
		TextBuffer*	getTextBuffer(void);
		bool		savePosition(void);
		void		setPage(pageStruct* page);
		int			getTabNumForPage(void);
	private:
		pageStruct*		savedPage;
		TextBuffer*		buf;
};

#endif
