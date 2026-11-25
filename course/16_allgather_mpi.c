/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./16_allgather_mpi
 *
 * Utilizzeremo:
 *
 *   tutti i processi --> possiedono una porzione di un vettore
 *
 * L'obiettivo è mostrare come MPI_Allgather raccolga i dati provenienti
 * da tutti i processi e distribuisca automaticamente il risultato a
 * tutti i partecipanti.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Allgather è una comunicazione collettiva che raccoglie dati da tutti
 * i processi e rende il buffer completo disponibile su ogni processo.
 * 
 * Dal punto di vista logico equivale a:
 *
 *   MPI_Gather + MPI_Bcast
 *
 * Tuttavia MPI_Allgather viene implementata direttamente da MPI
 * utilizzando algoritmi ottimizzati.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /* Ogni processo possiede due elementi.
   */
  int local_values[2];

  local_values[0] = my_rank * 2;
  local_values[1] = my_rank * 2 + 1;

  printf("[Processo %d] Valori locali: %d %d\n", my_rank, local_values[0], local_values[1]);

  int values[8];

  /* MPI_Allgather raccoglie i dati di tutti i processi e
   * memorizza il risultato su ogni processo.
   *
   * Parametri:
   *
   * - local_values    : buffer di invio;
   * - 2               : elementi inviati;
   * - MPI_INT         : tipo del buffer di invio;
   * - values          : buffer di ricezione;
   * - 2               : elementi ricevuti da ogni processo;
   * - MPI_INT         : tipo del buffer di ricezione;
   * - MPI_COMM_WORLD  : communicator. 
   */
  MPI_Allgather(
    local_values,
    2,
    MPI_INT,
    values,
    2,
    MPI_INT,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Vettore completo: ", my_rank);

  for(int i = 0; i < 2 * comm_sz; i++) {
    printf("%d ", values[i]);
  }

  printf("\n");

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Allgather è una comunicazione collettiva.
 *
 * Tutti i processi inviano una porzione di dati.
 *
 * Al termine della comunicazione ogni processo possiede
 * il buffer completo.
 *
 * Dal punto di vista logico:
 *
 *   MPI_Allgather = MPI_Gather + MPI_Bcast
 *
 * A differenza di MPI_Gather, dove olo il root riceve il buffer completo,
 * con MPI_Allgather tutti i processi ricevono il buffer completo.
 * ---------------------------------------------------------------------- */
