CC=gcc
CFLAGS=-g -Wall -Wshadow -Wvla -Werror -pedantic -Wno-unused-function
DEBUG=gdb
TOOL=valgrind
LDFLAGS=-lpthread
SRC=cleanup.c
OBJ=cleanup.o
EXE=cleanup

all: $(EXE)

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
