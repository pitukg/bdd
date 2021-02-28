#include <gvc.h>
#include "bdd.h"

static Agnode_t *traverse(bdd_t *, Agraph_t *);

void bdd_to_dot(GVC_t *gvc, bdd_t *bdd) {
  /* Create a digraph */
  Agraph_t *g = agopen("g", Agdirected, 0);

  /* Traverse bdd, adding nodes & edges */
  (void) traverse(bdd, g);

  /* Compute a layout according to the context */
  gvLayoutJobs(gvc, g);

  /* Write the graph according to -T and -o options in the context */
  gvRenderJobs(gvc, g);

  /* Free layout data */
  gvFreeLayout(gvc, g);

  /* Free graph structures */
  agclose(g);
}


union bdd_ptr_to_str {
  bdd_t *bdd;
  char chars[5];
};

static Agnode_t *traverse(bdd_t *bdd, Agraph_t *g) {
  union bdd_ptr_to_str bpts;
  bpts.chars[0] = bpts.chars[1] = bpts.chars[2] = bpts.chars[3] = 0;
  bpts.chars[4] = '\0';
  bpts.bdd = bdd;

  /* Get node, or create if doesn't exist */
  /* Only process node if doesn't yet exist */
  Agnode_t *p = agnode(g, bpts.chars, 0);
  if (NULL != p) return p;
  else p = agnode(g, bpts.chars, 1);

  switch (bdd->type) {
    case bddCONSTANT:
      {
	agsafeset(p, "label", bdd->data.constant ? "T" : "F", "");
      }
      break;
    case bddBRANCH:
      {
	char label[2];
	label[0] = bdd->data.branch.test_symbol;
	label[1] = '\0';
	agsafeset(p, "label", label, "");

	Agnode_t *if_true = traverse(bdd->data.branch.if_true, g);
	Agnode_t *if_false = traverse(bdd->data.branch.if_false, g);

	Agedge_t *to_if_true = agedge(g, p, if_true, 0, 1);
	Agedge_t *to_if_false = agedge(g, p, if_false, 0, 1);

	agsafeset(to_if_true, "style", "filled", "");
	agsafeset(to_if_false, "style", "dotted", "");
      }
      break;
  }

  return p;  
}

