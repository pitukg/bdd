%{
  #include <stdio.h>
  #include "driver.tab.h"
  #include "formula.h"

  extern symbol_t yysymbol;
%}

%%
[A-Z]	{
	  yysymbol = yytext[0];
	  return T_SYMBOL;
	}

\n	{
	  return *yytext;
	}

[ \t]	{
	  // Ignore space or tab
	}

.	{
	  return *yytext;
	}
%%

