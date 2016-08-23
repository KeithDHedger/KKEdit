
AUTOMAKE_OPTIONS = subdir-objects

all:
	gcc ${EXTRAMAKEFLAGS} $(CONFCXXFLAGS) $(CONFCPPFLAGS) $(CONFLDFLAGS) -fPIC -Wall -g -c terminalpane.cpp `pkg-config --cflags --libs vte$(VTEVERS)` $(GLIB_CFLAGS) $(GLIB_LIBS) $(GTK_CFLAGS) $(GTK_LIBS) $(GTKSRC_CFLAGS) $(GTKSRC_LIBS) -DLOCALEDIR=\"${localedir}\" -DDATADIR=\"${pkgdatadir}\" $(DEVMODE) $(USEGTK3) $(USE290) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o terminalpane.o

	gcc -shared -Wl,--version-script=exportmap,-soname,libterminalpane.so -o libterminalpane.so terminalpane.o -lc `pkg-config --libs vte${VTEVERS}`


clean:
	rm -f *.o *.so||true

distclean:
	rm -f *.o *.so Makefile||true

install: all
	mkdir -vp $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES
	cp libterminalpane.so $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
	cp po/fr_FR/LC_MESSAGES/TerminalPane.mo  $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES/TerminalPane$(GTK3SUFFIX).mo