#include "bdd.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

bdd_arena_t *mk_bdd_arena() {
  bdd_arena_t *arena = malloc(sizeof(bdd_arena_t));
  assert (arena != NULL);
  arena->cnt = 0;
  return arena;
}

static bdd_t *mk_bdd(bdd_arena_t *arena) {
  assert (arena->cnt < BDD_ARENA_SZ);
  return &arena->data[arena->cnt++];
}

static bdd_t constant_bdd[2] = {
  { .type = bddCONSTANT, .data.constant = false },
  { .type = bddCONSTANT, .data.constant = true }
};

// Note: LIMPLIES_ stands for <- type and RIMPLIES_ for -> type implication
enum binary_op { AND_, OR_, XOR_, LIMPLIES_, RIMPLIES_, IFF_ };

static inline enum binary_op formula_type_to_binary_op_(enum formula_type type) {
  if (type == AND) return AND_;
  if (type == OR) return OR_;
  if (type == XOR) return XOR_;
  if (type == IMPLIES) return RIMPLIES_;
  if (type == IFF) return IFF_;
  else assert (false); // Type is not a binary op
}

static bdd_t *not_(const bdd_t *bdd, bdd_arena_t *arena);
static bdd_t *bdd_of_binary_(bdd_t *left_bdd, bdd_t *right_bdd, enum binary_op op, bdd_arena_t *arena);

bdd_t *bdd_of_formula(formula_t *formula, bdd_arena_t *arena) {
  switch (formula->type) {
    case CONSTANT:
      return &constant_bdd[formula->data.constant ? 1 : 0];
    case SYMBOL:
    {
      bdd_t *newbdd = mk_bdd(arena);
      newbdd->type = bddBRANCH;
      newbdd->data.branch.test_symbol = formula->data.symbol;
      newbdd->data.branch.if_true = &constant_bdd[1];
      newbdd->data.branch.if_false = &constant_bdd[0];
      return newbdd;
    }
    case NOT:
    {
      bdd_t *of_inner = bdd_of_formula(formula->data.unary, arena);
      return not_(of_inner, arena);
    }
    default:
    {
      bdd_t *of_left = bdd_of_formula(formula->data.binary.left, arena),
	    *of_right = bdd_of_formula(formula->data.binary.right, arena);
      enum binary_op op = formula_type_to_binary_op_(formula->type);
      return bdd_of_binary_(of_left, of_right, op, arena);
    }
  }
}



static bdd_t *not_(const bdd_t *of_bdd, bdd_arena_t *arena) {
  switch (of_bdd->type) {
    case bddCONSTANT:
      return &constant_bdd[of_bdd->data.constant ? 0 : 1];
    case bddBRANCH:
    {
      bdd_t *not_if_true = not_(of_bdd->data.branch.if_true, arena),
	    *not_if_false = not_(of_bdd->data.branch.if_false, arena);
      bdd_t *newbdd = mk_bdd(arena);
      newbdd->type = bddBRANCH;
      newbdd->data.branch.test_symbol = of_bdd->data.branch.test_symbol;
      newbdd->data.branch.if_true = not_if_true;
      newbdd->data.branch.if_false = not_if_false;
      return newbdd;
    }
  }
}

static inline bool do_binary_op_(bool a, bool b, enum binary_op op) {
  switch (op) {
    case AND_: return a & b;
    case OR_: return a | b;
    case XOR_: return a ^ b;
    case LIMPLIES_: return a | (!b);
    case RIMPLIES_: return (!a) | b;
    case IFF_: return a == b;
  }
}

static bdd_t *bdd_of_binary_(bdd_t *left_bdd, bdd_t *right_bdd, enum binary_op op, bdd_arena_t *arena) {
  switch (left_bdd->type) {
    case bddCONSTANT:
    {
      switch (right_bdd->type) {
	case bddCONSTANT:
	{
	  bool result = do_binary_op_(left_bdd->data.constant, right_bdd->data.constant, op);
	  return &constant_bdd[result ? 1 : 0];
	}
	case bddBRANCH:
	{
	  switch (op) {
	    case AND_:
	      if (left_bdd->data.constant) return right_bdd;
	      else return &constant_bdd[0];
	    case OR_:
	      if (left_bdd->data.constant) return &constant_bdd[1];
	      else return right_bdd;
	    case XOR_:
	      if (left_bdd->data.constant) return not_(right_bdd, arena);
	      else return right_bdd;
	    case LIMPLIES_:
	      if (left_bdd->data.constant) return &constant_bdd[1];
	      else return not_(right_bdd, arena);
	    case RIMPLIES_:
	      if (left_bdd->data.constant) return right_bdd;
	      else return &constant_bdd[1];
	    case IFF_:
	      if (left_bdd->data.constant) return right_bdd;
	      else return not_(right_bdd, arena);
	  }
	}
      }
    }
    case bddBRANCH:
    {
      switch (right_bdd->type) {
	case bddCONSTANT:
	  if (op == RIMPLIES_) op = LIMPLIES_;
	  else if (op == LIMPLIES_) op = RIMPLIES_;
	  return bdd_of_binary_(right_bdd, left_bdd, op, arena);
	case bddBRANCH:
	{
	  if (left_bdd->data.branch.test_symbol == right_bdd->data.branch.test_symbol) {
	    bdd_t *newbdd = mk_bdd(arena);
	    newbdd->type = bddBRANCH;
	    newbdd->data.branch.test_symbol = left_bdd->data.branch.test_symbol;
	    newbdd->data.branch.if_true = bdd_of_binary_(
		left_bdd->data.branch.if_true, right_bdd->data.branch.if_true, op, arena);
	    newbdd->data.branch.if_false = bdd_of_binary_(
		left_bdd->data.branch.if_false, right_bdd->data.branch.if_false, op, arena);
	    if (newbdd->data.branch.if_true == newbdd->data.branch.if_false)
	      return newbdd->data.branch.if_true;
	    else return newbdd;
	    // right_bdd and possibly left_bdd is no longer used, arena will take care of freeing them
	  } else if (left_bdd->data.branch.test_symbol < right_bdd->data.branch.test_symbol) {
	    bdd_t *newbdd = mk_bdd(arena);
	    newbdd->type = bddBRANCH;
	    newbdd->data.branch.test_symbol = left_bdd->data.branch.test_symbol;
	    newbdd->data.branch.if_true = bdd_of_binary_(
		left_bdd->data.branch.if_true, right_bdd, op, arena);
	    newbdd->data.branch.if_false = bdd_of_binary_(
		left_bdd->data.branch.if_false, right_bdd, op, arena);
	    if (newbdd->data.branch.if_true == newbdd->data.branch.if_false)
	      return newbdd->data.branch.if_true;
	    else return newbdd;
	  } else {
	    if (op == RIMPLIES_) op = LIMPLIES_;
	    else if (op == LIMPLIES_) op = RIMPLIES_;
	    return bdd_of_binary_(right_bdd, left_bdd, op, arena);
	  }
	}
      }
    }
  }
}

void pretty_print_bdd(bdd_t *b) {
  printf("%p: ", b);
  switch (b->type) {
    case bddCONSTANT:
    {
      printf("value=%d\n", b->data.constant);
    }
      break;
    case bddBRANCH:
    {
      printf("(%c) -> (%p) -> (%p)\n", b->data.branch.test_symbol, b->data.branch.if_true, b->data.branch.if_false);
      pretty_print_bdd(b->data.branch.if_true);
      pretty_print_bdd(b->data.branch.if_false);
    }
      break;
  }
}

