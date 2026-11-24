#include <stdio.h>
#include <stdlib.h>

/* deferenziare un puntatore significa accedere
   al valore o alla variabile memorizzata 
   all'indirizzo a cui il puntatore sta puntando (*) */

int main(int argc, char** argv) {
  int arr[3] = {2, 3, 4};
  int* p = &arr[1];
  int** dp = &p; /* dichiara un doppio puntatore
                    dp (un puntatore a un puntatore 
                    a intero) e gli assegna l'indirizzo
                    di memoria della variabile p */

  *(*dp) += 1; /* *dp = p, dunque
                  *(*dp) = *p = arr[1], dunque
                  il contenuto di arr[1] passa da 3 a 4,
                  l'array ora contiene {2, 4, 4} */
  p += 1; /* sposta il puntatore p in avanti di una 
             posizione (ossia l'ampiezza in byte di
             un int), p ora punta a arr[2] (che vale 4).
             Poichè dp punta a p, anche *dp riflette
             questo cambio di indirizzo */
  *(*dp) += 1; /* incrementa il contenuto di arr[2]
                  di 1, arr[2] passa da 4 a 5 e 
                  l'array ora contiene {2, 4, 5} */

  for(int i = 0; i < 3; i ++) {
    printf("%d ", arr[i]);
  }

  return EXIT_SUCCESS;
}
