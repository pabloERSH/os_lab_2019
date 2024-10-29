CC=gcc
CFLAGS=-I.

all: parallel_min_max_timeout zombie process_memory parallel_sum

parallel_min_max_timeout : utils.o find_min_max.o ../../lab3/src/utils.h ../../lab3/src/find_min_max.h
	$(CC) -o parallel_min_max_timeout utils.o find_min_max.o parallel_min_max_timeout.c $(CFLAGS)

utils.o : ../../lab3/src/utils.h
	$(CC) -o utils.o -c ../../lab3/src/utils.c $(CFLAGS)

find_min_max.o : ../../lab3/src/utils.h ../../lab3/src/find_min_max.h
	$(CC) -o find_min_max.o -c ../../lab3/src/find_min_max.c $(CFLAGS)

zombie :
	$(CC) zombie.c -o zombie $(CFLAGS)

process_memory :
	$(CC) process_memory.c -o process_memory $(CFLAGS)

threads_sum.o : threads_sum.h
	$(CC) -c threads_sum.c -o threads_sum.o $(CFLAGS)

libthreads_sum.a : threads_sum.o
	ar rcs libthreads_sum.a threads_sum.o

parallel_sum : utils.o libthreads_sum.a
	$(CC) -pthread -o parallel_sum utils.o parallel_sum.c -L. -lthreads_sum $(CFLAGS)

clean :
	rm utils.o find_min_max.o parallel_min_max_timeout zombie process_memory parallel_sum threads_sum.o libthreads_sum.a