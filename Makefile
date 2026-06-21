
CC = gcc
CFLAGS=-Wall -O2 -std=c2x
LDFLAGS=

SRC = src/server.c src/client.h
OBJ = server.o

all: clean server client
	
server:
	$(CC) $(CFLAGS) $(SRC) -o server

clean:
	rm -f *.o server

.PHONY: all clean
