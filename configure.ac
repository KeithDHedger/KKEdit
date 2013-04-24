#
#
AC_INIT([KKEdit],[0.0],[PROJ],[KKEdit])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AM_PROG_CC_C_O

CXXFLAGS=-Wall
AC_SUBST(CXXFLAGS)

CFLAGS=-Wall
AC_SUBST(CFLAGS)

GTK_MODULES="gtk+-2.0 >= 2.24.0"
PKG_CHECK_MODULES(GTK, $GTK_MODULES)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GTKSRC_MODULES="gtksourceview-2.0 >= 2.9.9"
PKG_CHECK_MODULES(GTKSRC, $GTKSRC_MODULES)
AC_SUBST(GTKSRC_CFLAGS)
AC_SUBST(GTKSRC_LIBS)

UNIQ_MODULES="unique-1.0 >= 1.1.6"
PKG_CHECK_MODULES(UNIQ, $UNIQ_MODULES)
AC_SUBST(UNIQ_CFLAGS)
AC_SUBST(UNIQ_LIBS)


AC_MSG_CHECKING(build gtk-doc viewer)
AC_ARG_ENABLE(docviewer,
    [  --enable-docviewer         Build the Gtk-Doc viewer.],
    [enable_docviewer=$enableval],
    [enable_docviewer="yes"])
AC_MSG_RESULT($enable_docviewer)
if test "$enable_docviewer" = "yes"; then
	WEBKIT_MODULES="webkit-1.0 >= 1.10.0"
	PKG_CHECK_MODULES(WEBKIT, $WEBKIT_MODULES)
	AC_SUBST(WEBKIT_CFLAGS)
	AC_SUBST(WEBKIT_LIBS)
	AC_DEFINE([BUILDDOCVIEWER], [], [Build gtk-doc viewer])
fi

AC_CHECK_PROG(CTAGS, ctags ,true,false)
if test $CTAGS = false; then
	AC_MSG_ERROR([No ctags program found!])
fi

AC_OUTPUT(Makefile KKEdit/app/Makefile)


