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
  ret->children = NULL;
  ret->next = NULL;
  ret->last = ret;
  ret->raw = NULL;
  ret->raw_len = 0;
  if (raw != NULL) {
    ret->raw = malloc(len + 1);
    for (i = 0, j = 0; i < len; i++, j++) {
      if (raw[i] == '\\') {
        i++;
      }
      ret->raw[j] = raw[i];
    }
    ret->raw[j] = '\0';
    ret->raw_len = j;
  }
  return ret;
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

void print_forest(struct tree *tree, int debug) {
  if (tree->raw != NULL) {
    char *escaped = tree->raw;
    size_t escaped_len = tree->raw_len;
    if (tree->type == escaped_run) {
      escaped = escape(tree->raw, tree->raw_len, &escaped_len);
    }
    if (debug) {
      printf("raw: (%d / %d): '", tree->raw_len, escaped_len);
    }
    size_t i;
    for (i = 0; i < escaped_len; i++) {
      printf("%c", escaped[i]);
    }
    if (debug) {
      printf("'\n");
    }
    free(escaped);
  } else {
    printf("(");
    if (tree->children != NULL) {
      print_forest(tree->children, debug);
    }
    printf(")");
  }
  if (tree->next != NULL) {
    print_forest(tree->next, debug);
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

  struct tree *root;
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
