
.phony: static so clean

#all: libfsm

libfsm: src/fsm.c src/fsm.h
    mkdir build
    gcc -std=c99 -I src src/fsm.c -o build/fsm.o
    ar -rc lib/libfsm.a build/fsm.o
    gcc -fPIC -shared build/fsm.o -o lib/libfsm.so
	
clean:
	rm -rf build