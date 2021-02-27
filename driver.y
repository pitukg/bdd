%{
  #include <stdio.h>
  #include "bdd.h"
  #include "formula.h"

  #define YYSTYPE formula_t *

  symbol_t yysymbol;

  int yylex();
  void yyerror(const char *);
  formula_arena_t *fa;
  void process_formula(formula_t *);
%}

%start input

%token T_SYMBOL

%nonassoc '>' '='
%left '|'
%right '~'
%left '&'

%%
input	: /* empty */
	| input line
	;

line	: '\n'
	| formula '\n'		{ process_formula($1) }
	;

formula : T_SYMBOL		{ $$ = mk_symbol(yysymbol, fa) }
	| formula '|' formula	{ $$ = mk_or($1, $3, fa) }
	| formula '&' formula	{ $$ = mk_and($1, $3, fa) }
	| formula '>' formula	{ $$ = mk_implies($1, $3, fa) }
	| formula '=' formula	{ $$ = mk_iff($1, $3, fa) }
	| '~' formula		{ $$ = mk_not($2, fa) }
	| '(' formula ')'	{ $$ = $2 }
	;
%%

int main() {
  fa = mk_formula_arena();
  return yyparse();
}

void process_formula(formula_t *formula) {
  bdd_arena_t *ba = mk_bdd_arena();
  bdd_t *bdd = bdd_of_formula(formula, ba);
  pretty_print_bdd(bdd);
  printf("\n");
}

void yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
}

int yywrap() {
  return 1;
}

