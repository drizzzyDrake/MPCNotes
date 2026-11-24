#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  int arr[3] = {1, 2, 3};           /* in memoria stack:
                                       ------------------
                                       arr[2] 03 00 00 00
                                       arr[1] 02 00 00 00
                                       arr[0] 01 00 00 00
                                       ------------------
                                       Little-Endian */

  /* definizione di due puntatori con offset diversi: */
  int* int_ptr = &arr[0];           /* puntatore a intero che punta a arr[0] */
  char* char_ptr = (char*) int_ptr; /* puntatore convertito (cast) a char che punta al byte 01 */

  /* int_ptr avanza di N x sizeof(int) = N x 4 byte */
  printf("&arr[0]: %p\n", int_ptr);
  int_ptr += 1; /* 1 x 4 = 4 byte */
  printf("&arr[1]: %p\n", int_ptr);
  int_ptr += 2; /* 2 x 4 = 8 byte */
  printf("Out of Array Bounds: %p\n", int_ptr);

  /* char_ptr avanza di N x sizeof(char) = N x 1 byte */
  printf("&01 (arr[0]): %p\n", char_ptr);
  char_ptr += 1; /* 1 x 1 = 1 byte */
  printf("&00 (arr[0]): %p\n", char_ptr);
  char_ptr += 3; /* 3 x 1 = 3 byte */
  printf("&02 (arr[1]): %p\n", char_ptr);

  return EXIT_SUCCESS;
}
