/*
 *
 * ©K. D. Hedger. Fri  6 May 10:49:32 BST 2016 kdhedger68713@gmail.com

 * This file (sliceclass.h) is part of KKEdit.

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

#ifndef _SLICECLASS_
#define _SLICECLASS_

#define NUMHTMLENTRIES 252
enum	errorCodes {NOERROR=0,NOMATCH,NULLSRC,MULTMATCH};

/**
 *
 *
 * \brief String slicing
 *
 * General purpose string slicing class.
 *
 * \note Returns a pointer to a static buffer DO NOT FREE!
 * \note Check error with getResult(), return value can be used as source for next call. 
 *
 *
 */
 
class StringSlice
{
	public:
		StringSlice();
		~StringSlice();

		char		*sliceBetween(char* src,char* startstr,char* endstr);
		char		*sliceInclude(char* src,char* startstr,char* endstr);
		char		*sliceLen(char* src,int start,int len);
		char		*slice(char* src,int start,int end);
		char		*sliceStrLen(char* src,char* startstr,int len);
		char		*deleteSlice(char* src,char* delstr);
		char		*replaceSlice(char* src,char* findstr,char* replacestr);
		char		*replaceAllSlice(char* src,char* findstr,char* replacestr);
		void		setCaseless(bool caseval);
		void		copyToBuffer(const char* str);
		int			getResult(void);
		void		setReturnDupString(bool want);
		char		*decodeHtml(char* src);
		char		*randomName(int len);
		bool		getDuplicate(void);
	private:
		void		checkBufferLen(char* str);
		void		checkBufferLen(int size);
		char		*returnData(char* str);
		char		*buffer;
		bool		caseless;
		int			error;
		bool		duplicate;
		unsigned	bufferlen;
		unsigned	replaceAllOffset;
};

#endif

