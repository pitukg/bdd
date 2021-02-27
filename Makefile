CFLAGS=-std=c99 -g -DDEBUG -Werror -fsanitize=address,undefined
LEX=lex
YACC=bison

all: test driver

formula.o: formula.h formula.c
	$(CC) $(CFLAGS) -c formula.c
bdd.o: bdd.h bdd.c
	$(CC) $(CFLAGS) -c bdd.c

test: test.c formula.o bdd.o
	$(CC) $(CFLAGS) -o test test.c formula.o bdd.o

lex.yy.c: driver.l
	$(LEX) driver.l

driver.tab.c: driver.y
	$(YACC) -d driver.y

driver: driver.tab.c driver.tab.h lex.yy.c
	$(CC) $(CFLAGS) -o driver driver.tab.c lex.yy.c

.PHONY: clean
clean:
	-rm formula.o bdd.o test lex.yy.c driver.tab.c driver.tab.h driver
	-rm -r test.dSYM driver.dSYM

