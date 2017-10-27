#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define dump_assert(x)                    \
  do {                                    \
    if (!(x)) {                           \
      print_tree_dotgraph("zzzzbug.dot"); \
      assert(x);                          \
    }                                     \
  } while (0);

struct Node {
  struct Node* parent;
  struct Node* left;
  struct Node* right;
  int value;
};

struct Node* find(int value);
void insert_do_vrcholu(struct Node* vrchol, int value);
void xsplay();

// Input buffer
char buf[256];

uint64_t find_count;
uint64_t find_sum;

struct Node* strom;
struct Node* koren;

int strom_size;
int vyuziti_stromu;

bool naive_splay = false;

int nulls = 0;
void text_print(struct Node* node);
void text_print_rec(struct Node* node);

void text_print_rec(struct Node* node) {
  if (node->left) {
    printf("(");
    text_print_rec(node->left);
    printf(")");
  }
  printf(",%d,", node->value);
  if (node->right) {
    printf("(");
    text_print_rec(node->right);
    printf(")");
  }
}


void text_print(struct Node* node) {
  text_print_rec(node);
  printf("\n");
}

void print_tree_node(FILE* f, struct Node* v) {
  fprintf(f, "\"%d\" [fillcolor = pink, style = filled]\n", v->value);

  if (v->left) {
    fprintf(f, "\"%d\" -> \"%d\"\n", v->value, v->left->value);

    print_tree_node(f, v->left);
  } else {
    fprintf(f, "\"%d\" -> \"NULL%d\"\n", v->value, nulls++);
  }

  if (v->right) {
    fprintf(f, "\"%d\" -> \"%d\"\n", v->value, v->right->value);

    print_tree_node(f, v->right);
  } else {
    fprintf(f, "\"%d\" -> \"NULL%d\"\n", v->value, nulls++);
  }
}

void print_tree_dotgraph(const char* fname) {
  return;

  nulls = 0;
  FILE* f = fopen(fname, "w");

  fprintf(f, "digraph G {\n");
  fprintf(f, "graph [ordering=\"out\"];\n");

  if (koren->parent == NULL) {
    fprintf(f, "NEKOREN [style=filled, fillcolor=lightblue]\n");
    fprintf(f, "NEKOREN -> \"%d\"\n", koren->value);
  } else {
    fprintf(f, "\"%d FUJKOREN\" [style=filled, fillcolor=red]\n", koren->parent->value);
    fprintf(f, "\"%d FUJKOREN\" -> \"%d\"\n", koren->parent->value, koren->value);
  }

  print_tree_node(f, koren);

  fprintf(f, "}\n");
}

void replace_child(struct Node* parent, struct Node* old, struct Node* newer) {
  if (parent) {
    if (parent->left == old) {
      parent->left = newer;
    } else if (parent->right == old) {
      parent->right = newer;
    } else {
      dump_assert(false);
    }
  }
  old->parent = NULL;
  newer->parent = parent;
}


#define SET_L(a, b) { \
  (a)->left = (b);    \
                      \
  if (b) {            \
    (b)->parent = (a);\
  }                   \
}

#define SET_R(a, b) { \
  (a)->right = (b);    \
                      \
  if (b) {            \
    (b)->parent = (a);\
  }                   \
}

//    a       //      c
//   / \      //     / \
//  b   c     //     a e
//     / \    //    /\
//     d e    //    b d
struct Node* rotace_prava(struct Node* a) {
  struct Node *c, *d;

  c = a->right;
  d = c->left;

  replace_child(a->parent, a, c);

  SET_L(c, a); SET_R(a, d);

  return c;
}

//    a     //      b
//   / \    //     / \
//  b   c   //     d  a
// / \      //       / \
// d e      //       e  c
struct Node* rotace_leva(struct Node* a) {
  struct Node* b, *e;

  b = a->left;
  e = b->right;

  replace_child(a->parent, a, b);

  SET_R(b, a);
  SET_L(a, e);

  return b;
}

//     a     //     d
//    / \    //    / \
//   b   c   //    f  b
//  / \      //      / \
//  d  e     //      g  a
// / \       //        / \
// f g       //        e  c
struct Node* rotace_leva_leva(struct Node* a) {
  struct Node *b, *d, *e, *g;

  b = a->left; d = b->left;
  e = b->right; g = d->right;

  replace_child(a->parent, a, d);

  SET_R(d, b);
  SET_L(b, g); SET_R(b, a);
  SET_L(a, e);

  return d;
}

//     a           //     e
//    / \          //    / \
//    b  c         //   c   g
//      / \        //  / \
//      d  e       //  a  f
//        / \      // / \
//        f  g     // b d
struct Node* rotace_prava_prava(struct Node* a) {
  struct Node *c, *d, *e, *f;

  c = a->right; d = c->left;
  e = c->right; f = e->left;

  replace_child(a->parent, a, e);

  SET_L(e, c);
  SET_L(c, a); SET_R(c, f);
  SET_R(a, d);

  return e;
}

//     a        //     d
//    / \       //    / \
//    b  c      //   a    c
//      / \     //  / \  / \
//      d  e    //  b  f g  e
//     / \
//     f  g
struct Node* rotace_prava_leva(struct Node* a) {
  struct Node *c, *d, *f, *g;

  c = a->right; d = c->left;
  f = d->left; g = d->right;

  replace_child(a->parent, a, d);

  SET_L(d, a); SET_R(d, c);
  SET_R(a, f);
  SET_L(c, g);

  return d;
}

//     a       //     e
//    / \      //    / \
//    b  c     //   b    a
//   / \       //  / \  / \
//   d  e      //  d  f g  c
//     / \
//     f  g
struct Node* rotace_leva_prava(struct Node* a) {
  struct Node *b, *e, *f, *g;

  b = a->left; e = b->right;
  f = e->left; g = e->right;

  replace_child(a->parent, a, e);

  SET_L(e, b); SET_R(e, a);
  SET_R(b, f);
  SET_L(a, g);

  return e;
}

int splay_count = 0;

void splay(struct Node* n) {
  if (!naive_splay) {
    while (n->parent && n->parent->parent) {
      struct Node *parent = n->parent,
                  *grandparent = n->parent->parent;
      // n is on the left
      if (parent->value > n->value) {
        if (grandparent->value > parent->value) {
          n = rotace_leva_leva(grandparent);
        } else if (grandparent->value < parent->value) {
          n = rotace_prava_leva(grandparent);
        } else {
          dump_assert(false);
        }
      } else if (parent->value < n->value) {
        if (grandparent->value > parent->value) {
          n = rotace_leva_prava(grandparent);
        } else if (grandparent->value < parent->value) {
          n = rotace_prava_prava(grandparent);
        } else {
          dump_assert(false);
        }
      } else {
        dump_assert(false);
      }
    }
  }

  while(n->parent) {
    struct Node *parent = n->parent;

    if (parent->value > n->value) {
      n = rotace_leva(parent);
    } else {
      n = rotace_prava(parent);
    }
  }

  koren = n;
}

void splay_from(struct Node* vrchol) {
  /* printf("splay %d\n", splay_count); */
  splay_count++;
  char fname_before[255];
  char fname_after[255];

  sprintf(fname_before, "%d_before.dot", splay_count);
  sprintf(fname_after, "%d_zafter.dot", splay_count);
  print_tree_dotgraph(fname_before);

  splay(vrchol);
  /* struct Node *parent, *grandparent, *new_parent; */

  /* assert(vrchol); */
  /*  */
  /* while (vrchol->parent) { */
  /*   printf("iter\n"); */
  /*   parent = vrchol->parent; */
  /*  */
  /*   if (!parent->parent) { */
  /*     if (parent->left == vrchol) { */
  /*       new_parent = rotace_leva(parent); */
  /*     } else if (parent->right == vrchol) { */
  /*       new_parent = rotace_prava(parent); */
  /*     } else { */
  /*       assert(false); */
  /*     } */
  /*  */
  /*     koren = new_parent; */
  /*  */
  /*     goto done; */
  /*   } else { */
  /*     grandparent = parent->parent; */
  /*  */
  /*     bool is_gp_root = grandparent == koren; */
  /*  */
  /*     if (grandparent->left == parent) { */
  /*       if (parent->left == vrchol) { */
  /*         printf("ll ***\n"); */
  /*         new_parent = rotace_leva_leva(grandparent); */
  /*       } else if (parent->right == vrchol) { */
  /*         new_parent = rotace_leva_prava(grandparent); */
  /*       } else { */
  /*         dump_assert(false); */
  /*       } */
  /*     } else if (grandparent->right == parent) { */
  /*       if (parent->left == vrchol) { */
  /*         new_parent = rotace_prava_leva(grandparent); */
  /*       } else if (parent->right == vrchol) { */
  /*         new_parent = rotace_prava_prava(grandparent); */
  /*       } else { */
  /*         assert(false); */
  /*       } */
  /*     } else { */
  /*       assert(false); */
  /*     } */
  /*  */
  /*     if (is_gp_root) { */
  /*       koren = new_parent; */
  /*     } else { */
  /*       printf("neprekorenovavam*****\n"); */
  /*       #<{(| struct Node* grandgrandparent = grandparent->parent; |)}># */
  /*       #<{(|  |)}># */
  /*       #<{(| replace_child(grandgrandparent, grandparent, new_parent); |)}># */
  /*     } */
  /*  */
  /*     assert(new_parent == vrchol); */
  /*   } */
  /*  */
  /*   vrchol = new_parent; */
  /*   new_parent = NULL; */
  /*   parent = NULL; */
  /*   grandparent = NULL; */
  /* } */

done:
  print_tree_dotgraph(fname_after);
}

void insert_do_vrcholu(struct Node* vrchol, int value) {
  /* char fname[255]; */
  /*  */
  /* sprintf(fname, "%d_aainsert.dot", splay_count); */
  /* print_tree_dotgraph(fname); */

  assert(vrchol);

  while (vrchol) {
    struct Node* kam;

    if (vrchol->value == value) {
      /* find(value); */
      return;
    }

    if (vrchol->value > value) {
      kam = vrchol->left;
    } else if (vrchol->value < value){
      kam = vrchol->right;
    } else {
      assert(false);
    }

    if (kam) {
      vrchol = kam;
    } else {
      struct Node* novy_vrchol = strom + vyuziti_stromu;

      if (vrchol->value > value) {
        vrchol->left = novy_vrchol;
      } else if (vrchol->value < value) {
        vrchol->right = novy_vrchol;
      } else {
        assert(false);
      }

      novy_vrchol->parent = vrchol;
      novy_vrchol->value = value;

      /* stack_push(novy_vrchol); */
      splay_from(novy_vrchol);

      vyuziti_stromu++;
      break;
    }
  }
}

void insert(int num) {
  if (koren) {
    insert_do_vrcholu(koren, num);
  } else {
    koren = strom;
    koren->value = num;

    vyuziti_stromu++;
  }
}

struct Node* find(int value) {
  struct Node* vrchol = koren;

  struct Node* found = NULL;

  uint64_t depth = 0;

  do {
    depth++;
    if (!vrchol) break;

    if (vrchol->value == value) {
      found = vrchol;

      splay_from(found);
      break;
    }

    if (vrchol->value > value) {
      vrchol = vrchol->left;
    } else if (vrchol->value < value) {
      vrchol = vrchol->right;
    } else {
      assert(false);
    }
  } while (vrchol);

  /* printf("\t\t\tfind %llu depth\n", depth); */
  find_sum += depth;
  find_count++;

  return found;
}

void test();

int main(int argc, char** argv) {
  if (argc > 1) {
    fprintf(stderr, "Running naive splay.\n");
    naive_splay = true;
  } else {
    fprintf(stderr, "Running splay with double rotations.\n");
  }
  /* test(); return 0; */


  clock_t time_before = clock();
  clock_t last_time = time_before;

  while (gets(buf)) {
    int size;

    sscanf(buf + 2, "%d", &size);

    /* printf("\n%s\n", buf); */

    switch (buf[0]) {
      case '#':
        find_sum = 0;
        find_count = 0;

        strom = (struct Node*)calloc(sizeof(struct Node), size);
        strom_size = size;
        koren = NULL;
        vyuziti_stromu = 0;

        clock_t now = clock();
        last_time = now;

        if (find_count > 0) {
          float mean_depth = (float)find_sum / find_count;

#ifdef VERBOSE_PRINT
          clock_t running_diff = clock() - last_time;

          printf("%.2f mean find depth, size: %d", mean_depth, size);
          printf("\ttest time: %.3fs\n", (float)running_diff / CLOCKS_PER_SEC);
#else
          printf("%.2f\n", mean_depth);
#endif
        }

        break;

      case 'I':
        insert(size);

        break;

      case 'F':
        find(size);

        break;
    }
  }

#ifdef VERBOSE_PRINT
  clock_t time_diff = clock() - time_before;
  printf("\n\nTotal time: %.3fs\n", (float)time_diff / CLOCKS_PER_SEC);
#endif
}

#define ASSIGN_L(a, b) { a->left = b; b->parent = a; }
#define ASSIGN_R(a, b) { a->right = b; b->parent = a; }

#define TEST_START() { \
  nn = malloc(sizeof(struct Node) * 8); \
                           \
  a = &nn[0],              \
  b = &nn[1],              \
  c = &nn[2],              \
  d = &nn[3],              \
  e = &nn[4],              \
  f = &nn[5],              \
  g = &nn[6],              \
  x = &nn[7];              \
                           \
  ASSIGN_L(x, a);          \
                           \
  ASSIGN_L(a, b);          \
  ASSIGN_R(a, c);          \
                           \
  ASSIGN_L(b, d);          \
  ASSIGN_R(b, e);          \
                           \
  ASSIGN_L(c, f);          \
  ASSIGN_R(c, g);          \
}

#define TEST_END() { free(nn); }

#define AL(a, b) assert((a)->left == (b) && (a)->left->value == (b)->value);
#define AR(a, b) assert((a)->right == (b) && (a)->right->value == (b)->value);

void test() {

  //      x
  //      |
  //      a
  //     /  \
  //    b    c
  //   /\    /\
  //  d  e  f  g
  //
  struct Node *x, *a, *b, *c, *d, *e, *f, *g;

  struct Node *nn;
  {
    TEST_START();

    AL(x, a);

    AL(a, b); AR(a, c);
    AL(b, d); AR(b, e);
    AL(c, f); AR(c, g);

    TEST_END();
  }
}
