
INCLUDE=.
CC=gcc
CFLAGS=-Wall -g -lreadline -lncurses -I$(INCLUDE)
BUDOBJECTS=buddy.o block.o
OBJECTS=mydash.o error.o Job.o List.o Node.o execute.o

all: mydash

buddy: $(BUDOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(BUDOBJECTS) $(LFLAGS)

mydash: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)  $(LFLAGS)

clean:
	rm --force list *.o mydash buddy
