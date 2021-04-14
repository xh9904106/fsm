
.phony: all clean

all: libfsm

libfsm: src/fsm.c src/fsm.h
	mkdir build
	$(CC) -std=c99 -I src -c src/fsm.c -o build/fsm.o
	$(AR) -rcs lib/libfsm.a build/fsm.o
	$(CC) -fPIC -shared build/fsm.o -o lib/libfsm.so
	
clean:
	rm -rf build