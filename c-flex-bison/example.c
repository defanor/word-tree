#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "parser.h"
#include "lexer.h"
#include <string.h>

void yyerror (struct tree **root, yyscan_t scanner, char const *s)
{
  fprintf(stderr, "error: %s\n", s);
}

struct tree *tree_alloc(enum tree_type type, char *raw, size_t len) {
  size_t i, j;
  struct tree *ret = malloc(sizeof(struct tree));
  ret->type = type;
  if (type == parenthesized) {
    ret->children = NULL;
  } else {
    ret->literal.data = NULL;
    ret->literal.len = 0;
    if (raw != NULL) {
      ret->literal.data = malloc(len + 1);
      for (i = 0, j = 0; i < len; i++, j++) {
        if (raw[i] == '\\') {
          i++;
        }
        ret->literal.data[j] = raw[i];
      }
      ret->literal.data[j] = '\0';
      ret->literal.len = j;
    }
  }
  return ret;
}

struct forest *forest_alloc () {
  struct forest *ret = malloc(sizeof(struct forest));
  ret->trees = NULL;
  ret->tree_num = 0;
  return ret;
}

void plant_tree (struct forest *forest, struct tree *tree) {
  forest->trees =
    reallocarray(forest->trees, forest->tree_num + 1, sizeof(struct tree*));
  forest->trees[forest->tree_num] = tree;
  forest->tree_num = forest->tree_num + 1;
}

char *escape (const char *s, size_t len, size_t *out_len) {
  char *ret = malloc(len * 2 + 1);
  int i, j;
  for (i = 0, j = 0; i < len; i++, j++) {
    if (s[i] == '\\' || s[i] == '(' || s[i] == ')' ||
        s[i] == ' ' || s[i] == '\n') {
      ret[j] = '\\';
      j++;
    }
    ret[j] = s[i];
  }
  ret[j] = '\0';
  *out_len = j;
  return ret;
}

void print_tree (struct tree *tree, int debug) {
  if (tree->type == parenthesized) {
    printf("(");
    if (tree->children != NULL) {
      print_forest(tree->children, debug);
    }
    printf(")");
  } else {
    char *escaped = tree->literal.data;
    size_t escaped_len = tree->literal.len;
    if (tree->type == escaped_run) {
      escaped = escape(tree->literal.data, tree->literal.len, &escaped_len);
    }
    if (debug) {
      printf("raw: (%d / %d): '", tree->literal.len, escaped_len);
    }
    size_t i;
    for (i = 0; i < escaped_len; i++) {
      printf("%c", escaped[i]);
    }
    if (debug) {
      printf("'\n");
    }
    free(escaped);
  }
}

void print_forest (struct forest *forest, int debug) {
  int i;
  for (i = 0; i < forest->tree_num; i++) {
    print_tree(forest->trees[i], debug);
  }
}

int main (int argc, char **argv)
{
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init(&scanner)) {
    /* could not initialize */
    return -1;
  }

  state = yy_scan_string(argv[1], scanner);

  struct forest *root;
  /* yydebug = 1; */
  if (yyparse (&root, scanner)) {
    puts("parse failed");
  } else {
    puts("parsed:");
    print_forest(root, 0);
    printf("\n");
  }
  return 0;
}
