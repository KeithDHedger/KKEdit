
export GLOBAL = 1
export GTKVERSION = $(PLUGVERSION)

SUBDIRS = KKEdit/app $(PLUGINCLUDES)

remake:
	$(shell ./remakesourcelist)
	
uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEdit$(GTK3SUFFIX).desktop
	rm -f $(DESTDIR)${prefix}/share/applications/KKEditRoot$(GTK3SUFFIX).desktop
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEdit$(GTK3SUFFIX).png
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditRoot$(GTK3SUFFIX).png
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditPlug$(GTK3SUFFIX).png
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditPlug$(GTK3SUFFIX).png
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditPlugMenu$(GTK3SUFFIX).png
	rm -fr  $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)

update-icons:
	gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor
