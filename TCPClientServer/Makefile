CC = gcc
CFLAGS = -Wall -g -Iinclude

all: client server

client: src/TCPclient.c extras.o
	$(CC) $(CFLAGS) -o bin/client src/TCPclient.c extras.o
	
server: src/TCPserver.c extras.o
	$(CC) $(CFLAGS) -o bin/server src/TCPserver.c extras.o

extras.o: src/extras.c 
	$(CC) $(CFLAGS) -c src/extras.c -o extras.o

clean:
	rm *.o
