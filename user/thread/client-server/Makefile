CC=gcc
CFLAGS=-g -Wall -Wshadow -Wvla -Werror -pedantic -Wno-unused-function
DEBUG=gdb
TOOL=valgrind
LDFLAGS=-lpthread

all: listener sender

listener.exe: listener.o udp.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sender.exe: sender.o udp.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

listener.o: listener.c
	$(CC) -c $(CFLAGS) $< -o $@

sender.o: sender.c
	$(CC) -c $(CFLAGS) $< -o $@

udp.o: udp.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.exe