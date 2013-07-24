
include ../flagsandlibs
include ../sources

kkedit_SOURCES = $(COMMONSRC)

kkedit_CFLAGS = -O3 -export-dynamic
kkedit_CXXFLAGS = -O3 -export-dynamic

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

helpfilesdir = $(pkgdatadir)/help
helpfiles_DATA = ../resources/help/*

scriptfilesdir = $(pkgdatadir)/tools
scriptfiles_SCRIPTS = ../resources/tools/*

bin_PROGRAMS = kkedit

install-exec-hook:
	install -D ../resources/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	install -D ../resources/pixmaps/KKEdit.desktop $(DESTDIR)${prefix}/share/applications/KKEdit.desktop

	echo Icon=${prefix}/share/KKEdit/pixmaps/KKEdit.png >> $(DESTDIR)${prefix}/share/applications/KKEdit.desktop
	echo Exec=${bindir}/kkedit >> $(DESTDIR)${prefix}/share/applications/KKEdit.desktop

uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEdit.desktop $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	rm -fr $(pkgdatadir)

