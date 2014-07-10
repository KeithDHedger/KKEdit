
SUBDIRS = KKEdit/resources/example-plugin KKEdit/app KKEdit/pole

remake:
	$(shell ./remakesourcelist)
	
uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKEdit.desktop $(DESTDIR)${prefix}/share/pixmaps/KKEdit.png $(DESTDIR)${prefix}/share/icons/hicolor/256x256/apps/KKEdit.png $(DESTDIR)${prefix}/share/icons/hicolor/256x256/apps/KKEditRoot.png
	rm -fr $(pkgdatadir)
