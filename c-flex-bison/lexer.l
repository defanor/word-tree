%{
#include "tree.h"
#include "parser.h"
%}

%option reentrant noyywrap bison-bridge header-file="lexer.h"

%%

"(" { return TREE_START; }
")" { return TREE_END; }

[ \n]+ {
yylval->exp = tree_alloc(delimiter_run, yytext, yyleng);
return VAL;
}

([^ \n()\\]|\\[\\() \n])+ {
yylval->exp = tree_alloc(escaped_run, yytext, yyleng);
return VAL;
}

. { ECHO; return FAILURE; }
