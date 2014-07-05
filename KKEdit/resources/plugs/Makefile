PLUGNAME=kkedit-test-plug

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.c `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true