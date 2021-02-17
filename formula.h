#ifndef FORMULA_H
#define FORMULA_H

#include <stdbool.h>

typedef char symbol_t;

struct formula;
typedef struct formula formula_t;

struct formula {
  enum formula_type {
    CONSTANT, SYMBOL, NOT, AND, OR, XOR, IMPLIES, IFF
  } type;

  union {
    bool constant;
    symbol_t symbol;
    formula_t *unary;
    struct { formula_t *left, *right; } binary;
  } data;

};


#define FORMULA_ARENA_SZ	( 50U )
typedef struct {
  unsigned int cnt;
  formula_t data[FORMULA_ARENA_SZ];
} formula_arena_t;

// Arena constructor
formula_arena_t *mk_formula_arena();

// Formula constructors per type
formula_t *mk_constant_formula(bool constant, formula_arena_t *arena);
formula_t *mk_symbol(symbol_t symbol, formula_arena_t *arena);
formula_t *mk_not(formula_t *formula, formula_arena_t *arena);
formula_t *mk_and(formula_t *left, formula_t *right, formula_arena_t *arena);
formula_t *mk_or(formula_t *left, formula_t *right, formula_arena_t *arena);
formula_t *mk_xor(formula_t *left, formula_t *right, formula_arena_t *arena);
formula_t *mk_implies(formula_t *left, formula_t *right, formula_arena_t *arena);
formula_t *mk_iff(formula_t *left, formula_t *right, formula_arena_t *arena);

// Formula parser
formula_t *parse_formula(const char *s, formula_arena_t *arena);

#endif // FORMULA_H

