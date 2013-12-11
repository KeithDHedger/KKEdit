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

static const gchar *country_table[][MAX_COUNTRY_NUM] =
{
	/* LATIN1 */        {NULL},
	/* LATIN2 */        {"cs", "hr", "hu", "pl", "ro", "sk", "sl", "sq", "sr", "uz"},
	/* LATIN3 */        {"eo", "mt", NULL},
	/* LATIN4 */        {"et", "lt", "lv", "mi", NULL},
	/* LATINC */        {"be", "bg", "ky", "mk", "mn", "ru", "tt", NULL},
	/* LATINC_UA */     {"uk", NULL},
	/* LATINC_TJ */     {"tg", NULL},
	/* LATINA */        {"ar", "fa", "ur", NULL},
	/* LATING */        {"el", NULL},
	/* LATINH */        {"he", "yi", NULL},
	/* LATIN5 */        {"az", "tr", NULL},
	/* CHINESE_CN */    {"zh_CN", NULL},
	/* CHINESE_TW */    {"zh_TW", NULL},
	/* CHINESE_HK */    {"zh_HK", NULL},
	/* JAPANESE */      {"ja", NULL},
	/* KOREAN */        {"ko", NULL},
	/* VIETNAMESE */    {"vi", NULL},
	/* THAI */          {"th", NULL},
	/* GEORGIAN */      {"ka", NULL},
};

static const gchar *encoding_table[][ENCODING_MAX_ITEM_NUM] =
{
	                  /*  IANA           OpenI18N       Codepage */
	/* LATIN1 */        { "ISO-8859-1",  "ISO-8859-15", "CP1252" },
	/* LATIN2 */        { "ISO-8859-2",  "ISO-8859-16", "CP1250" },
	/* LATIN3 */        { "ISO-8859-3",  NULL,          NULL },
	/* LATIN4 */        { "ISO-8859-4",  "ISO-8859-13", "CP1257" },
	/* LATINC */        { "ISO-8859-5",  "KOI8-R",      "CP1251" },
	/* LATINC_UA */     { "ISO-8859-5",  "KOI8-U",      "CP1251" },
	/* LATINC_TJ */     { "ISO-8859-5",  "KOI8-T",      "CP1251" },
	/* LATINA */        { "ISO-8859-6",  NULL,          "CP1256" },
	/* LATING */        { "ISO-8859-7",  NULL,          "CP1253" },
	/* LATINH */        { "ISO-8859-8",  NULL,          "CP1255" },
	/* LATIN5 */        { "ISO-8859-9",  NULL,          "CP1254" },
	/* CHINESE_CN */    { "GB2312",      "GB18030",     "CP936" },
	/* CHINESE_TW */    { "BIG5",        "EUC-TW",      "CP950" },
	/* CHINESE_HK */    { "BIG5",        "BIG5-HKSCS",  "CP950" },
	/* JAPANESE */      { "ISO-2022-JP", "EUC-JP",      "CP932" },
	/* KOREAN */        { "ISO-2022-KR", "EUC-KR",      "CP949" },
	/* VIETNAMESE */    { NULL,          "VISCII",      "CP1258" },
	/* THAI */          { NULL,          "TIS-620",     "CP874" },
	/* GEORGIAN */      { NULL,          "GEORGIAN-PS", NULL },
};

typedef struct {
	const gchar *item[ENCODING_MAX_ITEM_NUM];
} EncArray;

#endif
