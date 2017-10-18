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
  struct Node* left;
  struct Node* right;
  int value;
};

struct Node* find(int value);
void insert_do_vrcholu(struct Node* vrchol, int value);
int jdem_doleva(int vrchol, int value);
void xsplay();

int stack_capacity = 0;
int stack_size = 0;
struct Node** stack = NULL;

// Input buffer
char buf[256];

struct Node* strom;
struct Node* koren;

int strom_size;
int vyuziti_stromu;

void stack_init(int max_size) {
  assert(max_size > 0);

  if (stack_capacity < max_size) {
    stack = realloc(stack, max_size);
  }

  assert(stack);

  memset(stack, 0, sizeof(struct Node*) * stack_size);

  stack_capacity = max_size;
  stack_size = 0;
}

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

void stack_clear() {
  assert(stack_capacity > 0);
  stack_size = 0;
}

void stack_push(struct Node* node) {
  printf("s: %d, cap: %d\n", stack_size, stack_capacity);
  assert(stack_capacity - stack_size > 0);

  stack[stack_size++] = node;
}

struct Node* stack_pop() {
  assert(stack_size > 0);

  stack_size--;

  return stack[stack_size];
}

int stack_is_empty() { return stack_size == 0; }

// Returns a pointer to the top of the stack so that it can be easily walked.
struct Node* stack_top() {
  assert(stack_size > 0);
  return stack[stack_size - 1];
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

  c->left = a;
  c->right = e;

  a->left = b;
  a->right = d;

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

  b->left = d;
  b->right = a;

  a->left = e;
  a->right = c;

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

  d->left = f;
  d->right = b;

  b->left = g;
  b->right = a;

  a->left = e;
  a->right = c;

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

  e->left = c;
  e->right = g;

  c->left = a;
  c->right = f;

  a->left = b;
  a->right = d;

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

  d->left = a;
  d->right = c;

  a->left = b;
  a->right = b;

  c->left = g;
  c->right = e;

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

  e->left = b;
  e->right = b;

  b->left = d;
  b->right = f;

  a->left = g;
  a->right = c;

  return e;
}

int splay_count = 0;

void splay_bettur() {
  printf("splay %d\n", splay_count);
  splay_count++;
  char fname_before[255];
  char fname_after[255];

  sprintf(fname_before, "%d_before.dot", splay_count);
  sprintf(fname_after, "%d_zafter.dot", splay_count);
  print_tree_dotgraph(fname_before);
  if (stack_is_empty())
    goto done;

  struct Node* vrchol, *parent, *grandparent, *new_parent;

  vrchol = stack_pop();
  assert(vrchol);

  while (!stack_is_empty()) {
    printf("iter\n");
    parent = stack_pop();

    if (stack_is_empty()) {
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
      grandparent = stack_pop();

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

      if (stack_is_empty()) {
        koren = new_parent;
      } else {
        struct Node* grandgrandparent = stack_top();

        if (grandgrandparent->left == grandparent) {
          grandgrandparent->left = new_parent;
        } else {
          grandgrandparent->right = new_parent;
        }
      }

      assert(new_parent == vrchol);
    }

    stack_push(new_parent);

    new_parent = NULL;
  }

done:
  print_tree_dotgraph(fname_after);
}

void xsplay() {
  printf("splay %d\n", splay_count);
  splay_count++;
  char fname_before[255];
  char fname_after[255];

  sprintf(fname_before, "%d_before.dot", splay_count);
  sprintf(fname_after, "%d_zafter.dot", splay_count);
  print_tree_dotgraph(fname_before);

  struct Node* vrchol, *parent, *grandparent, *new_parent;

  if (stack_is_empty())
    goto finish;

  vrchol = stack_pop();
  assert(vrchol);

  while (!stack_is_empty()) {
    parent = stack_pop();

    /* dump_assert(parent->value != vrchol->value); */
    /* assert(parent != vrchol); */

    if (stack_is_empty()) {
      // Only simple splay
      bool left_child = parent->left == vrchol;

      if (left_child) {
        new_parent = rotace_leva(parent);
        assert(new_parent == vrchol);
      } else {
        new_parent = rotace_prava(parent);
        assert(new_parent == vrchol);
      }

      koren = new_parent;
      goto finish;
    } else {
      grandparent = stack_pop();

      bool left_child = parent->left == vrchol;
      bool left_parent_child = grandparent->left == parent;

      if (!left_child) assert(parent->right == vrchol);
      if (!left_parent_child) assert(grandparent->right == parent);

      if (left_parent_child && left_child) {
        new_parent = rotace_leva_leva(grandparent);
      } else if (left_parent_child && !left_child) {
        new_parent = rotace_leva_prava(grandparent);
      } else if (!left_parent_child && left_child) {
        new_parent = rotace_prava_leva(grandparent);
      } else if (!left_parent_child && !left_child) {
        new_parent = rotace_prava_prava(grandparent);
      }

      if (stack_is_empty()) {
        koren = new_parent;
      } else {
        struct Node* grandgrandparent = stack_top();

        if (grandgrandparent->left == grandparent) {
          grandgrandparent->left = new_parent;
        } else {
          grandgrandparent->right = new_parent;
        }
      }

      assert(new_parent == vrchol);
    }

    stack_push(new_parent);
  }

finish:
  print_tree_dotgraph(fname_after);
}

int jdem_doleva(int vrchol, int value) { return vrchol < value; }

void insert_do_vrcholu(struct Node* vrchol, int value) {
  assert(vrchol);

  stack_clear();

  while (vrchol) {
    struct Node* kam;

    stack_push(vrchol);

    if (vrchol->value == value) {
      find(value);
      return;
    }

    // TODO: ignorovat ==
    int doleva = jdem_doleva(vrchol->value, value);

    if (doleva) {
      kam = vrchol->left;
    } else {
      kam = vrchol->right;
    }

    if (kam) {
      vrchol = kam;
    } else {
      struct Node* novy_vrchol = strom + vyuziti_stromu;

      if (doleva) {
        vrchol->left = novy_vrchol;
      } else {
        vrchol->right = novy_vrchol;
      }

      novy_vrchol->value = value;

      stack_push(novy_vrchol);

      vyuziti_stromu++;
      break;
    }
  }

  splay_bettur();
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

  stack_clear();

  struct Node* found = NULL;

  do {
    if (!vrchol) break;

    stack_push(vrchol);

    if (vrchol->value == value) {
      found = vrchol;
      break;
    }

    int doleva = jdem_doleva(vrchol->value, value);

    if (doleva) {
      vrchol = vrchol->left;
    } else {
      vrchol = vrchol->right;
    }
  } while (vrchol);

  if (found) {
    splay_bettur();
  }

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

        stack_init(strom_size);

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
