CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: server client

server: udp_server.o
	${CC} udp_server.o -o server

client: udp_client.o
	${CC} udp_client.o -o client

udp_server.o: udp_server.c
	${CC} ${CFLAGS} udp_server.c

udp_client.o: udp_client.c
	${CC} ${CFLAGS} udp_client.c

clean:
	rm -f *.o *~
