
INSTALL=..
INCLUDE=$(INSTALL)/include
LIBS=$(INSTALL)/lib
LFLAGS=-L$(LIBS) -lmylib -lpthread
CC=gcc
CFLAGS=-Wall -g -I$(INCLUDE)

pc: pc.o  Item.o
	$(CC) $(CFLAGS) -o $@ pc.o Item.o  $(LFLAGS)

clean:
	/bin/rm -f *.o pc
