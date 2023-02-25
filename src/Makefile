CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o builtincmd.o util.o validator.o

nyush.o: nyush.c builtincmd.h util.h validator.h

builtincmd.o: builtincmd.c builtincmd.h

util.o: util.c util.h

validator.o: validator.c validator.h


.PHONY: clean
clean:
	rm -f *.o nyush