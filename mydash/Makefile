
INCLUDE=.
CC=gcc
CFLAGS=-Wall -g -lreadline -lncurses -I$(INCLUDE)
OBJECTS=mydash.o error.o Job.o List.o Node.o execute.o

all: mydash

mydash: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)  $(LFLAGS)

clean:
	rm --force list *.o mydash
