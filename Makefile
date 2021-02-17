CFLAGS=-std=c99 -g -DDEBUG -Werror -fsanitize=address,undefined

all: formula bdd test

formula: formula.h formula.c
	$(CC) $(CFLAGS) -c formula.c
bdd: bdd.h bdd.c
	$(CC) $(CFLAGS) -c bdd.c

test: test.c formula.o bdd.o
	$(CC) $(CFLAGS) -o test test.c formula.o bdd.o
	./test

