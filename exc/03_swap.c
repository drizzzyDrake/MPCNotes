#include <stdio.h>
#include <stdlib.h>

void broken_swap(int a, int b) {
  int tmp = a;
  a = b;
  b = tmp;
}

void fixed_swap(int* a, int* b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

int main(int argc, char** argv) {
  int a = 42, b = -7;

  broken_swap(a, b); /* call-by-value */
  printf("a: %d, b: %d\n", a, b);

  fixed_swap(&a, &b); /* call-by-reference */
  printf("a: %d, b: %d", a, b);

  return EXIT_SUCCESS;
}
