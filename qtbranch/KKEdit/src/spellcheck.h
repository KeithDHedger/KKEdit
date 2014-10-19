/******************************************************
*
*     ©keithhedger Tue 16 Jul 20:46:19 BST 2013
*     kdhedger68713@gmail.com
*
*     spellcheck.h
* 
******************************************************/

#ifndef _SPELLCHECK_
#define _SPELLCHECK_
#ifdef _ASPELL_

#ifndef _USEQT5_
void checkWord(GtkWidget* widget,gpointer data);
#else
//TODO//
void checkWord(void);
#endif
#ifndef _USEQT5_
void doChangeWord(GtkWidget* widget,gpointer data);
#else
//TODO//
void doChangeWord(void);
#endif
#ifndef _USEQT5_
void doAddIgnoreWord(GtkWidget* widget,gpointer data);
#else
//TODO//
void doAddIgnoreWord(void);
#endif
#ifndef _USEQT5_
void doSpellCheckDoc(GtkWidget* widget,gpointer data);
#else
//TODO//
void doSpellCheckDoc(void);
#endif
#ifndef _USEQT5_
void doCancelCheck(GtkWidget* widget,gpointer data);
#else
//TODO//
void doCancelCheck(void);
#endif

#endif
#endif
