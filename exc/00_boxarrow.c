#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  int x = 1;
  int arr[3] = {2, 3, 4};
  int* p = &arr[1]; /* dichiara un puntatore a intero 
                       di nome p. L'operatore & ricava 
                       l'indirizzo di memoria di arr[1]
                       e lo assegna a p */

  /* &x stampa l'indirizzo in memoria della variabile x */
  printf("&x: %p;  x: %d\n", &x, x);
  printf("&arr[0]: %p;  arr[0]: %d\n", &arr[0], arr[0]);
  printf("&arr[2]: %p;  arr[2]: %d\n", &arr[2], arr[2]);
  /* &p : indirizzo di memoria dove risiede la variabile puntatore
      p : indirizzo di memoria memorizzato dentro p (indirizzo di arr[1])
     *p : valore presente all'indirizzo puntato da p (numero 3) */
  printf("&p: %p;  p: %p;  *p: %d\n", &p, p, *p);

  return EXIT_SUCCESS;
}
