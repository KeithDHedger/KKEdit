/******************************************************
*
*     ©keithhedger Wed 11 Dec 10:41:48 GMT 2013
*     kdhedger68713@gmail.com
*
*     encoding.h
*
*	Thanks to the developers of leafpad for the code below.
*	Tarot Osuji <tarot@sdf.lonestar.org>
*	Original source can be found here:
*	http://savannah.nongnu.org/download/leafpad/leafpad-0.8.17.tar.gz
******************************************************/

#ifndef _ENCODING_
#define _ENCODING_
#if 0
#define MAX_COUNTRY_NUM 10
enum {
	IANA = 0,
	OPENI18N,
	CODEPAGE,
	ENCODING_MAX_ITEM_NUM
};

enum {
	LATIN1 = 0,
	LATIN2,
	LATIN3,
	LATIN4,
	LATINC,
	LATINC_UA,
	LATINC_TJ,
	LATINA,
	LATING,
	LATINH,
	LATIN5,
	CHINESE_CN,
	CHINESE_TW,
	CHINESE_HK,
	JAPANESE,
	KOREAN,
	VIETNAMESE,
	THAI,
	GEORGIAN,
	END_CODE
};

typedef struct
{
	char*	item[ENCODING_MAX_ITEM_NUM];
} EncArray;

const gchar *detect_charset(const gchar *text);
const gchar *get_default_charset(void);
#endif
#endif
