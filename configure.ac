#
#
AC_INIT([KKEdit],[0.3.4],[PROJ],[KKEdit])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC

X11_MODULES="x11 >= 1.0.0"
PKG_CHECK_MODULES(X11, $X11_MODULES)
AC_SUBST(X11_LIBS)
AC_SUBST(X11_CFLAGS)

AC_MSG_CHECKING(whether to use gtk3)
AC_ARG_ENABLE(gtk3,
    [  --enable-gtk3           Use GTK3.],
    [enable_gtk3=$enableval],
    [enable_gtk3="no"])
AC_MSG_RESULT($enable_gtk3)
if test "$enable_gtk3" = "yes"; then
	AC_DEFINE([_USEGTK3_], [], [Use GTK3])
	AC_DEFINE([_EXECSUFFIX_], ["-3"], [Use GTK3])
	GTK_MODULES="gtk+-3.0 >= 3.10.0"
	GTKSRC_MODULES="gtksourceview-3.0 >= 3.10.0"
	program_transform_name='s,\(^[[kK]].*t.*\),\1-3,'
	PLUGVERSION=3
	GTK3SUFFIX=-3
else
	GTK_MODULES="gtk+-2.0 >= 2.24.0"
	GTKSRC_MODULES="gtksourceview-2.0 >= 2.9.9"
	AC_DEFINE([_EXECSUFFIX_], [""], [Use GTK3])
	PLUGVERSION=2
fi

AC_MSG_CHECKING(Build/Install plugins)
AC_ARG_ENABLE(plugins,AS_HELP_STRING([--enable-plugins],[Build/Install plugins]),[enable_plugs=$enableval],[enable_plugs="yes"])
AC_MSG_RESULT($enable_plugs)
if test "$enable_plugs" = "yes"; then
	PLUGINCLUDES="KKEdit/plugins/TerminalPane KKEdit/plugins/ClipboardPlugin KKEdit/plugins/FileBrowser KKEdit/plugins/SessionManager"
	if test "$enable_gtk3" = "yes"; then
		AC_MSG_CHECKING(Finding vte version)
		PKG_CHECK_EXISTS(vte-2.91,[usevte=91],[usevte=90])
		if test $usevte = 91; then
			VTE_MODULES="vte-2.91 >= 0.40.0"
			VTEVERS=2.91
			AC_MSG_RESULT(using $VTEVERS)
		else
			VTE_MODULES="vte-2.90 >= 0.29.1"
			VTEVERS=2.90
			AC_DEFINE([_VTEVERS290_], [""], [Use vte-2.90])
			AC_MSG_RESULT(using $VTEVERS)
		fi
	else
		VTE_MODULES="vte >= 0.28.2"
	fi
	PKG_CHECK_MODULES(VTE,$VTE_MODULES,[],[AC_MSG_ERROR([No VTE dev files found ... For debian install either libvte-dev for gtk2 build or for gtk3 build libvte-2.91-dev or libvte-2.90-dev, see INSTALL file for details.])])
else
	PLUGINCLUDES=
fi

AC_MSG_CHECKING(Build/Install unstable/new plugins)
AC_ARG_ENABLE(unstable-plugins,AS_HELP_STRING([--disable-unstable-plugins],[Build/Install unstable/new plugins]),[enable_new_plugs=$enableval],[enable_new_plugs="no"])
AC_MSG_RESULT($enable_new_plugs)
if test "$enable_new_plugs" = "yes"; then
	NEWPLUGINCLUDES=KKEdit/unstable-plugins/*Plugin
else
	NEWPLUGINCLUDES=
fi

AC_SUBST(PLUGINCLUDES)
AC_SUBST(NEWPLUGINCLUDES)
AC_SUBST(PLUGVERSION)
AC_SUBST(VTEVERS)
AC_SUBST(GTK3SUFFIX)

PKG_CHECK_MODULES(GTK,$GTK_MODULES)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(GTKSRC,$GTKSRC_MODULES)
AC_SUBST(GTKSRC_CFLAGS)
AC_SUBST(GTKSRC_LIBS)

GLIB_MODULES="gmodule-2.0 >= 2.32.4"
PKG_CHECK_MODULES(GLIB, $GLIB_MODULES)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_MSG_CHECKING(whether to build gtk-doc viewer)
AC_ARG_ENABLE(docviewer,AS_HELP_STRING([--enable-docviewer],[Build the Gtk-Doc viewer]),
    [enable_docviewer=$enableval],
    [enable_docviewer="yes"])
AC_MSG_RESULT($enable_docviewer)
if test "$enable_docviewer" = "yes"; then
if test "$enable_gtk3" = "yes"; then
	WEBKIT_MODULES="webkitgtk-3.0 >= 2.4.0"
else
	WEBKIT_MODULES="webkit-1.0 >= 1.6.1"
fi
	PKG_CHECK_MODULES(WEBKIT, $WEBKIT_MODULES)
	AC_SUBST(WEBKIT_CFLAGS)
	AC_SUBST(WEBKIT_LIBS)
	AC_DEFINE([_BUILDDOCVIEWER_], [], [Build gtk-doc viewer])
fi

AC_ARG_WITH(qt5-docdir,AS_HELP_STRING([ --with-qt5-docdir],[Installtion DIR for Qt5 documentation]),
    [with_qt5_docdir=$withval],
    [with_qt5_docdir="/usr/share/doc/qt5"])

AC_SUBST(with_qt5_docdir)
AC_MSG_RESULT([Setting Qt5 Docs directory to $with_qt5_docdir])

AC_MSG_CHECKING(whether to include spell checker (aspell))
AC_ARG_ENABLE(aspell,AS_HELP_STRING([--enable-aspell],[Include spellchecker]),
    [enable_aspell=$enableval],
    [enable_aspell="yes"])
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

AC_ARG_WITH(gsu-path,AS_HELP_STRING([ --with-gsu-path],[Path to graphical su app]),
    [with_gsu_path=$withval],
    [with_gsu_path="gtksu -- "])

AC_SUBST(with_gsu_path)
AC_MSG_RESULT([Setting path to graphical su app to \"$with_gsu_path\"])

AC_ARG_WITH(debug_level,AS_HELP_STRING([ --with-debug-level],[Debug level ( 0,1,2,3,4 )]),
    [with_debug_level=$withval],
    [with_debug_level=0])
AC_SUBST(with_debug_level)
AC_MSG_RESULT([Setting debug level to $with_debug_level])

AC_OUTPUT(Makefile KKEdit/app/Makefile)


