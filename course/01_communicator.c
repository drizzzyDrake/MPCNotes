#include <stdio.h>
#include <mpi.h>

/* Nei sistemi a memoria distribuita, ogni processo esegue il 
 * proprio codice ed è indipendente. Per coordinare il lavoro è 
 * necessario assegnare un identificativo univoco a ciascuno. In MPI 
 * questo identificatore è un numero intero non negativo chiamato rank.
 */
int main(int argc, char* argv[]) {

  /* Communicator: gruppo/collezione di processi che possono 
   * scambiarsi messaggi tra loro. */
  int comm_sz, my_rank;

  /* Quando l'ambiente MPI viene inizializzato, viene creato automaticamente
   * un communicator predefinito che contiene tuutti i processi avviati dal 
   * programma. Questo comunicatore globale si chiama MPI_COMM_WORLD. */
  MPI_Init(&argc, &argv);

  /* Funzioni per interrogare il communicator: */
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); /* Restituisce nella variabile
                                            * comm_sz il numero totale di 
                                            * processi presenti nel communicator. */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /* Restituisce nella variabile
                                            * my_rank il rank (id) del processo
                                            * specifico che sta eseguendo 
                                            * quella chiamata.*/

  /* I processi vengono eseguiti in parallelo in modo asincrono. Non c'è 
   * alcuna sincronizzazione implicita tra i vari printf dei vari processi.
   * L'ordine con cui i messaggi arrivano sullo schermo dipende esclusivamente
   * da come lo scheduler del sistema operativo gestisce lo scheduling dei 
   * processi e dall'I/O. */
  printf("Hello from process %d out of %d\n", my_rank, comm_sz);

  MPI_Finalize();

  return 0;
}
