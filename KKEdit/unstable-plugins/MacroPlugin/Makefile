PACKAGENAME=$(shell /usr/bin/basename `pwd`)
PLUGNAME=$(shell echo ${PACKAGENAME}|/usr/bin/tr A-Z a-z)
MONAME=${PACKAGENAME}.mo
PLATFORM=gtk
GTKVERSION?=2
EXTRAMAKEFLAGS?=
GLOBAL?=0
DEVELOP?=0

PREFIX?=/usr

ifeq (3,$(GTKVERSION))
INSTALLSUFFIX=-3
endif

ifeq (0,$(GLOBAL))
	LOCALEFOLDER=${HOME}/.KKEdit${INSTALLSUFFIX}/plugins-${PLATFORM}/locale
	INSTALLFOLDER=${HOME}/.KKEdit${INSTALLSUFFIX}/plugins-${PLATFORM}
else
	LOCALEFOLDER=$(PREFIX)/share/locale
	INSTALLFOLDER=$(PREFIX)/share/KKEdit${INSTALLSUFFIX}/plugins-${PLATFORM}
endif

ifeq (1,$(DEVELOP))
	DEVMODE=-D_DEVMODE_
endif

all:
	gcc ${EXTRAMAKEFLAGS} -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs ${PLATFORM}+-${GTKVERSION}.0 gmodule-2.0 glib-2.0 gtksourceview-${GTKVERSION}.0` -DLOCALEDIR=\"${LOCALEFOLDER}\" $(DEVMODE) $(USEGTK3) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm -f lib${PLUGNAME}.so||true
	rm -f ${PLUGNAME}.o||true

install:
	gcc ${EXTRAMAKEFLAGS} -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs ${PLATFORM}+-${GTKVERSION}.0 gmodule-2.0 glib-2.0 gtksourceview-${GTKVERSION}.0` -DLOCALEDIR=\"${LOCALEFOLDER}\" $(DEVMODE) $(USEGTK3) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc
	mkdir -vp $(DESTDIR)${INSTALLFOLDER} $(DESTDIR)$(LOCALEFOLDER)/fr_FR/LC_MESSAGES || true
	cp -v lib${PLUGNAME}.so $(DESTDIR)$(INSTALLFOLDER)	
	cp po/fr_FR/LC_MESSAGES/$(PACKAGENAME).mo $(DESTDIR)$(LOCALEFOLDER)/fr_FR/LC_MESSAGES/$(PACKAGENAME)$(INSTALLSUFFIX).mo

uninstall:
	rm -rv $(DESTDIR)$(INSTALLFOLDER)/lib${PLUGNAME}.so $(DESTDIR)$(LOCALEFOLDER)/fr_FR/LC_MESSAGES/$(PACKAGENAME)$(INSTALLSUFFIX).mo||true

makepkg: clean
	cd .. && tar -cvzhf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
