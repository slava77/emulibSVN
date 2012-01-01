CC = gcc
CFLAGS = -g -Wall -I../include -DSJY73A -DDEBUG
LIBRARY = ../lib/libsjy.a


all	: $(LIBRARY) test_inquiry

.c.o:
	$(CC) $(CFLAGS) -c $*.c

test_inquiry:	test_inquiry.o $(LIBRARY)
	$(CC) $(CFLAGS) test_inquiry.o $(LIBRARY) -o $@

clean:
	rm test_inquiry test_inquiry.o
