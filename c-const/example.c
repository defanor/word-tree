/*
  An example of parsing in plain C, without memory allocations, and
  with adjustable maximum parsing depths for literals and trees.

  An example:
  cc example.c && ./a.out 1 0 'a b() (c\ d\(\)  (e (f)))'
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int is_space (char c) {
  return (c == ' ' || c == '\n');
}

ssize_t read_token (const char *input, size_t input_len, int delimited) {
  size_t i;

  /* A tree: match until it is closed */
  if (input[0] == '(') {
    int level = 0;
    for (i = 0; i < input_len; i++) {
      if (input[i] == '(') {
        level++;
      } else if (input[i] == ')') {
        level--;
      } else if (input[i] == '\\') {
        i++;
      }
      if (level == 0) {
        return i + 1;
      }
    }
    return -1;
  }

  /* A literal */
  for (i = 0; i < input_len; i++) {
    if ((! is_space(input[0])) && input[i] == '\\') {
      i++;
      continue;
    }
    if (delimited &&
        (is_space(input[0]) ^ is_space(input[i]))) {
      return i;
    }
    if (input[i] == '(' || input[i] == ')') {
      return i;
    }
  }
  return i;
}

size_t unescape (const char *src, size_t src_len, char *dst) {
  size_t i, j;
  for (i = 0, j = 0; i < src_len; i++, j++) {
    if (src[i] == '\\') {
      i++;
    }
    dst[j] = src[i];
  }
  return j;
}

int main (int argc, char **argv) {
  int max_depth_tree = atoi(argv[1]);
  int max_depth_literal = atoi(argv[2]);
  char *input = argv[3];
  if (argc != 4) {
    printf("Usage: %s <max_deptH_tree> <max_depth_literal> <input>\n",
           argv[0]);
    return -1;
  }
  ssize_t input_len = strlen(input);
  int level = 0, pos = 0;
  while (pos < input_len) {
    if (input[pos] == ')') {
      if (level > 0) {
        level--;
      } else {
        puts("Parentheses closed unexpectedly");
        return -1;
      }
      pos++;
      continue;
    }
    ssize_t token_len = read_token(input + pos, input_len - pos,
                                   level < max_depth_literal);
    if (token_len <= 0) {
      return -1;
    }
    size_t unescaped_len = token_len;
    if (input[pos] != '(') {
      unescaped_len = unescape(input + pos, token_len, input + pos);
    }
    printf("%d %ld %ld: ", level, token_len, unescaped_len);
    fwrite(input + pos, 1, unescaped_len, stdout);
    puts("");
    if (input[pos] == '(' && level < max_depth_tree) {
      level++;
      pos++;
    } else {
      pos += token_len;
    }
  }
}
