#include "formula.h"

#include <assert.h>
#include <stdlib.h>

formula_arena_t *mk_formula_arena() {
  formula_arena_t *arena = malloc(sizeof(formula_arena_t));
  assert (arena != NULL);
  arena->cnt = 0;
  return arena;
}

static formula_t *mk_formula(formula_arena_t *arena) {
  assert (arena->cnt < FORMULA_ARENA_SZ);
  return &arena->data[arena->cnt++];
}

formula_t *mk_constant_formula(bool constant, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = CONSTANT;
  newf->data.constant = constant;
  return newf;
}

formula_t *mk_symbol(symbol_t symbol, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = SYMBOL;
  newf->data.symbol = symbol;
  return newf;
}

formula_t *mk_not(formula_t *formula, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = NOT;
  newf->data.unary = formula;
  return newf;
}

formula_t *mk_and(formula_t *left, formula_t *right, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = AND;
  newf->data.binary.left = left, newf->data.binary.right = right;
  return newf;
}

formula_t *mk_or(formula_t *left, formula_t *right, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = OR;
  newf->data.binary.left = left, newf->data.binary.right = right;
  return newf;
}

formula_t *mk_xor(formula_t *left, formula_t *right, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = XOR;
  newf->data.binary.left = left, newf->data.binary.right = right;
  return newf;
}

formula_t *mk_implies(formula_t *left, formula_t *right, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = IMPLIES;
  newf->data.binary.left = left, newf->data.binary.right = right;
  return newf;
}

formula_t *mk_iff(formula_t *left, formula_t *right, formula_arena_t *arena) {
  formula_t *newf = mk_formula(arena);
  newf->type = IFF;
  newf->data.binary.left = left, newf->data.binary.right = right;
  return newf;
}

// TODO this parsing is not supposed to be fully correct, and doesn't handle op prec
static formula_t *parse_formula_helper(const char **s, formula_arena_t *arena) {
  if ((**s >= 'A' && **s <= 'Z') || **s == '(') {
    formula_t *f;
    if (**s == '(') {
      (*s)++;
      f = parse_formula_helper(s, arena);
      assert (**s == ')');
      (*s)++;
    } else {
      f = mk_symbol(**s, arena);
      (*s)++;
    }
    if (**s == '&') {
      (*s)++;
      formula_t *right = parse_formula_helper(s, arena);
      return mk_and(f, right, arena);
    } else if (**s == '|') {
      (*s)++;
      formula_t *right = parse_formula_helper(s, arena);
      return mk_or(f, right, arena);
    } else if (**s == '^') {
      (*s)++;
      formula_t *right = parse_formula_helper(s, arena);
      return mk_xor(f, right, arena);
    } else if (**s == '>') {
      (*s)++;
      formula_t *right = parse_formula_helper(s, arena);
      return mk_implies(f, right, arena);
    } else if (**s == '=') {
      (*s)++;
      formula_t *right = parse_formula_helper(s, arena);
      return mk_iff(f, right, arena);
    } else {
      return f;
    }
  } else if (**s == '~') {
    (*s)++;
    formula_t *f = parse_formula_helper(s, arena);
    return mk_not(f, arena);
  } else {
    return NULL;
  }
}

formula_t *parse_formula(const char *s, formula_arena_t *arena) {
  return parse_formula_helper(&s, arena);
}

