CC=gcc
CFLAGS=-g -Wall -Wshadow -Wvla -Werror -pedantic -Wno-unused-function
DEBUG=gdb
TOOL=valgrind
LDFLAGS=-lpthread
SRC=client.c
OBJ=client.o
EXE=client

all: $(EXE)make

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(OBJ): $(SRC)
	$(CC) -c $(CFLAGS) $< -o $@

debug:
	$(DEBUG) $(EXE)

clean:
	rm -f $(OBJ) $(EXE)

run: $(EXE)
	$(TOOL) ./$(EXE)
