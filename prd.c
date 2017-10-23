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

void print_tree_node(FILE* f, struct Node* v) {
  fprintf(f, "\"%d\" [fillcolor = pink, style = filled]\n", v->value);

  if (v->left) {
    fprintf(f, "\"%d\" -> \"%d\"\n", v->value, v->left->value);

    print_tree_node(f, v->left);
  }

  if (v->right) {
    fprintf(f, "\"%d\" -> \"%d\"", v->value, v->right->value);

    print_tree_node(f, v->right);
  }
}

void print_tree_dotgraph(const char* fname) {
  FILE* f = fopen(fname, "w");

  fprintf(f, "digraph G {\n");

  print_tree_node(f, koren);

  fprintf(f, "}\n");
}

void replace_child(struct Node* parent, struct Node* old, struct Node* new) {
  if (parent) {
    if (parent->left == old) {
      parent->left = new;
    } else if (parent->right == old) {
      parent->right = new;
    } else {
      assert(false);
    }
  }
}

//    a
//   / \
//  b   c
//     / \
//     d e
//
//      c
//     / \
//     a e
//    /\
//    b d
struct Node* rotace_prava(struct Node* a) {
  struct Node* b, *c, *d, *e;

  b = a->left;
  c = a->right;

  d = c->left;
  e = c->right;

  replace_child(a->parent, a, c);

  c->left = a;
  c->right = e;

  a->left = b;
  a->right = d;

  b->parent = a;
  d->parent = a;
  a->parent = c;
  e->parent = c;

  return c;
}

//    a
//   / \
//  b   c
// / \
// d e
//
//      b
//     / \
//     d  a
//       / \
//       e  c
struct Node* rotace_leva(struct Node* a) {
  struct Node* b, *c, *d, *e;

  b = a->left;
  c = a->right;

  d = b->left;
  e = b->right;

  replace_child(a->parent, a, b);

  b->left = d;
  b->right = a;

  a->left = e;
  a->right = c;

  if (d) d->parent = b;
  if (a) a->parent = b;
  if (e) e->parent = a;
  if (c) c->parent = a;

  return b;
}

//     a
//    / \
//   b   c
//  / \
//  d  e
// / \
// f g
//
//
//     d
//    / \
//    f  b
//      / \
//      g  a
//        / \
//        e  c
//
struct Node* rotace_leva_leva(struct Node* a) {
  struct Node* b, *c, *d, *e, *f, *g;

  b = a->left;
  c = a->right;

  d = b->left;
  e = b->right;

  f = d->left;
  g = d->right;

  replace_child(a->parent, a, d);

  d->left = f;
  d->right = b;

  b->left = g;
  b->right = a;

  a->left = e;
  a->right = c;

  if (f) f->parent = d;
  if (b) b->parent = d;
  if (g) g->parent = b;
  if (a) a->parent = b;
  if (e) e->parent = a;
  if (c) c->parent = a;

  return d;
}

//     a
//    / \
//    b  c
//      / \
//      d  e
//        / \
//        f  g
//
//     e
//    / \
//   c   g
//  / \
//  a  f
// / \
// b d
//
struct Node* rotace_prava_prava(struct Node* a) {
  struct Node* b, *c, *d, *e, *f, *g;

  b = a->left;
  c = a->right;

  d = c->left;
  e = c->right;

  f = e->left;
  g = e->right;

  replace_child(a->parent, a, e);

  e->left = c;
  e->right = g;

  c->left = a;
  c->right = f;

  a->left = b;
  a->right = d;

  if (c) c->parent = e;
  if (g) g->parent = e;
  if (a) a->parent = c;
  if (f) f->parent = c;
  if (b) b->parent = a;
  if (d) d->parent = a;

  return e;
}

//     a
//    / \
//    b  c
//      / \
//      d  e
//     / \
//     f  g
//
//     d
//    / \
//   a    c
//  / \  / \
//  b  f g  e
//
struct Node* rotace_prava_leva(struct Node* a) {
  struct Node* b, *c, *d, *e, *f, *g;

  b = a->left;
  c = a->right;

  d = c->left;
  e = c->right;

  f = d->left;
  g = d->right;

  replace_child(a->parent, a, d);

  d->left = a;
  d->right = c;

  a->left = b;
  a->right = b;

  c->left = g;
  c->right = e;

  if (a) a->parent = d;
  if (c) c->parent = d;
  if (b) b->parent = a;
  if (f) f->parent = a;
  if (g) g->parent = c;
  if (e) e->parent = c;

  return d;
}

//     a
//    / \
//    b  c
//   / \
//   d  e
//     / \
//     f  g
//
//     e
//    / \
//   b    a
//  / \  / \
//  d  f g  c
struct Node* rotace_leva_prava(struct Node* a) {
  struct Node* b, *c, *d, *e, *f, *g;

  b = a->left;
  c = a->right;

  d = b->left;
  e = b->right;

  f = e->left;
  g = e->right;

  replace_child(a->parent, a, e);

  e->left = b;
  e->right = b;

  b->left = d;
  b->right = f;

  a->left = g;
  a->right = c;

  if (b) b->parent = e;
  if (a) a->parent = e;
  if (d) d->parent = b;
  if (f) f->parent = b;
  if (g) g->parent = a;
  if (c) c->parent = a;

  return e;
}

int splay_count = 0;

void splay_from(struct Node* vrchol) {
  printf("splay %d\n", splay_count);
  splay_count++;
  char fname_before[255];
  char fname_after[255];

  sprintf(fname_before, "%d_before.dot", splay_count);
  sprintf(fname_after, "%d_zafter.dot", splay_count);
  print_tree_dotgraph(fname_before);

  struct Node *parent, *grandparent, *new_parent;

  assert(vrchol);

  while (vrchol->parent) {
    printf("iter\n");
    parent = vrchol->parent;

    if (!parent->parent) {
      if (parent->left == vrchol) {
        new_parent = rotace_leva(parent);
      } else if (parent->right == vrchol) {
        new_parent = rotace_prava(parent);
      } else {
        assert(false);
      }

      koren = new_parent;

      goto done;
    } else {
      grandparent = parent->parent;

      if (grandparent->left == parent) {
        if (parent->left == vrchol) {
          new_parent = rotace_leva_leva(grandparent);
        } else if (parent->right == vrchol) {
          new_parent = rotace_leva_prava(grandparent);
        } else {
          assert(false);
        }
      } else if (grandparent->right == parent) {
        if (parent->left == vrchol) {
          new_parent = rotace_prava_leva(grandparent);
        } else if (parent->right == vrchol) {
          new_parent = rotace_prava_prava(grandparent);
        } else {
          assert(false);
        }
      } else {
        assert(false);
      }

      if (!grandparent->parent) {
        koren = new_parent;
      } else {
        struct Node* grandgrandparent = grandparent->parent;

        replace_child(grandgrandparent, grandparent, new_parent);
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
  assert(vrchol);

  while (vrchol) {
    struct Node* kam;

    if (vrchol->value == value) {
      /* find(value); */
      return;
    }

    if (vrchol->value < value) {
      kam = vrchol->left;
    } else if (vrchol->value > value){
      kam = vrchol->right;
    } else {
      assert(false);
    }

    if (kam) {
      vrchol = kam;
    } else {
      struct Node* novy_vrchol = strom + vyuziti_stromu;

      if (vrchol->value < value) {
        vrchol->left = novy_vrchol;
      } else if (vrchol->value > value) {
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

    int doleva = jdem_doleva(vrchol->value, value);

    if (doleva) {
      vrchol = vrchol->left;
    } else {
      vrchol = vrchol->right;
    }
  } while (vrchol);

  return found;
}

int main() {
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
