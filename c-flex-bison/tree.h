enum tree_type {
  parenthesized,
  escaped_run,
  delimiter_run
};

struct tree
{
  enum tree_type type;
  struct tree *children;
  struct tree *next;
  struct tree *last;
  char *raw;
  size_t raw_len;
};

struct tree *tree_alloc(enum tree_type type, char *raw, size_t len);
