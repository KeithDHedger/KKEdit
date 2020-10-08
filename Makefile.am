
export GLOBAL = 1
export VTEVERSION = $(VTEVERS)
export CONFCXXFLAGS = $(CXXFLAGS)
export CONFCPPFLAGS = $(CPPFLAGS)
export CONFLDFLAGS = $(LDFLAGS)

SUBDIRS = KKEdit/app $(PLUGDIRS) $(NEWPLUGINCLUDES)

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

make-deb:
	@mkdir -vp /tmp/deb/
	@mkdir -vp /tmp/deb/control
	@make install DESTDIR=/tmp/deb/data
	@cp packages/debian/control$(GTK3SUFFIX) /tmp/deb/control/control
	@/bin/sed -i "s/Installed-Size: /Installed-Size: `du -s "/tmp/deb/data"|awk '{print $$1}'`/" "/tmp/deb/control/control"
	@/bin/sed -i "s/Version: /Version: $(VERSION)/" "/tmp/deb/control/control"
	@echo "2.0" > /tmp/deb/debian-binary
	@tar -C /tmp/deb/data -cvaf /tmp/deb/data.tar.xz .
	@tar -C /tmp/deb/control -cvaf /tmp/deb/control.tar.xz .
	@ar r /tmp/deb/kkedit$(GTK3SUFFIX)-$(VERSION).deb /tmp/deb/debian-binary /tmp/deb/control.tar.xz /tmp/deb/data.tar.xz
	mv /tmp/deb/kkedit$(GTK3SUFFIX)-$(VERSION).deb /tmp
	@rm -rf /tmp/deb/
