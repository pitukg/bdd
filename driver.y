%{
  #include <gvc.h>
  #include <stdio.h>
  #include <unistd.h>
  #include "bdd.h"
  #include "formula.h"

  #define YYSTYPE formula_t *

  symbol_t yysymbol;
  int yylex();
  void yyerror(const char *);

  formula_arena_t *fa;
  GVC_t *gvc;
  void process_formula(formula_t *);
  void bdd_to_dot(GVC_t *, bdd_t *);
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
	| formula '\n'		{ process_formula($1); return 0 /* TODO fix reuse gvc context*/ }
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

  gvc = gvContext();
  char *args[] = { "dot", "-oout.png", "-Tpng" };
  gvParseArgs(gvc, sizeof(args)/sizeof(char *), args);

  int ret = yyparse();
  ret |= gvFreeContext(gvc);
  return ret;
}

void process_formula(formula_t *formula) {
  bdd_arena_t *ba = mk_bdd_arena();
  bdd_t *bdd = bdd_of_formula(formula, ba);
  bdd_to_dot(gvc, bdd);

  static char app[] = "/usr/bin/open";
  static char *const argv[] = { app, "out.png", NULL };

  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    if (execv(app, argv) < 0) {
      fprintf(stderr, "execv error\n");
    }
  }
}

void yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
}

int yywrap() {
  return 1;
}

