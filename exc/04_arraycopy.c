#include <stdio.h>
#include <stdlib.h>

/* la funzione copy prende in input l'array a[]
 * e la sua dimensione. Restituisce in output 
 * l'indirizzo di memoria del nuovo array duplicato
 * (un puntatore a intero) */
int* copy(int a[], int size) {
  int i, *a2;

  a2 = (int*)malloc(size * sizeof(int)); /* allocazione dinamica di memoria
                                            nello Heap con malloc():
                                            - (int*) esegue il cast del valore
                                              restituito da malloc, che di 
                                              defualt e void*
                                            - size * sizeof(int) calcola lo 
                                              spazio necessario in base alla 
                                              lunghezza dell'array (size) x 
                                              4 byte (sizeof(int)) */
  if (a2 == NULL) /* se la RAM è esaurita la malloc fallisce e restituice NULL */
    return NULL;

  for (i = 0; i < size; i++) /* copia dei valori dell'array */
    a2[i] = a[i];

  return a2;
}

int main(int argc, char** argv) {
  int nums[4] = {1, 2, 3, 4};

  int* nums_copy = copy(nums, 4);
  
  printf("&nums: %p\t&nums_copy: %p\n", &nums, &nums_copy);
  for (int i = 0; i < 4; i++) {
    printf("%d\t\t\t%d\n", nums[i], nums_copy[i]);
  }

  free(nums_copy); /* dealloca e libera la memoria dell'Heap riservata in 
  precedenza tramite malloc. Evita i memory leak */

  printf("&nums: %p\t&nums_copy: %p\n", &nums, &nums_copy);
  for (int i = 0; i < 4; i++) {
    printf("%d\t\t\t%d\n", nums[i], nums_copy[i]);
  }

  return EXIT_SUCCESS;
}
