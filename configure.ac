#
#
AC_INIT([KKEdit],[0.0.27],[PROJ],[KKEdit])

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

AC_MSG_CHECKING(whether to build gtk-doc viewer)
AC_ARG_ENABLE(docviewer,
    [  --enable-docviewer         Build the Gtk-Doc viewer.],
    [enable_docviewer=$enableval],
    [enable_docviewer="yes"])
AC_MSG_RESULT($enable_docviewer)
if test "$enable_docviewer" = "yes"; then
	WEBKIT_MODULES="webkit-1.0 >= 1.6.1"
	PKG_CHECK_MODULES(WEBKIT, $WEBKIT_MODULES)
	AC_SUBST(WEBKIT_CFLAGS)
	AC_SUBST(WEBKIT_LIBS)
	AC_DEFINE([BUILDDOCVIEWER], [], [Build gtk-doc viewer])
fi

AC_MSG_CHECKING(whether to include spell checker (aspell))
AC_ARG_ENABLE(aspell,
    [  --enable-aspell         Include spellchecker.],
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
	AC_MSG_NOTICE([No gtksu program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([http://gtk-apps.org/content/show.php/GtkSu?content=158974])
else
	AC_DEFINE([_GTKSU_], [], [Do admin stuff])
	AC_MSG_NOTICE([Using gtksu for admin stuff!])
fi

AC_CHECK_PROG(MANPAGEEDITOR, manpageeditor ,true,false)
if test $MANPAGEEDITOR = false; then
	AC_MSG_NOTICE([No manpageeditor program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([http://gtk-apps.org/content/show.php?content=160219])
else
	AC_DEFINE([_MANPAGEEDITOR_], [], [Manpage Editor])
	AC_MSG_NOTICE([Including 'Manpage Editor' menu item])
fi


#AC_MSG_CHECKING([for glib-networking (glib TLS implementation)])
#save_CFLAGS="$CFLAGS"
#save_LIBS="$LIBS"
#CFLAGS="$CFLAGS $GLIB_CFLAGS"
#LIBS="$LIBS $GLIB_LIBS"
AC_RUN_IFELSE([AC_LANG_PROGRAM([#include <gio/gio.h>],
			       [int main() {
    int supported;
    g_type_init();
    supported = g_tls_backend_supports_tls(g_tls_backend_get_default());

    printf("%s\n", supported ? "YES" : "NO");
}])],
	      [have_glib_networking=yes],
	      [have_glib_networking=no],
	      [have_glib_networking="unknown (cross-compiling)"])
CFLAGS="$save_CFLAGS"
LIBS="$save_LIBS"
AC_MSG_RESULT($have_glib_networking)
#
#AC_ARG_ENABLE(tls-check,
#	      AS_HELP_STRING([--disable-tls-check], [Don't error out if glib-networking is unavailable]))
#if test "$enable_tls_check" != "no" -a "$have_glib_networking" = "no"; then
#	AC_MSG_ERROR([libsoup requires glib-networking for TLS support.



#AC_CHECK_LIB(gio/modules/libgiognutls,[],[AC_DEFINE([_GLIBNETWORKING_], [], [glib-networking])])

#GLIBNETWORKING_MODULES="gtksourceview-2.0 >= 2.9.9"
#PKG_CHECK_MODULES(GTKSRC, $GTKSRC_MODULES)

AC_OUTPUT(Makefile KKEdit/app/Makefile)


