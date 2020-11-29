# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -Wall -pthread
OBJS = wserver.o wclient.o request.o io_helper.o 

.SUFFIXES: .c .o 

.PHONY: all test clean

all: wserver wclient spin.cgi

wserver: wserver.o request.o io_helper.o
	$(CC) $(CFLAGS) -o wserver wserver.o request.o io_helper.o circular_queue.c heap.c helpers.c thread_pool.c thread_worker.c

wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -o wclient wclient.o io_helper.o -pthread

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

TEST_SFF:
	./wserver -d . -p 5001 -t 5 -b 10 -s SFF &
	./wclient localhost 5001 /test/1.html /test/2.html /test/3.html /test/4.html /test/5.html /test/6.html /test/7.html /test/8.html > /dev/null

TEST_FIFO:
	./wserver -d . -p 5002 -t 5 -b 10 -s FIFO &
	./wclient localhost 5002 /test/1.html /test/2.html /test/3.html /test/4.html /test/5.html /test/6.html /test/7.html /test/8.html > /dev/null


FIFO:
	./wserver -d . -p 5000 -t 5 -b 10 -s FIFO

SFF:
	./wserver -d . -p 5000 -t 5 -b 10 -s SFF

SFNF:
	./wserver -d . -p 5000 -t 5 -b 10 -s SFNF


clean:
	-rm -f $(OBJS) wserver wclient spin.cgi
