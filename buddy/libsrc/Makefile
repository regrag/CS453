
INSTALL=..
LIB=$(INSTALL)/lib
INCLUDE=.
CC=gcc
CFLAGS=-Wall -g -fPIC -shared -I$(INCLUDE)/
LFLAGS=-L$(LIB) -lmylib

LIBOBJS=block.o buddy.o

all: libs 

libs: libmylib.so  libmylib.a

libmylib.so: $(LIBOBJS)
	$(LD) -shared -fPIC -o $@  $(LIBOBJS)

libmylib.a: $(LIBOBJS)
	$(AR)  rcv $@ $(LIBOBJS)
	ranlib $@

test:
	gcc -g -Wall -o buddytest block.o buddy.o

install: libs
	install --mode=755  libmylib.so $(LIB)/libmylib.so
	install --mode=755  libmylib.a $(LIB)/libmylib.a

installclean:  clean
	rm --force $(LIB)/libmylib.so
	rm --force $(LIB)/libmylib.a

clean:
	rm --force list *.o a.out buddytest core libmylib.a libmylib.so buddy.o block.o

