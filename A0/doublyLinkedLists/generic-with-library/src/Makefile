
INSTALL=..
INCLUDE=$(INSTALL)/include
LIBS=$(INSTALL)/lib
LFLAGS=-L$(LIBS) -lmylib
CC=gcc
#CFLAGS=-Wall -g  -static  -I$(INCLUDE)
CFLAGS=-Wall -g  -I$(INCLUDE)

all: TestList


TestList: TestList.o Object.o
	$(CC) $(CFLAGS) -o $@ TestList.o Object.o  $(LFLAGS)

clean:
	rm --force list *.o TestList
