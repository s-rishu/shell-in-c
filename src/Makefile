CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o builtincmd.o util.o

nyush.o: nyush.c builtincmd.h util.h

builtincmd.o: builtincmd.c builtincmd.h

util.o: util.c util.h


.PHONY: clean
clean:
	rm -f *.o nyush