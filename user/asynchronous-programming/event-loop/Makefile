CC=gcc
CFLAGS=-g -Wall -Wshadow -Wvla -Werror -pedantic -Wno-unused-function
DEBUG=gdb
TOOL=valgrind
LDFLAGS=-lpthread
EXE=main

all: $(EXE)

$(EXE): ev-loop.o main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

ev-loop.o: ev-loop.c
	$(CC) -c $(CFLAGS) $< -o $@

main.o: main.c
	$(CC) -c $(CFLAGS) $< -o $@

debug:
	$(DEBUG) $(EXE)

clean:
	rm -f *.o $(EXE)

run: $(EXE)
	$(TOOL) ./$(EXE)