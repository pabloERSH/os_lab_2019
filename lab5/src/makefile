CC=gcc
CFLAGS=-I.

all: mutex factorial_mod deadlock

mutex: 
	$(CC) -pthread -o mutex mutex.c $(CFLAGS)

factorial_mod: 
	$(CC) -pthread -o factorial_mod factorial_mod.c $(CFLAGS)

deadlock: 
	$(CC) -pthread -o deadlock deadlock.c $(CFLAGS)

clean: 
	rm mutex factorial_mod deadlock