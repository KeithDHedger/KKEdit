
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkedit_SOURCES = $(COMMONSRC)

kkedit_CFLAGS = -ggdb -O0 -export-dynamic
kkedit_CXXFLAGS =  -ggdb -O0 -export-dynamic

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

bin_PROGRAMS = kkedit

install-exec-hook:
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	install -D ../resources/pixmaps/KKEdit.desktop $(DESTDIR)${prefix}/share/applications/KKEdit.desktop
	install -D ../resources/pixmaps/KKEditRoot.desktop $(DESTDIR)${prefix}/share/applications/KKEditRoot.desktop
	echo "Exec=${with_gsu_path} kkedit -m %f" >> $(DESTDIR)${prefix}/share/applications/KKEditRoot.desktop
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEdit.png
	install -D ../resources/pixmaps/KKEditRoot.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditRoot.png
	mkdir -vp "$(DESTDIR)$(pkgdatadir)/Example External Tools"
	cp -r "../resources/Example External Tools" "$(DESTDIR)$(pkgdatadir)"
	gtk-update-icon-cache --ignore-theme-index --force ${prefix}/share/icons/hicolor||true

uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEdit.desktop $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/icons/hicolor/256x256/apps/KKEdit.png $(DESTDIR)${prefix}/share/icons/hicolor/256x256/apps/KKEditRoot.png
	rm -fr $(pkgdatadir)

