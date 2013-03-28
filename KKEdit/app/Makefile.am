
include ../flagsandlibs
include ../sources

kkedit_SOURCES = $(COMMONSRC)

kkedit_CFLAGS = -O3 -export-dynamic
kkedit_CXXFLAGS = -O3 -export-dynamic

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/README

bin_PROGRAMS = kkedit

man1_MANS = ../resources/man/kkedit.1

install-exec-hook:
	cp ../resources/pixmaps/KKEdit.desktop /usr/share/applications/KKEdit.desktop
	echo Icon=${prefix}/share/KKEdit/pixmaps/KKEdit.png >> /usr/share/applications/KKEdit.desktop
	echo Exec=${prefix}/bin/kkedit >> /usr/share/applications/KKEdit.desktop
#	update-menus

uninstall-hook:
	rm -f /usr/share/applications/KKEdit.desktop
#	update-menus
	rm -fr $(pkgdatadir)

