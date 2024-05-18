%{
  #include <stdio.h>
  #include "tree.h"
  #include "parser.h"
  #include "lexer.h"
  void yyerror (struct tree **root, yyscan_t scanner, char const *);
%}

%code requires {
  typedef void* yyscan_t;
}

%header "parser.h"
%define parse.trace
%define api.pure
%token
  TREE_START
  TREE_END
  <exp> VAL
  FAILURE
%parse-param {struct tree **root}
%param   { yyscan_t scanner }

%union {
  struct tree* exp;
}
%type <exp> tree
%type <exp> forest
%type <exp> tree_or_val

%%
 /* The grammar */

forest:
%empty { $$ = NULL; }
| forest[F] tree_or_val[C]
{
  if ($F != NULL) {
    $F->last->next = $C;
    $F->last = $C;
    $$ = $F;
  } else {
    $$ = $C;
  }
  *root = $$;
};

tree: TREE_START forest[F] TREE_END
{
  $$ = tree_alloc(parenthesized, NULL, 0);
  $$->children = $F;
  *root = $$;
};

tree_or_val: tree | VAL;
