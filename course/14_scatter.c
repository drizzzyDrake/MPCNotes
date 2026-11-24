/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./14_scatter
 *
 * Utilizzeremo:
 *
 *    rank 0 --> possiede un vettore
 *    tutti i processi --> ricevono una porzione del vettore
 *
 * L'obiettivo è mostrare come MPI_Scatter distribuisca parti diverse
 * dello stesso buffer ai vari processi.
 *
 * A differenza di MPI_Bcast, ogni processo riceve dati differenti.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Scatter è una comunicazione collettiva che divide un buffer
 * presente sul processo root in più parti e ne invia una ad ogni
 * processo del communicator.
 *
 * Tutti i processi devono chiamare MPI_Scatter.
 *
 * Solo il processo root possiede inizialmente il buffer completo.
 *
 * Al termine della comunicazione ogni processo riceve la propria
 * porzione del buffer.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /* Il vettore completo esiste solo sul processo root. */
  int values[8];

  if(my_rank == 0) {

    for(int i = 0; i < 8; i++) {
      values[i] = i;
    }

    printf("[Processo %d] Vettore iniziale:\n", my_rank);

    for(int i = 0; i < 8; i++) {
      printf("%d ", values[i]);
    }

    printf("\n\n");
  }

  /* Ogni processo riceverà due elementi. */
  int local_values[2];

  /* Parametri:
   *
   * - values          : buffer di invio (valido solo sul root);
   * - 2               : elementi inviati ad ogni processo;
   * - MPI_INT         : tipo del buffer di invio;
   * - local_values    : buffer di ricezione;
   * - 2               : elementi ricevuti;
   * - MPI_INT         : tipo del buffer di ricezione;
   * - 0               : processo root;
   * - MPI_COMM_WORLD  : communicator. */
  MPI_Scatter(
    values,
    2,
    MPI_INT,
    local_values,
    2,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Ricevuti: %d %d\n", my_rank, local_values[0], local_values[1]);

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Scatter è una comunicazione collettiva.
 *
 * Tutti i processi del communicator partecipano alla chiamata.
 *
 * Il processo root possiede il buffer completo.
 *
 * MPI divide automaticamente il buffer in parti uguali e ne invia
 * una ad ogni processo.
 *
 * A differenza di MPI_Bcast, dove tutti ricevono gli stessi dati,
 * con MPI_Scatter ogni processo riceve dati differenti.
 *
 * Se il numero di elementi da distribuire non è uguale per tutti
 * i processi si utilizza la variante: MPI_Scatterv
 * ---------------------------------------------------------------------- */
