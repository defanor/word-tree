enum tree_type {
  parenthesized,
  escaped_run,
  delimiter_run,
};

struct forest {
  struct tree **trees;
  int tree_num;
};

struct raw {
  char *data;
  int len;
};

struct tree
{
  enum tree_type type;
  union {
    struct forest *children;
    struct raw literal;
  };
};

struct tree *tree_alloc(enum tree_type type, char *raw, size_t len);
struct forest *forest_alloc();
void plant_tree(struct forest *forest, struct tree *tree);

void print_tree (struct tree *tree, int debug);
void print_forest (struct forest *forest, int debug);
