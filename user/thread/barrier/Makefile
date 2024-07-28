CC=gcc
CFLAGS=-g -Wall -Wshadow -Wvla -Werror -pedantic -Wno-unused-function
DEBUG=gdb
TOOL=valgrind
LDFLAGS=-lpthread
EXE=main

all: $(EXE)

$(EXE): main.o barrier.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

main.o: main.c
	$(CC) -c $(CFLAGS) $< -o $@

barrier.o: barrier.c
	$(CC) -c $(CFLAGS) $< -o $@

debug:
	$(DEBUG) $(EXE)

clean:
	rm -f *.o $(EXE)

run: $(EXE)
	$(TOOL) ./$(EXE)