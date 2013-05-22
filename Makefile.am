
SUBDIRS = KKEdit/app

remake:
	$(shell ./remakesourcelist)

makepkg:
	svn commit -m ""
	cd /tmp
	cd /tmp && svn co file:///home/keithhedger/Development/SVN/KKEdit
	find /tmp/KKEdit -name ".svn" -exec rm -r '{}' \;
	cd /tmp/KKEdit && tar -czvf ../KKEdit-$(cat KKEdit/src/globals.h|grep KKEDIT_VERSION|awk '{ print $3}'|sed 's/"//g').tar.gz .