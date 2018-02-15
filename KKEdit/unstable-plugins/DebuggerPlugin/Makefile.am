PLUGNAME = debugger
SONAME = libdebugger
SRCFILES = $(PLUGNAME).cpp

AUTOMAKE_OPTIONS = subdir-objects

all:
	gcc ${EXTRAMAKEFLAGS} $(CONFCXXFLAGS) $(CONFCPPFLAGS) $(CONFLDFLAGS) -fPIC -Wall -g -c $(SRCFILES) $(GLIB_CFLAGS) $(GLIB_LIBS) $(GTK_CFLAGS) $(GTK_LIBS) $(GTKSRC_CFLAGS) $(GTKSRC_LIBS) -DLOCALEDIR=\"${localedir}\" -DDATADIR=\"${pkgdatadir}\" $(DEVMODE) $(USEGTK3) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -D_RESOURCES_=\"$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk/$(PLUGNAME)/resources/\" -o $(PLUGNAME).o

	gcc -nodefaultlibs -shared -Wl,-rpath=/usr/local/lib/gcc49,--version-script=exportmap,-soname,$(SONAME).so -o $(SONAME).so $(PLUGNAME).o -lc

clean: distclean

distclean:
	rm -f *.o *.so Makefile||true

install: all
	mkdir -vp $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk/$(PLUGNAME)/resources
	cp $(SONAME).so $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
	cp po/fr_FR/LC_MESSAGES/Debugger.mo  $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES
	cp -r resources $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk/$(PLUGNAME)/resources
