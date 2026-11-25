/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./21_reduce_scatter_mpi
 *
 * Utilizzeremo:
 *
 *   tutti i processi --> possiedono un vettore locale
 *
 * L'obiettivo è mostrare come MPI_Reduce_scatter combini una riduzione
 * e una distribuzione dei risultati in un'unica comunicazione collettiva.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Reduce_scatter è una comunicazione collettiva che combina due
 * operazioni:
 *
 * - riduce i dati provenienti da tutti i processi;
 * - distribuisce automaticamente parti del risultato finale.
 *
 * Dal punto di vista logico equivale a:
 *
 *   MPI_Reduce + MPI_Scatter
 *
 * Tuttavia MPI esegue entrambe le operazioni con un'unica comunicazione.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /* Ogni processo possiede un vettore di 4 elementi.
   */
  int local_values[4];

  for(int i = 0; i < 4; i++) {
    local_values[i] = (my_rank + 1) * (i + 1);
  } 

  printf("[Processo %d] Valori locali: ", my_rank);

  for(int i = 0; i < 4; i++) {
    printf("%d ", local_values[i]);
  }

  printf("\n");

  /* Ogni processo riceverà un elemento del risultato finale.
   */
  int recvcounts[4] = {1, 1, 1, 1};

  int result;

  MPI_Reduce_scatter(
    local_values,
    &result,
    recvcounts,
    MPI_INT,
    MPI_SUM,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Risultato ricevuto: %d\n", my_rank, result);

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Reduce_scatter è una comunicazione collettiva.
 *
 * Tutti i processi contribuiscono alla riduzione.
 *
 * Il risultato finale non viene raccolto su un solo processo.
 *
 * MPI divide automaticamente il risultato tra i processi.
 *
 * Dal punto di vista logico:
 *
 *   MPI_Reduce_scatter  =  MPI_Reduce + MPI_Scatter
 *
 * Questa operazione è molto utilizzata nelle librerie di Deep Learning
 * distribuito per sincronizzare gradienti e pesi in modo efficiente.
 * ---------------------------------------------------------------------- */
