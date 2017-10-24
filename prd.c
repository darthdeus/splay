#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

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

struct Node* strom;
struct Node* koren;

int strom_size;
int vyuziti_stromu;

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
  printf("printim\n");
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
  } else {
    newer->parent = NULL;
  }
}


//     a          b
//    /\         /\
//   b  c   ->  d  a
//  /\            /\
// d  e          e  c
struct Node *
l_rotate(struct Node *n) {
    struct Node *a = n;
    struct Node *b = n->left;

    a->left = b->right;
    if (a->left)
        a->left->parent = a;

    b->parent = a->parent;

    b->right = a;
    b->right->parent = b;

    if (b->parent) {
        if (b->parent->left == a)
            b->parent->left = b;
        else
            b->parent->right = b;
    }

    return b;
}

//     a          c
//    /\         /\
//   b  c   ->  a  e
//     /\      /\
//    d  e    b  d
struct Node *
r_rotate(struct Node *n) {
    struct Node *a = n;
    struct Node *c = n->right;

    a->right = c->left;
    if (a->right)
        a->right->parent = a;

    c->parent = a->parent;

    c->left = a;
    c->left->parent = c;

    if (c->parent) {
        if (c->parent->left == a)
            c->parent->left = c;
        else
            c->parent->right = c;
    }

    return c;
}

//       a            e
//      / \          / \
//     b  c   ->    c   i
//       / \       / \
//      d  e      a  h
//        / \    / \
//        h i    b d
struct Node *
rr_rotate(struct Node *n) {
    struct Node *a = n;
    struct Node *c = n->right;
    struct Node *e = n->right->right;

    a->right = c->left;
    if (a->right)
        a->right->parent = a;

    c->right = e->left;
    if (c->right)
        c->right->parent = c;

    e->parent = a->parent;

    c->left = a;
    c->left->parent = c;

    e->left = c;
    e->left->parent = e;

    if (e->parent) {
        if (e->parent->left == a)
            e->parent->left = e;
        else
            e->parent->right = e;
    }

    return e;
}

//       a            d
//      / \          / \
//     b   c   ->   h   b
//    / \              / \
//   d   e            i   a
//  / \                  / \
// h   i                e   c
struct Node *
ll_rotate(struct Node *n) {
    struct Node *a = n;
    struct Node *b = n->left;
    struct Node *d = n->left->left;

    a->left = b->right;
    if (a->left)
        a->left->parent = a;

    b->left = d->right;
    if (b->left)
        b->left->parent = b;

    d->parent = a->parent;

    b->right = a;
    b->right->parent = b;

    d->right = b;
    d->right->parent = d;

    if (d->parent) {
        if (d->parent->left == a)
            d->parent->left = d;
        else
            d->parent->right = d;
    }

    return d;
}

//      a             e
//    /  \          /   \
//   b    c   ->   b     a
//  / \           / \   / \
// d   e         d   h i   c
//    / \
//   h   i
struct Node *
lr_rotate(struct Node *n) {
    struct Node *a = n;
    struct Node *b = n->left;
    struct Node *e = n->left->right;

    a->left = e->right;
    if (a->left)
        a->left->parent = a;

    b->right = e->left;
    if (b->right)
        b->right->parent = b;

    e->parent = a->parent;

    e->left = b;
    e->left->parent = e;

    e->right = a;
    e->right->parent = e;

    if (e->parent) {
        if (e->parent->left == a)
            e->parent->left = e;
        else
            e->parent->right = e;
    }

    return e;
}

//      a            d
//     / \          / \
//   b    c   ->  a    c
//       / \     / \  / \
//      d  e    b  h i   e
//     / \
//    h   i
struct Node *
rl_rotate(struct Node *n) {
    struct Node *a = n;
    struct Node *c = n->right;
    struct Node *d = n->right->left;

    a->right = d->left;
    if (a->right)
        a->right->parent = a;

    c->left = d->right;
    if (c->left)
        c->left->parent = c;

    d->parent = a->parent;

    d->right = c;
    d->right->parent = d;

    d->left = a;
    d->left->parent = d;

    if (d->parent) {
        if (d->parent->left == a)
            d->parent->left = d;
        else
            d->parent->right = d;
    }

    return d;
}


int splay_count = 0;

void splay(struct Node* n) {
  while (n->parent && n->parent->parent) {
    struct Node *parent = n->parent,
                *grandparent = n->parent->parent;
    // n is on the left
    if (parent->value > n->value) {
      if (grandparent->value > parent->value) {
        n = ll_rotate(grandparent);
      } else if (grandparent->value < parent->value) {
        n = rl_rotate(grandparent);
      } else {
        dump_assert(false);
      }
    } else if (parent->value < n->value) {
      if (grandparent->value > parent->value) {
        n = lr_rotate(grandparent);
      } else if (grandparent->value < parent->value) {
        n = rr_rotate(grandparent);
      } else {
        dump_assert(false);
      }
    } else {
      dump_assert(false);
    }
  }

  while(n->parent) {
    struct Node *parent = n->parent;

    if (parent->value > n->value) {
      n = l_rotate(parent);
    } else {
      n = r_rotate(parent);
    }
  }

  koren = n;
}

void splay_from(struct Node* vrchol) {
  printf("splay %d\n", splay_count);
  splay_count++;
  char fname_before[255];
  char fname_after[255];

  sprintf(fname_before, "%d_before.dot", splay_count);
  sprintf(fname_after, "%d_zafter.dot", splay_count);
  print_tree_dotgraph(fname_before);

  /* splay(vrchol); */
  struct Node *parent, *grandparent, *new_parent;

  assert(vrchol);

  while (vrchol->parent) {
    printf("iter\n");
    parent = vrchol->parent;

    if (!parent->parent) {
      if (parent->left == vrchol) {
        new_parent = l_rotate(parent);
      } else if (parent->right == vrchol) {
        new_parent = r_rotate(parent);
      } else {
        assert(false);
      }

      koren = new_parent;

      goto done;
    } else {
      grandparent = parent->parent;

      bool is_gp_root = grandparent == koren;

      if (grandparent->left == parent) {
        if (parent->left == vrchol) {
          printf("ll ***\n");
          new_parent = ll_rotate(grandparent);
        } else if (parent->right == vrchol) {
          new_parent = lr_rotate(grandparent);
        } else {
          dump_assert(false);
        }
      } else if (grandparent->right == parent) {
        if (parent->left == vrchol) {
          new_parent = rl_rotate(grandparent);
        } else if (parent->right == vrchol) {
          new_parent = rr_rotate(grandparent);
        } else {
          assert(false);
        }
      } else {
        assert(false);
      }

      if (is_gp_root) {
        koren = new_parent;
      } else {
        printf("neprekorenovavam*****\n");
        /* struct Node* grandgrandparent = grandparent->parent; */
        /*  */
        /* replace_child(grandgrandparent, grandparent, new_parent); */
      }

      assert(new_parent == vrchol);
    }

    vrchol = new_parent;
    new_parent = NULL;
    parent = NULL;
    grandparent = NULL;
  }

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
  printf("insertim %d\n", num);
  if (koren) {
    printf("mam strom\n");

    insert_do_vrcholu(koren, num);
  } else {
    printf("NEmam strom\n");

    koren = strom;
    koren->value = num;

    vyuziti_stromu++;
  }
}

struct Node* find(int value) {
  /* printf("finduju\n"); */
  struct Node* vrchol = koren;

  struct Node* found = NULL;

  do {
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

  return found;
}

void test();

int main() {
  /* test(); return 0; */

  while (gets(buf)) {
    int size;

    sscanf(buf + 2, "%d", &size);

    printf("\n%s\n", buf);

    switch (buf[0]) {
      case '#':
        strom = (struct Node*)calloc(sizeof(struct Node), size);
        strom_size = size;
        koren = NULL;
        vyuziti_stromu = 0;

        printf("Vytvarim strom %d\n", size);
        break;

      case 'I':
        insert(size);

        break;

      case 'F':
        find(size);

        break;
    }
  }
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
