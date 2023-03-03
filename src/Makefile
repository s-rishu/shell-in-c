CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: myshell

myshell: myshell.o builtincmd.o util.o validator.o cmdutil.o

myshell.o: myshell.c builtincmd.h util.h validator.h cmdutil.h

builtincmd.o: builtincmd.c builtincmd.h

util.o: util.c util.h

validator.o: validator.c validator.h

cmdutil.o: cmdutil.c cmdutil.h


.PHONY: clean
clean:
	rm -f *.o nyush