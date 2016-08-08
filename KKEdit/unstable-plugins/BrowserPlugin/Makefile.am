PLUGNAME = browserplugin
SONAME = libbrowserplugin
SRCFILES = $(PLUGNAME).cpp

AUTOMAKE_OPTIONS = subdir-objects

all:
	gcc49 ${EXTRAMAKEFLAGS} $(CONFCXXFLAGS) $(CONFCPPFLAGS) $(CONFLDFLAGS) -fPIC -Wall -g -c $(SRCFILES) `pkg-config --cflags --libs gtk+-$(GTKVERSION).0 gmodule-2.0 glib-2.0 gtksourceview-${GTKVERSION}.0 webkit-1.0 ` -DLOCALEDIR=\"${localedir}\" -DDATADIR=\"${pkgdatadir}\" $(DEVMODE) $(USEGTK3) $(USE290) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o $(PLUGNAME).o


	gcc49 -nodefaultlibs -shared -Wl,-rpath=/usr/local/lib/gcc49,--version-script=exportmap,-soname,$(SONAME).so -o $(SONAME).so $(PLUGNAME).o -lc `pkg-config --libs  webkit-1.0`


clean: distclean

distclean:
	rm -f *.o *.so||true

install: all
	mkdir -vp $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES
	cp $(SONAME).so $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
	cp po/fr_FR/LC_MESSAGES/$(PLUGNAME).mo  $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES