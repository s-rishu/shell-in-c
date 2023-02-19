CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o builtincmd.o

nyush.o: nyush.c builtincmd.h

builtincmd.o: builtincmd.c builtincmd.h


.PHONY: clean
clean:
	rm -f *.o nyush