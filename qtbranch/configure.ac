#
#
AC_INIT([KKEdit],[0.3.0],[PROJ],[KKEdit])

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
PKG_PROG_PKG_CONFIG

X11_MODULES="x11 >= 1.0.0"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_LIBS)


#AC_ARG_ENABLE(qt5,[  --enable-qt5		Enable QT5.],[enable_qt5=$enableval],[enable_qt5="no"])

AC_ARG_ENABLE(qt5,AS_HELP_STRING([--enable-qt5],[Enable QT5]),[enable_qt5=$enableval],[enable_qt5="no"])

if test $enable_qt5 = "yes" ;then
	QTWIDGETS_MODULES="Qt5Widgets >= 5.2.1"
	PKG_CHECK_MODULES(QTWIDGETS, $QTWIDGETS_MODULES)
	AC_SUBST(QTWIDGETS_CFLAGS)
	AC_SUBST(QTWIDGETS_LIBS)

	QTCONCURRENT_MODULES="Qt5Concurrent >= 5.2.1"
	PKG_CHECK_MODULES(QTCONCURRENT, $QTCONCURRENT_MODULES)
	AC_SUBST(QTCONCURRENT_CFLAGS)
	AC_SUBST(QTCONCURRENT_LIBS)

	AC_DEFINE([_USEQT5_], [], [Build Qt5])

#	GIO_MODULES="gio-2.0 >= 2.38.2"
#	PKG_CHECK_MODULES(GIO, $GIO_MODULES)
#	AC_SUBST(GIO_CFLAGS)
#	AC_SUBST(GIO_LIBS)

else
	GTK_MODULES="gtk+-2.0 >= 2.24.0"
	PKG_CHECK_MODULES(GTK, $GTK_MODULES)
	AC_SUBST(GTK_CFLAGS)
	AC_SUBST(GTK_LIBS)

	GTKSRC_MODULES="gtksourceview-2.0 >= 2.9.9"
	PKG_CHECK_MODULES(GTKSRC, $GTKSRC_MODULES)
	AC_SUBST(GTKSRC_CFLAGS)
	AC_SUBST(GTKSRC_LIBS)

#	UNIQ_MODULES="unique-1.0 >= 1.1.6"
#	PKG_CHECK_MODULES(UNIQ, $UNIQ_MODULES)
#	AC_SUBST(UNIQ_CFLAGS)
#	AC_SUBST(UNIQ_LIBS)
fi

AM_CONDITIONAL([USEQT5], [test x$enable_qt5 = "xyes"])

X11_MODULES="x11 >= 1.0.0"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_LIBS)


#GTK_MODULES="gtk+-2.0 >= 2.24.0"
#PKG_CHECK_MODULES(GTK, $GTK_MODULES)
#AC_SUBST(GTK_CFLAGS)
#AC_SUBST(GTK_LIBS)

#GTKSRC_MODULES="gtksourceview-2.0 >= 2.9.9"
#PKG_CHECK_MODULES(GTKSRC, $GTKSRC_MODULES)
#AC_SUBST(GTKSRC_CFLAGS)
#AC_SUBST(GTKSRC_LIBS)
#//gobject-2.0
#GOBJ_MODULES="gobject-2.0 >= 2.0.0"
#PKG_CHECK_MODULES(GOBJ, $GOBJ_MODULES)
#AC_SUBST(GOBJ_CFLAGS)
#AC_SUBST(GOBJ_LIBS)

GLIB_MODULES="glib-2.0 >= 2.34.1"
PKG_CHECK_MODULES(GLIB, $GLIB_MODULES)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

GMODULE_MODULES="gmodule-2.0 >= 2.32.4"
PKG_CHECK_MODULES(GMODULE, $GMODULE_MODULES)
AC_SUBST(GMODULE_CFLAGS)
AC_SUBST(GMODULE_LIBS)

AC_MSG_CHECKING(whether to build gtk-doc viewer)
AC_ARG_ENABLE(docviewer,AS_HELP_STRING([--enable-docviewer],[Build the Gtk-Doc viewer]),
    [enable_docviewer=$enableval],
    [enable_docviewer="yes"])
AC_MSG_RESULT($enable_docviewer)
if test "$enable_docviewer" = "yes"; then
	WEBKIT_MODULES="webkit-1.0 >= 1.6.1"
	PKG_CHECK_MODULES(WEBKIT, $WEBKIT_MODULES)
	AC_SUBST(WEBKIT_CFLAGS)
	AC_SUBST(WEBKIT_LIBS)
	AC_DEFINE([_BUILDDOCVIEWER_], [], [Build gtk-doc viewer])
fi

AC_ARG_WITH(qt5-docdir,AS_HELP_STRING([--with-qt5-docdir],[Installtion DIR for Qt5 documentation]),
    [with_qt5_docdir=$withval],
    [with_qt5_docdir="/usr/share/doc/qt5"])

AC_SUBST(with_qt5_docdir)
AC_MSG_RESULT([Setting Qt5 Docs directory to $with_qt5_docdir])

AC_MSG_CHECKING(whether to include spell checker (aspell))
AC_ARG_ENABLE(aspell,AS_HELP_STRING([--enable-aspell],[Include spellchecker]),
    [enable_aspell=$enableval],
    [enable_aspell="no"])
AC_MSG_RESULT($enable_aspell)
if test "$enable_aspell" = "yes"; then
	ASPELL_LIBS=-laspell
	AC_SUBST(ASPELL_LIBS)
	AC_DEFINE([_ASPELL_], [], [Include spellchecker])
fi

AC_CHECK_PROG(CTAGS, ctags ,true,false)
if test $CTAGS = false; then
	AC_MSG_ERROR([No ctags program found!])
fi

AC_CHECK_PROG(GTKSU, gtksu ,true,false)
if test $GTKSU = false; then
	AC_MSG_NOTICE([No GtkSu program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([http://gtk-apps.org/content/show.php/GtkSu?content=158974])
fi

AC_CHECK_PROG(MANPAGEEDITOR, manpageeditor ,true,false)
if test $MANPAGEEDITOR = false; then
	AC_MSG_NOTICE([No Manpage Editor program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([http://gtk-apps.org/content/show.php?content=160219])
fi

AC_ARG_WITH(gsu-path,AS_HELP_STRING([--with-gsu-path],[Path to graphical su app]),
    [with_gsu_path=$withval],
    [with_gsu_path="gtksu -- "])

AC_SUBST(with_gsu_path)
AC_MSG_RESULT([Setting path to graphical su app to \"$with_gsu_path\"])

AC_OUTPUT(Makefile KKEdit/app/Makefile)


