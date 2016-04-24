
export GLOBAL = 1
export GTKVERSION = $(PLUGVERSION)

SUBDIRS = KKEdit/app $(PLUGINCLUDES)

remake:
	$(shell ./remakesourcelist)
	
uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEdit.desktop
	rm -f $(DESTDIR)${prefix}/share/applications/KKEditRoot.desktop
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEdit.png
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditRoot.png
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditPlug.png
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditPlug.png
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditPlugMenu.png
	rm -fr  $(DESTDIR)$(pkgdatadir)$(DOCSUFFIX)

update-icons:
	gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor
