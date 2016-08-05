
AUTOMAKE_OPTIONS = subdir-objects

all:
	gcc ${EXTRAMAKEFLAGS} $(CONFCXXFLAGS) $(CONFCPPFLAGS) $(CONFLDFLAGS) -fPIC -Wall -g -c filebrowser.cpp `pkg-config --cflags --libs gtk+-$(GTKVERSION).0 gmodule-2.0 glib-2.0 gtksourceview-${GTKVERSION}.0` -DLOCALEDIR=\"${localedir}\" $(DEVMODE) $(USEGTK3) $(USE290) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o filebrowser.o


	gcc -shared -Wl,--version-script=exportmap,-soname,libfilebrowser.so -o libfilebrowser.so filebrowser.o -lc `pkg-config --libs gtk+-$(GTKVERSION).0`

clean: distclean

distclean:
	rm *.o *.so

install: all
	mkdir -vp $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES
	cp libfilebrowser.so $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
	cp po/fr_FR/LC_MESSAGES/FileBrowser.mo  $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES