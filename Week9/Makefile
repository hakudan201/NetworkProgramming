CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: server client

server: server-socket.o
	${CC} server-socket.o -o server -pthread

client: client-socket.o
	${CC} client-socket.o -o client

server-socket.o: server-socket.c
	${CC} ${CFLAGS} server-socket.c

client-socket.o: client-socket.c
	${CC} ${CFLAGS} client-socket.c

clean:
	rm -f *.o *~
