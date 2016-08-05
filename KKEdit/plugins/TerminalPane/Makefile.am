
AUTOMAKE_OPTIONS = subdir-objects

all:
	gcc ${EXTRAMAKEFLAGS} $(CONFCXXFLAGS) $(CONFCPPFLAGS) $(CONFLDFLAGS) -fPIC -Wall -g -c terminalpane.cpp `pkg-config --cflags --libs gtk+-$(GTKVERSION).0 gmodule-2.0 glib-2.0 gtksourceview-${GTKVERSION}.0 vte$(VTEVERS)` -DLOCALEDIR=\"${localedir}\" $(DEVMODE) $(USEGTK3) $(USE290) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o terminalpane.o


	gcc -shared -Wl,--version-script=exportmap,-soname,libterminalpane.so -o libterminalpane.so terminalpane.o -lc `pkg-config --libs vte${VTEVERS}`


clean: distclean

distclean:
	rm *.o *.so

install: all
	mkdir -vp $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES
	cp libterminalpane.so $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
	cp po/fr_FR/LC_MESSAGES/TerminalPane.mo  $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES