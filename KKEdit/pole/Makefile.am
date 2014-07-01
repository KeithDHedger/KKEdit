
AM_CPPFLAGS = -I$(top_srcdir) \
			$(GTK_CFLAGS) \
			-DPREFIX=\""${prefix}\"" \
			-DDATADIR=\""${pkgdatadir}"\" \
			-DSYSCONFDIR=\""${sysconfdir}/${PACKAGE}"\" \
			-DLIBDIR=\""${libdir}/${PACKAGE}"\"

LIBS =	$(GTK_LIBS)

barberpole_SOURCES = main.cpp

barberpole_CFLAGS = -O3 -export-dynamic
barberpole_CXXFLAGS = -O3 -export-dynamic

poledir = $(pkgdatadir)
pole_PROGRAMS = barberpole

