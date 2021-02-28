GRAPHVIZ_INCLUDE_DIR=/usr/local/Cellar/graphviz/2.44.1/include/graphviz
GRAPHVIZ_LIB_DIR=/usr/local/Cellar/graphviz/2.44.1/lib
CFLAGS=-std=c99 -g -DDEBUG -Werror -fsanitize=address,undefined -I$(GRAPHVIZ_INCLUDE_DIR) -L$(GRAPHVIZ_LIB_DIR)
LEX=lex
YACC=bison

all: test driver

formula.o: formula.h formula.c
	$(CC) $(CFLAGS) -c formula.c
bdd.o: bdd.h bdd.c
	$(CC) $(CFLAGS) -c bdd.c

to_dot.o: to_dot.c
	$(CC) $(CFLAGS) -c to_dot.c

test: test.c formula.o bdd.o to_dot.o
	$(CC) $(CFLAGS) -o test test.c formula.o bdd.o to_dot.o -lgvc -lcgraph

lex.yy.c: driver.l
	$(LEX) driver.l

driver.tab.c: driver.y
	$(YACC) -d driver.y

driver: driver.tab.c driver.tab.h lex.yy.c formula.o bdd.o to_dot.o
	$(CC) $(CFLAGS) -o driver driver.tab.c lex.yy.c formula.o bdd.o to_dot.o -lgvc -lcgraph


.PHONY: clean
clean:
	-rm formula.o bdd.o to_dot.o test lex.yy.c driver.tab.c driver.tab.h driver out.png
	-rm -r test.dSYM driver.dSYM

