
export GLOBAL = 1
export GTKVERSION = $(PLUGVERSION)
export VTEVERSION = $(VTEVERS)

SUBDIRS = KKEdit/app $(PLUGINCLUDES) $(NEWPLUGINCLUDES)

remake:
	$(shell ./remakesourcelist)
	
uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEdit$(GTK3SUFFIX).desktop||true
	rm -f $(DESTDIR)${prefix}/share/applications/KKEditRoot$(GTK3SUFFIX).desktop||true
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEdit$(GTK3SUFFIX).png||true
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditRoot$(GTK3SUFFIX).png||true
	rm -f $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKEditPlug$(GTK3SUFFIX).png||true
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEdit$(GTK3SUFFIX).png||true
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditRoot$(GTK3SUFFIX).png||true
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditPlug$(GTK3SUFFIX).png||true
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKEditPlugMenu$(GTK3SUFFIX).png||true
	rm -fr $(DESTDIR)$(pkgdatadir)$(GTK3SUFFIX)||true
	rm -fr $(DESTDIR)${localedir}/fr_FR/LC_MESSAGES/kkedit$(GTK3SUFFIX).mo||true
	rm -f $(DESTDIR)${includedir}/kkedit-plugins$(GTK3SUFFIX).h||true

update-icons:
	gtk-update-icon-cache -q -t -f ${prefix}/share/icons/hicolor
