CC=gcc
CFLAGS=-g -Wall -Wshadow -Wvla -Werror -pedantic -Wno-unused-function
DEBUG=gdb
TOOL=valgrind
LDFLAGS=-lpthread
EXE=prod-cons

all: $(EXE)

$(EXE): prod-cons.o queue.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

prod-cons.o: prod-cons.c
	$(CC) -c $(CFLAGS) $< -o $@

queue.o: queue.c
	$(CC) -c $(CFLAGS) $< -o $@

debug:
	$(DEBUG) $(EXE)

clean:
	rm -f *.o $(EXE)

run: $(EXE)
	$(TOOL) ./$(EXE)