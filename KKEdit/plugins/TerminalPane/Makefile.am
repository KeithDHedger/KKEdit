
AUTOMAKE_OPTIONS = subdir-objects
#GTKVERSION?=2

#bin_PROGRAMS = libterminalpane
#docfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
#docfiles_DATA = libterminalpane.so

all:
	gcc ${EXTRAMAKEFLAGS} $(CONFCXXFLAGS) $(CONFCPPFLAGS) $(CONFLDFLAGS) -fPIC -Wall -g -c terminalpane.cpp `pkg-config --cflags --libs gtk+-$(GTKVERSION).0 gmodule-2.0 glib-2.0 gtksourceview-${GTKVERSION}.0 vte${VTESUFFIX}` -DLOCALEDIR=\"${LOCALEFOLDER}\" $(DEVMODE) $(USEGTK3) $(USE290) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o terminalpane.o


	gcc -shared -Wl,--version-script=exportmap,-soname,libterminalpane.so -o libterminalpane.so terminalpane.o -lc `pkg-config --libs vte${VTESUFFIX}`

clean:
	rm *.o *.so

install:
	mkdir -vp $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
	cp libterminalpane.so $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)/plugins-gtk
