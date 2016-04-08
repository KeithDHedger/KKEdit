
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkedit_SOURCES = $(COMMONSRC)
KKEditProgressBar_SOURCES = ../pole/main.cpp

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

helpfilesdir = $(pkgdatadir)/help
helpfiles_DATA = ../resources/help/*

scriptfilesdir = $(pkgdatadir)/tools
scriptfiles_SCRIPTS = ../resources/tools/*

docfilesdir = $(pkgdatadir)/docs
docfiles_DATA = ../resources/docs/*

stylefilesdir = $(pkgdatadir)/styles
stylefiles_DATA = ../resources/styles/*

transfilesdir = $(localedir)/fr_FR/LC_MESSAGES
transfiles_DATA = ../../po/fr_FR/LC_MESSAGES/kkedit.mo

include_HEADERS = ../src/kkedit-plugins.h

bin_PROGRAMS = kkedit KKEditProgressBar

install-exec-hook:
	install -D ../resources/pixmaps/KKEditPlug.png $(DESTDIR)${prefix}/share/pixmaps/KKEditPlug.png
	install -D ../resources/pixmaps/KKEditPlugMenu.png $(DESTDIR)${prefix}/share/pixmaps/KKEditPlugMenu.png
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	install -D ../resources/pixmaps/KKEdit.desktop $(DESTDIR)${prefix}/share/applications/KKEdit.desktop
	install -D ../resources/pixmaps/KKEditRoot.desktop $(DESTDIR)${prefix}/share/applications/KKEditRoot.desktop

	echo "Exec=${with_gsu_path} kkedit$(MAKEFILESUFFIX) -m %f" >> $(DESTDIR)${prefix}/share/applications/KKEditRoot.desktop
	echo "Exec=kkedit$(MAKEFILESUFFIX) %f" >> $(DESTDIR)${prefix}/share/applications/KKEdit.desktop
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEdit.png
	install -D ../resources/pixmaps/KKEditRoot.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditRoot.png
	install -D ../resources/pixmaps/KKEditPlug.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditPlug.png
	mkdir -vp $(DESTDIR)$(pkgdatadir)/plugins
	mkdir -vp "$(DESTDIR)$(pkgdatadir)/Example External Tools"
	cp -r "../resources/Example External Tools" "$(DESTDIR)$(pkgdatadir)"



