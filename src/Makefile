CC = gcc
CFLAGS = -std=c11 -g -Wall -pedantic -o
OBJS = client server supervisor

all: $(OBJS)

$(OBJS):
	gcc -c $@.c $(CFLAGS) $@_main.o

clean:
	rm -f *.o
