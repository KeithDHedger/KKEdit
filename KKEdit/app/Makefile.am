
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

bin_PROGRAMS = kkedit

install-exec-hook:
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	install -D ../resources/pixmaps/KKEdit.desktop $(DESTDIR)${prefix}/share/applications/KKEdit.desktop

	echo Icon=KKEdit.png >> $(DESTDIR)${prefix}/share/applications/KKEdit.desktop
	echo Exec=kkedit %f >> $(DESTDIR)${prefix}/share/applications/KKEdit.desktop

uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEdit.desktop $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	rm -fr $(pkgdatadir)

