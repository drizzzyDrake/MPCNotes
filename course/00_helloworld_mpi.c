#include <stdio.h>
/* Header principale della libreria MPI. */
#include <mpi.h> 

int main(int argc, char* argv[]) {
  /* Inizializza l'ambiente di esecuzione MPI. Deve essere chiamata 
   * prima di qualsiasi altra funzione MPI. */
  MPI_Init(&argc, &argv);  
  /* Stampa la stringa "hello, world" a schermo seguita da un a capo. 
   * Poiché il modello di esecuzione MPI è di tipo SPMD (Single-Program 
   * Multiple-Data), questo codice viene eseguito contemporaneamente da 
   * tutti i processi avviati. Se il programma viene lanciato con N 
   * processi, la frase verrà stampata N volte. */
  printf("hello, world\n");
  /* Termina l'ambiente MPI e libera tutte le risorse allocate dalla
   * libreria per questo programma. Dopo questa chiamata non è più 
   * possibile eseguire alcuna funzione MPI. */
  MPI_Finalize();
  return 0;
}
