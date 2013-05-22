
SUBDIRS = KKEdit/app

remake:
	$(shell ./remakesourcelist)

makepkg:
	svn commit -m ""
	cd /tmp
	cd /tmp && svn co file:///home/keithhedger/Development/SVN/KKEdit
	find /tmp/KKEdit -name ".svn" -exec rm -r '{}' \; || true
	cd /tmp/KKEdit && tar -czvf ../KKEdit-$(cat /tmp/KKEdit/KKEdit/src/globals.h|grep KKEDIT_VERSION|awk '{ print $3}'|sed 's/"//g').tar.gz .