#ifndef BDD_H
#define BDD_H

#include "formula.h"

struct bdd;
typedef struct bdd bdd_t;

struct bdd {
  enum { bddCONSTANT, bddBRANCH } type;
  union {
    struct {
      symbol_t test_symbol;
      bdd_t *if_true, *if_false;
    } branch;
    bool constant;
  } data;
};


#define BDD_ARENA_SZ	( 100U )
typedef struct {
  unsigned int cnt;
  bdd_t data[BDD_ARENA_SZ];
} bdd_arena_t;

// Arena constructor
bdd_arena_t *mk_bdd_arena();

// Constructor from formula
bdd_t *bdd_of_formula(formula_t *formula, bdd_arena_t *arena);

// Utility functions
void pretty_print_bdd(bdd_t *b);

#endif // BDD_H

