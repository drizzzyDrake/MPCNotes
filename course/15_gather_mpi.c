/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./15_gather_mpi
 *
 * Utilizzeremo:
 *
 *   tutti i processi --> possiedono una porzione di un vettore
 *   rank 0           --> raccoglie tutte le porzioni
 *
 * L'obiettivo è mostrare come MPI_Gather raccolga dati provenienti da
 * tutti i processi in un unico buffer sul processo root.
 *
 * MPI_Gather rappresenta l'operazione inversa di MPI_Scatter.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Gather è una comunicazione collettiva che raccoglie dati da tutti
 * i processi del communicator e li memorizza in un unico buffer sul
 * processo root.
 *
 * Tutti i processi devono chiamare MPI_Gather.
 *
 * Ogni processo contribuisce con la stessa quantità di dati.
 *
 * Solamente il processo root riceve il vettore completo. 
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

  /* Il buffer completo esiste solo sul processo root. 
   */
  int values[8];

  /* Parametri:
   *
   * - local_values    : buffer di invio;
   * - 2               : elementi inviati;
   * - MPI_INT         : tipo del buffer di invio;
   * - values          : buffer di ricezione (valido solo sul root);
   * - 2               : elementi ricevuti da ogni processo;
   * - MPI_INT         : tipo del buffer di ricezione;
   * - 0               : processo root;
   * - MPI_COMM_WORLD  : communicator. 
   */
  MPI_Gather(
    local_values,
    2,
    MPI_INT,
    values,
    2,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  if(my_rank == 0) {

    printf("\n[Processo %d] Vettore raccolto:\n", my_rank);

    for(int i = 0; i < 2 * comm_sz; i++) {
      printf("%d ", values[i]);
    }

    printf("\n");
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Gather è una comunicazione collettiva.
 *
 * Tutti i processi del communicator partecipano alla chiamata.
 *
 * Ogni processo invia la stessa quantità di dati.
 *
 * Il processo root riceve tutte le porzioni e ricostruisce il
 * buffer completo.
 *
 * A differenza di MPI_Scatter, dove un buffer viene diviso tra tutti
 * i processi, con MPI_Gather tutti i processi ricompongono un unico
 * buffer sul root.
 *
 * Se ogni processo deve inviare una quantità diversa di dati
 * si utilizza: MPI_Gatherv
 * ---------------------------------------------------------------------- */
