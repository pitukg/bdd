#include "bdd.h"

int main() {

  formula_arena_t *fa = mk_formula_arena();
  formula_t *f = parse_formula("(((~A)&((~B)&(~C)))|(A&B))|(B&C)", fa);

  bdd_arena_t *ba = mk_bdd_arena();
  bdd_t *b = bdd_of_formula(f, ba);
  pretty_print_bdd(b);

  return 0;
}

