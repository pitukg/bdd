%{
#include <stdio.h>
int yylex();
void yyerror(char *);
%}

%start command
%token SYMBOL
%left '|'
%left '&'

%%
command : formula ';'
	;
formula	: formula '|' formula	{ $$ = 1; }
	| formula '&' formula	{ $$ = 1; }
	| SYMBOL		{ /* char in yylval */ $$ = 1; }
	;
%%

int main() {
  return yyparse();
}


void yyerror(char *s) {
  fprintf(stderr, "%s\n", s);
}

int yywrap() {
  return 1;
}

