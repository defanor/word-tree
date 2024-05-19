%{
  #include <stdio.h>
  #include "tree.h"
  #include "parser.h"
  #include "lexer.h"
  void yyerror (struct forest **root, yyscan_t scanner, char const *);
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
%parse-param {struct forest **root}
%param   { yyscan_t scanner }

%union {
  struct tree* exp;
  struct forest* forest;
}
%type <exp> tree
%type <forest> forest
%type <exp> tree_or_val

%%
 /* The grammar */

forest:
%empty { $$ = NULL; }
| forest[F] tree_or_val[T]
{
  if ($F != NULL) {
    plant_tree($F, $T);
    $$ = $F;
  } else {
    $$ = forest_alloc();
    plant_tree($$, $T);
  }
  *root = $$;
};

tree: TREE_START forest[F] TREE_END
{
  $$ = tree_alloc(parenthesized, NULL, 0);
  $$->children = $F;
};

tree_or_val: tree | VAL;
