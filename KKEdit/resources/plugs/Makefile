PLUGNAME=kkedit-test-plug

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0` -o ${PLUGNAME}.o
	gcc -export-dynamic -shared -Wl,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true