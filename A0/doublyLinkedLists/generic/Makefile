
INCLUDE=.
CC=gcc
#CFLAGS=-Wall -g -static  -I$(INCLUDE)
CFLAGS=-Wall -g  -I$(INCLUDE)

all: TestList

TestList: TestList.o Object.o List.o Node.o 
	$(CC) $(CFLAGS) -o $@ TestList.o Object.o List.o Node.o  $(LFLAGS)

clean:
	rm --force list *.o TestList
