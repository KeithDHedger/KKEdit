
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkedit_SOURCES = $(COMMONSRC)
KKEditProgressBar_SOURCES = ../pole/main.cpp

pixfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

helpfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/help
helpfiles_DATA = ../resources/help/*

scriptfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/tools
scriptfiles_SCRIPTS = ../resources/tools/*

docfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/docs
docfiles_DATA = ../resources/docs/*

stylefilesdir = $(pkgdatadir)$(GTK3SUFFIX)/styles
stylefiles_DATA = ../resources/styles/*

#transfilesdir = $(localedir)/fr_FR/LC_MESSAGES
#transfiles_DATA = ../../po/fr_FR/LC_MESSAGES/kkedit.mo

#include_HEADERS = ../src/kkedit-plugins$(GTK3SUFFIX).h

bin_PROGRAMS = kkedit KKEditProgressBar

install-exec-hook:
	install -D ../resources/pixmaps/KKEditPlug.png $(DESTDIR)${prefix}/share/pixmaps/KKEditPlug$(GTK3SUFFIX).png
	install -D ../resources/pixmaps/KKEditPlugMenu.png $(DESTDIR)${prefix}/share/pixmaps/KKEditPlugMenu$(GTK3SUFFIX).png
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/pixmaps/KKEdit$(GTK3SUFFIX).png
	install -D ../resources/pixmaps/KKEdit.desktop $(DESTDIR)${prefix}/share/applications/KKEdit$(GTK3SUFFIX).desktop
	install -D ../resources/pixmaps/KKEditRoot.desktop $(DESTDIR)${prefix}/share/applications/KKEditRoot$(GTK3SUFFIX).desktop

	install -D ../../po/fr_FR/LC_MESSAGES/kkedit.mo $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES/kkedit$(GTK3SUFFIX).mo
	install -D ../src/kkedit-plugins.h $(DESTDIR)${includedir}/kkedit-plugins$(GTK3SUFFIX).h

	echo "Exec=${with_gsu_path} kkedit$(GTK3SUFFIX) -m %f" >> $(DESTDIR)${prefix}/share/applications/KKEditRoot$(GTK3SUFFIX).desktop
	echo "Exec=kkedit$(GTK3SUFFIX) %f" >> $(DESTDIR)${prefix}/share/applications/KKEdit$(GTK3SUFFIX).desktop
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEdit$(GTK3SUFFIX).png
	install -D ../resources/pixmaps/KKEditRoot.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditRoot$(GTK3SUFFIX).png
	install -D ../resources/pixmaps/KKEditPlug.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditPlug$(GTK3SUFFIX).png
	mkdir -vp "$(DESTDIR)${pkgdatadir}$(GTK3SUFFIX)/Example External Tools"
	cp -r "../resources/Example External Tools" "$(DESTDIR)${pkgdatadir}$(GTK3SUFFIX)"

install-data-hook:
	sed -i 's/kkedit/kkedit$(GTK3SUFFIX)/g' $(DESTDIR)${pkgdatadir}$(GTK3SUFFIX)/tools/*


