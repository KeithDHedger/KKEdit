/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:52:10 GMT 2015 kdhedger68713@gmail.com

 * This file (sliceclass.cpp) is part of KKEdit.

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

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

#include "kkedit-includes.h"

const char* NAMED_ENTITIES[][2] = {
	{"ge;","≥"},
	{"gt;",">"},
	{"lt;","<"},
	{"amp;","&"},
	{"quot;","\""},
	{"AElig;","Æ"},
	{"Aacute;","Á"},
	{"Acirc;","Â"},
	{"Agrave;","À"},
	{"Alpha;","Α"},
	{"Aring;","Å"},
	{"Atilde;","Ã"},
	{"Auml;","Ä"},
	{"Beta;","Β"},
	{"Ccedil;","Ç"},
	{"Chi;","Χ"},
	{"Dagger;","‡"},
	{"Delta;","Δ"},
	{"ETH;","Ð"},
	{"Eacute;","É"},
	{"Ecirc;","Ê"},
	{"Egrave;","È"},
	{"Epsilon;","Ε"},
	{"Eta;","Η"},
	{"Euml;","Ë"},
	{"Gamma;","Γ"},
	{"Iacute;","Í"},
	{"Icirc;","Î"},
	{"Igrave;","Ì"},
	{"Iota;","Ι"},
	{"Iuml;","Ï"},
	{"Kappa;","Κ"},
	{"Lambda;","Λ"},
	{"Mu;","Μ"},
	{"Ntilde;","Ñ"},
	{"Nu;","Ν"},
	{"OElig;","Œ"},
	{"Oacute;","Ó"},
	{"Ocirc;","Ô"},
	{"Ograve;","Ò"},
	{"Omega;","Ω"},
	{"Omicron;","Ο"},
	{"Oslash;","Ø"},
	{"Otilde;","Õ"},
	{"Ouml;","Ö"},
	{"Phi;","Φ"},
	{"Pi;","Π"},
	{"Prime;","″"},
	{"Psi;","Ψ"},
	{"Rho;","Ρ"},
	{"Scaron;","Š"},
	{"Sigma;","Σ"},
	{"THORN;","Þ"},
	{"Tau;","Τ"},
	{"Theta;","Θ"},
	{"Uacute;","Ú"},
	{"Ucirc;","Û"},
	{"Ugrave;","Ù"},
	{"Upsilon;","Υ"},
	{"Uuml;","Ü"},
	{"Xi;","Ξ"},
	{"Yacute;","Ý"},
	{"Yuml;","Ÿ"},
	{"Zeta;","Ζ"},
	{"aacute;","á"},
	{"acirc;","â"},
	{"acute;","´"},
	{"aelig;","æ"},
	{"agrave;","à"},
	{"alefsym;","ℵ"},
	{"alpha;","α"},
	{"and;","∧"},
	{"ang;","∠"},
	{"apos;","'"},
	{"aring;","å"},
	{"asymp;","≈"},
	{"atilde;","ã"},
	{"auml;","ä"},
	{"bdquo;","„"},
	{"beta;","β"},
	{"brvbar;","¦"},
	{"bull;","•"},
	{"cap;","∩"},
	{"ccedil;","ç"},
	{"cedil;","¸"},
	{"cent;","¢"},
	{"chi;","χ"},
	{"circ;","ˆ"},
	{"clubs;","♣"},
	{"cong;","≅"},
	{"copy;","©"},
	{"crarr;","↵"},
	{"cup;","∪"},
	{"curren;","¤"},
	{"dArr;","⇓"},
	{"dagger;","†"},
	{"darr;","↓"},
	{"deg;","°"},
	{"delta;","δ"},
	{"diams;","♦"},
	{"divide;","÷"},
	{"eacute;","é"},
	{"ecirc;","ê"},
	{"egrave;","è"},
	{"empty;","∅"},
	{"emsp;"," "},
	{"ensp;"," "},
	{"epsilon;","ε"},
	{"equiv;","≡"},
	{"eta;","η"},
	{"eth;","ð"},
	{"euml;","ë"},
	{"euro;","€"},
	{"exist;","∃"},
	{"fnof;","ƒ"},
	{"forall;","∀"},
	{"frac12;","½"},
	{"frac14;","¼"},
	{"frac34;","¾"},
	{"frasl;","⁄"},
	{"gamma;","γ"},
	{"hArr;","⇔"},
	{"harr;","↔"},
	{"hearts;","♥"},
	{"hellip;","…"},
	{"iacute;","í"},
	{"icirc;","î"},
	{"iexcl;","¡"},
	{"igrave;","ì"},
	{"image;","ℑ"},
	{"infin;","∞"},
	{"int;","∫"},
	{"iota;","ι"},
	{"iquest;","¿"},
	{"isin;","∈"},
	{"iuml;","ï"},
	{"kappa;","κ"},
	{"lArr;","⇐"},
	{"lambda;","λ"},
	{"lang;","〈"},
	{"laquo;","«"},
	{"larr;","←"},
	{"lceil;","⌈"},
	{"ldquo;","“"},
	{"le;","≤"},
	{"lfloor;","⌊"},
	{"lowast;","∗"},
	{"loz;","◊"},
	{"lrm;","\xE2\x80\x8E"},
	{"lsaquo;","‹"},
	{"lsquo;","‘"},
	{"macr;","¯"},
	{"mdash;","—"},
	{"micro;","µ"},
	{"middot;","·"},
	{"minus;","−"},
	{"mu;","μ"},
	{"nabla;","∇"},
	{"nbsp;"," "},
	{"ndash;","–"},
	{"ne;","≠"},
	{"ni;","∋"},
	{"not;","¬"},
	{"notin;","∉"},
	{"nsub;","⊄"},
	{"ntilde;","ñ"},
	{"nu;","ν"},
	{"oacute;","ó"},
	{"ocirc;","ô"},
	{"oelig;","œ"},
	{"ograve;","ò"},
	{"oline;","‾"},
	{"omega;","ω"},
	{"omicron;","ο"},
	{"oplus;","⊕"},
	{"or;","∨"},
	{"ordf;","ª"},
	{"ordm;","º"},
	{"oslash;","ø"},
	{"otilde;","õ"},
	{"otimes;","⊗"},
	{"ouml;","ö"},
	{"para;","¶"},
	{"part;","∂"},
	{"permil;","‰"},
	{"perp;","⊥"},
	{"phi;","φ"},
	{"pi;","π"},
	{"piv;","ϖ"},
	{"plusmn;","±"},
	{"pound;","£"},
	{"prime;","′"},
	{"prod;","∏"},
	{"prop;","∝"},
	{"psi;","ψ"},
	{"rArr;","⇒"},
	{"radic;","√"},
	{"rang;","〉"},
	{"raquo;","»"},
	{"rarr;","→"},
	{"rceil;","⌉"},
	{"rdquo;","”"},
	{"real;","ℜ"},
	{"reg;","®"},
	{"rfloor;","⌋"},
	{"rho;","ρ"},
	{"rlm;","\xE2\x80\x8F"},
	{"rsaquo;","›"},
	{"rsquo;","’"},
	{"sbquo;","‚"},
	{"scaron;","š"},
	{"sdot;","⋅"},
	{"sect;","§"},
	{"shy;","\xC2\xAD"},
	{"sigma;","σ"},
	{"sigmaf;","ς"},
	{"sim;","∼"},
	{"spades;","♠"},
	{"sub;","⊂"},
	{"sube;","⊆"},
	{"sum;","∑"},
	{"sup;","⊃"},
	{"sup1;","¹"},
	{"sup2;","²"},
	{"sup3;","³"},
	{"supe;","⊇"},
	{"szlig;","ß"},
	{"tau;","τ"},
	{"there4;","∴"},
	{"theta;","θ"},
	{"thetasym;","ϑ"},
	{"thinsp;"," "},
	{"thorn;","þ"},
	{"tilde;","˜"},
	{"times;","×"},
	{"trade;","™"},
	{"uArr;","⇑"},
	{"uacute;","ú"},
	{"uarr;","↑"},
	{"ucirc;","û"},
	{"ugrave;","ù"},
	{"uml;","¨"},
	{"upsih;","ϒ"},
	{"upsilon;","υ"},
	{"uuml;","ü"},
	{"weierp;","℘"},
	{"xi;","ξ"},
	{"yacute;","ý"},
	{"yen;","¥"},
	{"yuml;","ÿ"},
	{"zeta;","ζ"},
	{"zwj;","\xE2\x80\x8D"},
	{"zwnj;","\xE2\x80\x8C"}
};

StringSlice::StringSlice()
{
	ERRDATA
	bufferlen=5;
	buffer=(char*)calloc(bufferlen,1);
	caseless=false;
	duplicate=false;
	ERRDATA
}

StringSlice::~StringSlice()
{
	ERRDATA debugFree(&buffer);
}

/*! Return text between 'startstr' and 'endstr' EXCLUSIVE.
    \param src Haystack.
    \param startstr Start needle.
    \param endstr End needle.
    \return Pointer to static buffer, do not free.
    \note Values of NULL for startstr or endstr are set start and end of src respectively.
*/
char* StringSlice::sliceBetween(char* src,char* startstr,char* endstr)
{
	ERRDATA
	char*	startptr;
	char*	endptr;
	char*	copyofstr;

	this->error=NOERROR;
	if((src==NULL))
		{
			this->error=NULLSRC;
			ERRDATA return(this->buffer);
		}
	copyofstr=strdup(src);

	if(startstr!=NULL)
		{
			if(caseless==true)
				startptr=strcasestr(copyofstr,startstr);
			else
				startptr=strstr(copyofstr,startstr);

			if(startptr==NULL)
				{
					this->error=NOMATCH;
					ERRDATA return(src);
				}
			else
				startptr=(char*)(long)startptr+(long)strlen(startstr);
		}
	else
		startptr=copyofstr;

	if(endstr!=NULL)
		{
			if(caseless==true)
				endptr=strcasestr(startptr,endstr);
			else
				endptr=strstr(startptr,endstr);

			if(endptr==NULL)
				{
					this->error=NOMATCH;
					ERRDATA return(src);
				}
		}
	else
		endptr=&copyofstr[strlen(copyofstr)];

	this->checkBufferLen((long)endptr-(long)startptr+1);
	strncpy(buffer,startptr,(long)endptr-(long)startptr);
	buffer[(long)endptr-(long)startptr]=0;
	ERRDATA debugFree(&copyofstr);
	ERRDATA return(this->returnData(this->buffer));
}

/*! Return text between 'startstr' and 'endstr' INCLUSIVE.
    \param src Haystack.
    \param startstr Start needle.
    \param endstr End needle.
    \return Pointer to static buffer, do not free.
    \note Values of NULL for startstr or endstr are set start and end of src respectively.
*/
char* StringSlice::sliceInclude(char* src,char* startstr,char* endstr)
{
	ERRDATA
	char*	startptr;
	char*	endptr;
	char*	copyofstr;
	int		strlenend;

	this->error=NOERROR;
	if((src==NULL))
		{
			this->error=NULLSRC;
			ERRDATA return(this->buffer);
		}
	copyofstr=strdup(src);

	if(startstr!=NULL)
		{
			if(caseless==true)
				startptr=strcasestr(copyofstr,startstr);
			else
				startptr=strstr(copyofstr,startstr);
			if(startptr==NULL)
				{
					this->error=NOMATCH;
					ERRDATA return(src);
				}
		}
	else
		{
			startptr=copyofstr;
		}

	if(endstr!=NULL)
		{
			strlenend=strlen(endstr);
			if(caseless==true)
				endptr=strcasestr(startptr,endstr);
			else
				endptr=strstr(startptr,endstr);
			if(endptr==NULL)
				{
					this->error=NOMATCH;
					ERRDATA return(src);
				}
		}
	else
		{
			endptr=&copyofstr[strlen(copyofstr)];
			strlenend=0;
		}

	this->checkBufferLen(copyofstr);
	strncpy(buffer,startptr,(long)endptr+strlenend-(long)startptr);
	buffer[(long)endptr+strlenend-(long)startptr]=0;
	ERRDATA debugFree(&copyofstr);
	ERRDATA return(this->returnData(this->buffer));
}

/*! Return 'len' bytes from 'start'.
    \param src Haystack.
    \param start Start character number.
    \param len Number of bytes to return.
    \return Pointer to static buffer, do not free.
    \note Values outside the range of the string or -1 is the same as 0 or strlen(Haystack).
*/
char* StringSlice::sliceLen(char* src,int start,int len)
{
	ERRDATA
	char*	copyofstr;

	this->error=NOERROR;
	if(src==NULL)
		{
			this->error=NULLSRC;
			ERRDATA return(this->buffer);
		}
	copyofstr=strdup(src);
	if(start<0)
		start=0;
	if(len<0)
		len=strlen(copyofstr);

	if(start+len>(int)strlen(copyofstr))
		len=(unsigned int)strlen(copyofstr)-start;

	this->checkBufferLen(start+len+1);
	strncpy(buffer,&copyofstr[start],len);
	buffer[len]=0;
	ERRDATA debugFree(&copyofstr);
	ERRDATA return(this->returnData(this->buffer));
}

/*! Return string from 'start' to 'end' inclusive.
    \param src Haystack.
    \param start Start character number.
    \param end End character number.
    \return Pointer to static buffer, do not free.  
    \note Values outside the range of the string or -1 is the same as 0 or strlen(Haystack).
*/  
char* StringSlice::slice(char* src,int start,int end)
{
	ERRDATA
	char*	copyofstr;

	this->error=NOERROR;
	if(src==NULL)
		{
			this->error=NULLSRC;
			ERRDATA return(this->buffer);
		}
	copyofstr=strdup(src);
	if(start<0)
		start=0;
	if(end<0)
		end=strlen(copyofstr)-1;

	this->checkBufferLen(end-start+1);
	strncpy(buffer,&copyofstr[start],end-start+1);
	buffer[end-start+1]=0;
	ERRDATA debugFree(&copyofstr);
	ERRDATA return(this->returnData(this->buffer));
}

/*! Return 'len' bytes from the END of 'Needle' 
    \param src Haystack.
    \param startstr Needle.
    \param len Number of characters to return.
    \return Pointer to static buffer, do not free.  
    \note Values outside the range of the string or -1 is the same strlen(Haystack).
*/  
char* StringSlice::sliceStrLen(char* src,char* startstr,int len)
{
	ERRDATA
	char*	ptr;
	char*	copyofstr;

	this->error=NOERROR;
	if(src==NULL)
		{
			this->error=NULLSRC;
			ERRDATA return(this->buffer);
		}

	copyofstr=strdup(src);

	if(caseless==true)
		ptr=strcasestr(copyofstr,startstr);
	else
		ptr=strstr(copyofstr,startstr);

	if(ptr==NULL)
		{
			this->error=NOMATCH;
			this->copyToBuffer((const char*)copyofstr);
			ERRDATA return(this->buffer);
		}

	ptr=(char*)(long)ptr+strlen(startstr);
	checkBufferLen(len);
	strncpy(buffer,ptr,len);
	buffer[len]=0;
	ERRDATA debugFree(&copyofstr);
	ERRDATA return(this->returnData(this->buffer));
}

/*! Return bool
    \param none.
    \return Current state of duplicate.  
*/  
bool StringSlice::getDuplicate(void)
{
	ERRDATA return(this->duplicate);
}

/*! Return 'Haystack' - 'Needle'
    \param src Haystack.
    \param delstr Needle.
    \return Pointer to static buffer, do not free.  
*/  
char* StringSlice::deleteSlice(char* src,char* delstr)
{
	ERRDATA
	char*	ptr;
	int		delstrlen;
	int		srcstrlen;
	char*	copyofstr;

	this->error=NOERROR;
	if((src==NULL) || (delstr==NULL))
		{
			this->error=NULLSRC;
			ERRDATA return(this->buffer);
		}

	copyofstr=strdup(src);
	delstrlen=strlen(delstr);
	srcstrlen=strlen(copyofstr);

	if(caseless==true)
		ptr=strcasestr(copyofstr,delstr);
	else
		ptr=strstr(copyofstr,delstr);

	if(ptr==NULL)
		{
			this->error=NOMATCH;
			this->copyToBuffer((const char*)copyofstr);
			ERRDATA debugFree(&copyofstr);
			ERRDATA return(this->buffer);
		}

	checkBufferLen(copyofstr);
	strncpy(buffer,copyofstr,(long)ptr-(long)copyofstr);
	strncpy(&buffer[(long)ptr-(long)copyofstr],(char*)((long)ptr+delstrlen),(long)copyofstr+srcstrlen-((long)ptr+delstrlen));
	buffer[(long)ptr-(long)copyofstr+(long)copyofstr+srcstrlen-((long)ptr+delstrlen)]=0;
	ERRDATA return(this->returnData(this->buffer));
}

/*! Replace first occurence of 'Needle' in 'Haystack'.
    \param src Haystack.
    \param findstr Needle.
    \param replacestr New needles.
    \return Pointer to static buffer, do not free.  
*/  
char* StringSlice::replaceSlice(char* src,char* findstr,char* replacestr)
{
	ERRDATA
	char*	ptr;
	int		startreplace;
	int		finalseglen;
	int		fullsize;
	char*	copyofstr;

	this->error=NOERROR;

	if(src==NULL)
		{
			this->error=NULLSRC;
			ERRDATA return(this->buffer);
		}

	copyofstr=strdup(src);

	if(caseless==true)
		ptr=strcasestr(&copyofstr[this->replaceAllOffset],findstr);
	else
		ptr=strstr(&copyofstr[this->replaceAllOffset],findstr);

	
	if(ptr==NULL)
		{
			this->error=NOMATCH;
			this->copyToBuffer((const char*)copyofstr);
			ERRDATA debugFree(&copyofstr);
			ERRDATA return(this->buffer);
		}

	startreplace=(long)ptr-(long)copyofstr;
	finalseglen=strlen(copyofstr)-startreplace-strlen(findstr);
	this->replaceAllOffset=startreplace+strlen(replacestr);

	fullsize=finalseglen+startreplace+strlen(replacestr);
	this->checkBufferLen(fullsize);
	strncpy(buffer,copyofstr,startreplace);
	buffer[startreplace]=0;
	strncat(buffer,replacestr,strlen(replacestr));
	strncat(&buffer[strlen(buffer)],(char*)(long)ptr+strlen(findstr),finalseglen);
	ERRDATA debugFree(&copyofstr);
	ERRDATA return(this->returnData(this->buffer));
}

/*! Replace all occurences of 'Needle' in 'Haystack'.
    \param src Haystack.
    \param findstr Needle.
    \param replacestr New needles.
    \return Pointer to static buffer, do not free.  
*/  
char* StringSlice::replaceAllSlice(char* src,char* findstr,char* replacestr)
{
	ERRDATA

	const char*	result=NULL;
	int			numresults=0;

	this->error=NOERROR;
	this->replaceAllOffset=0;

	result=this->replaceSlice(src,findstr,replacestr);
	if(this->error==NOERROR)
		numresults--;
	do
		{
			result=this->replaceSlice((char*)result,findstr,replacestr);
			if(this->error==NOERROR)
				numresults--;
		}while(this->error==NOERROR);

	if(numresults!=0)
		error=numresults;

	ERRDATA return(this->returnData(this->buffer));
}

/*! Whether ALL folowing operations are caseless or not.
    \param caseval true=caseless,false=regard case.
    \param findstr Needle.
    \return
*/  
void StringSlice::setCaseless(bool caseval)
{
	ERRDATA
	caseless=caseval;
}

/*! Resize internal buffer if needed based on string length.
    \param str.
    \return
*/  
void StringSlice::checkBufferLen(char* str)
{
	ERRDATA
	if(bufferlen<=strlen(str))
		{
			bufferlen=strlen(str)+1024;
			buffer=(char*)realloc(buffer,bufferlen);
		}
	ERRDATA
}

/*! Resize internal buffer if needed based on value of size.
    \param size.
    \return
*/  
void StringSlice::checkBufferLen(int size)
{
	ERRDATA
	if(bufferlen<=(unsigned)size)
		{
			bufferlen=size+1024;
			buffer=(char*)realloc(buffer,bufferlen);
		}
	ERRDATA
}

/*! Copy str to internal buffer resizing as needed.
    \param str.
    \return
*/  
void StringSlice::copyToBuffer(const char* str)
{
	ERRDATA
	this->checkBufferLen((char*)str);
	strcpy(buffer,str);
	ERRDATA
}

/*! Get error code of last op.
    \return error one of @ref errorCodes
*/  
int StringSlice::getResult(void)
{
	ERRDATA return(this->error);
}

/*! Whether ALL folowing operations return a duplicated string or not.
    \param want true=duplicate string,false=return internal buffer.
    \return
*/  
void StringSlice::setReturnDupString(bool want)
{
	ERRDATA
	duplicate=want;
	ERRDATA
}

//returnData(char* str);
/*! Returns buffer or duplicated buffer.
    \param str Data to return.
    \return const char*
*/  
char* StringSlice::returnData(char* str)
{
	ERRDATA
	if(duplicate==true)
		{
			ERRDATA return(strdup(buffer));
		}
	else
		{
			ERRDATA return(buffer);
		}
}

/*! Returns decoded HTML.
    \param src Data to decode.
    \return char*
*/  
char* StringSlice::decodeHtml(char* src)
{
	ERRDATA
	char*	buf=(char*)calloc(strlen(src),1);
	int		bufchar=0;
	int		srcchar=0;
	char*	ptr;

	this->error=NOERROR;

	if(src==NULL)
		{
			this->error=NULLSRC;
			ERRDATA return(src);
		}

	while(srcchar<(int)strlen(src))
		{
			if(src[srcchar]=='&')
				{
					srcchar++;
					ptr=&src[srcchar];
					for(int j=0;j<NUMHTMLENTRIES;j++)
						{
							if(strncmp(NAMED_ENTITIES[j][0],ptr,strlen(NAMED_ENTITIES[j][0]))==0)
								{
									strcpy(&buf[bufchar],NAMED_ENTITIES[j][1]);
									bufchar=bufchar+strlen(NAMED_ENTITIES[j][1]);
									srcchar=srcchar+strlen(NAMED_ENTITIES[j][0]);
									goto breakout;
								}
						}
				}
			else
				{
					buf[bufchar]=src[srcchar];
					bufchar++;
					srcchar++;
				}
			breakout:;
		}
	this->copyToBuffer(buf);
	ERRDATA debugFree(&buf);
	ERRDATA return(this->returnData(this->buffer));
}

/*! Returns random name 'len' chars long.
    \param len max length of name.
    \return char*
*/  
char* StringSlice::randomName(int len)
{
	ERRDATA
	this->checkBufferLen(len);
	srand(time(0));

	const char alphanum[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for(int i=0;i<len;++i)
       	this->buffer[i]=alphanum[rand()%(sizeof(alphanum)-1)];

    this->buffer[len]=0;
    ERRDATA return(this->returnData(this->buffer));
}


