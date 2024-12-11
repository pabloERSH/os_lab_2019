CC=gcc
CFLAGS=-I.

all: server client

client : 
	$(CC) -pthread -o client client.c -L. -lmultmodulo $(CFLAGS)

K ?= 8
MOD ?= 1000000
SERVERS ?= servers.txt

run_client : client
	./client --k $(K) --mod $(MOD) --servers $(SERVERS)

server : 
	$(CC) -pthread -o server server.c -L. -lmultmodulo $(CFLAGS)

PORT ?= 20003
TNUM ?= 4

run_server : server
	./server --port $(PORT) --tnum $(TNUM)

clean :
	rm client server