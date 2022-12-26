CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: server client

server: tcp_server.o
	${CC} tcp_server.o -o server

client: tcp_client.o
	${CC} tcp_client.o -o client

tcp_server.o: tcp_server.c
	${CC} ${CFLAGS} tcp_server.c

tcp_client.o: tcp_client.c
	${CC} ${CFLAGS} tcp_client.c

clean:
	rm -f *.o *~
