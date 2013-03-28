

include ../flagsandlibs
include ../sources

kkeditdebug_SOURCES = $(COMMONSRC)

kkeditdebug_CFLAGS = -ggdb -O0 -export-dynamic
kkeditdebug_CXXFLAGS = -ggdb -O0 -export-dynamic

bin_PROGRAMS = kkeditdebug

install-exec-local:
	cd $(DESTDIR)$(bindir) && \
	rm kkeditdebug

