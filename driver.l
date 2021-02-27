%{
#include <stdio.h>
#include "driver.tab.h"
%}

%%
[A-Z]	{
	  ECHO;
	  yylval = yytext[0];
	  return SYMBOL;
	}

.	{
	  ECHO;
	  return *yytext;
	}
%%

