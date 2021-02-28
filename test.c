#include <gvc.h>
#include <stdio.h>
#include <unistd.h>
#include "bdd.h"

extern void bdd_to_dot(GVC_t *, bdd_t *);

int main() {

  formula_arena_t *fa = mk_formula_arena();
  formula_t *f = parse_formula("(((~A)&((~B)&(~C)))|(A&B))|(B&C)", fa);

  bdd_arena_t *ba = mk_bdd_arena();
  bdd_t *b = bdd_of_formula(f, ba);
  pretty_print_bdd(b);

  GVC_t *gvc = gvContext();
  char *args[] = { "dot", "-oout.png", "-Tpng" };
  gvParseArgs(gvc, sizeof(args)/sizeof(char *), args);
  bdd_to_dot(gvc, b);
  gvFreeContext(gvc);

  char app[] = "/usr/bin/open";
  char *const argv[] = { app, "out.png", NULL };
  if (execv(app, argv) < 0) {
    fprintf(stderr, "execv error\n");
  }

  return 0;
}

