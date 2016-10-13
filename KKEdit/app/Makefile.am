
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkedit_SOURCES = $(COMMONSRC)
KKEditProgressBar_SOURCES = ../pole/main.cpp
kkeditmsg_SOURCES = ../kkeditmsg/main.cpp

pixfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

helpfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/help
helpfiles_DATA = ../resources/help/*

scriptfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/*

docfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/docs
docfiles_DATA = ../resources/docs/*

stylefilesdir = $(pkgdatadir)$(GTK3SUFFIX)/styles
stylefiles_DATA = ../resources/styles/*

toolsdir = $(pkgdatadir)$(GTK3SUFFIX)/tools
tools_DATA = ../resources/tools/*

goodiesdir = $(pkgdatadir)$(GTK3SUFFIX)/goodies
goodies_DATA = ../resources/goodies/*

man1_MANS = ../resources/man/kkedit.1

bin_PROGRAMS = kkedit KKEditProgressBar kkeditmsg

install-exec-hook:

	mkdir -p $(DESTDIR)${prefix}/share/pixmaps
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEditPlug$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/pixmaps/KKEditPlug$(GTK3SUFFIX).png
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEditPlugMenu$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/pixmaps/KKEditPlugMenu$(GTK3SUFFIX).png
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEdit$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/pixmaps/KKEdit$(GTK3SUFFIX).png
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEditRoot$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/pixmaps/KKEditRoot$(GTK3SUFFIX).png
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEditProject$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/pixmaps/KKEditProject$(GTK3SUFFIX).png

	mkdir -vp $(DESTDIR)${prefix}/share/applications $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES $(DESTDIR)${includedir} ||true
	install ../resources/pixmaps/KKEdit.desktop $(DESTDIR)${prefix}/share/applications/KKEdit$(GTK3SUFFIX).desktop
	install ../resources/pixmaps/KKEditRoot.desktop $(DESTDIR)${prefix}/share/applications/KKEditRoot$(GTK3SUFFIX).desktop

	install ../../po/fr_FR/LC_MESSAGES/kkedit.mo $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES/kkedit$(GTK3SUFFIX).mo
	install -m 644 ../src/kkedit-plugins.h $(DESTDIR)${includedir}/kkedit-plugins$(GTK3SUFFIX).h

	echo "Icon=KKEditRoot$(GTK3SUFFIX)" >> $(DESTDIR)${prefix}/share/applications/KKEditRoot$(GTK3SUFFIX).desktop
	echo "Exec=${with_gsu_path} kkedit$(GTK3SUFFIX) -m %f" >> $(DESTDIR)${prefix}/share/applications/KKEditRoot$(GTK3SUFFIX).desktop
	echo "Icon=KKEdit$(GTK3SUFFIX)" >> $(DESTDIR)${prefix}/share/applications/KKEdit$(GTK3SUFFIX).desktop
	echo "Exec=kkedit$(GTK3SUFFIX) %f" >> $(DESTDIR)${prefix}/share/applications/KKEdit$(GTK3SUFFIX).desktop

	mkdir -p $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEdit$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEdit$(GTK3SUFFIX).png
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEditRoot$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditRoot$(GTK3SUFFIX).png
	ln -sf ${pkgdatadir}$(GTK3SUFFIX)/pixmaps/KKEditPlug$(GTK3SUFFIX).png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditPlug$(GTK3SUFFIX).png

	mkdir -vp "$(DESTDIR)${pkgdatadir}$(GTK3SUFFIX)/Example External Tools"
	cp -r "../resources/Example External Tools" "$(DESTDIR)${pkgdatadir}$(GTK3SUFFIX)"

install-data-hook:
	$(SEDCOMMAND) 's/kkedit/kkedit$(GTK3SUFFIX)/g' $(DESTDIR)${pkgdatadir}$(GTK3SUFFIX)/tools/*



