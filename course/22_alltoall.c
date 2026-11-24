/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./22_alltoall
 *
 * Utilizzeremo:
 *
 *    tutti i processi --> inviano un dato a tutti gli altri processi
 *
 * L'obiettivo è mostrare come MPI_Alltoall permetta ad ogni processo di
 * inviare dati differenti a tutti gli altri processi in un'unica
 * comunicazione collettiva.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Alltoall è una comunicazione collettiva nella quale:
 *
 * - ogni processo invia un blocco di dati a tutti gli altri;
 * - ogni processo riceve un blocco di dati da tutti gli altri.
 *
 * Ogni processo possiede un buffer di invio ed uno di ricezione.
 *
 * MPI provvede automaticamente a distribuire tutti i dati.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /* Ogni processo prepara un valore destinato ad ogni processo. */
  int send_values[4];

  for(int i = 0; i < comm_sz; i++) {
    send_values[i] = my_rank * 10 + i;
  }

  printf("[Processo %d] Buffer di invio: ", my_rank);

  for(int i = 0; i < comm_sz; i++) {
    printf("%d ", send_values[i]);
  }

  printf("\n");

  int recv_values[4];

  /* MPI_Alltoall invia un elemento ad ogni processo e riceve
   * un elemento da ogni processo.
   *
   * Parametri:
   *
   * - send_values     : buffer di invio;
   * - 1               : elementi inviati ad ogni processo;
   * - MPI_INT         : tipo dei dati inviati;
   * - recv_values     : buffer di ricezione;
   * - 1               : elementi ricevuti da ogni processo;
   * - MPI_INT         : tipo dei dati ricevuti;
   * - MPI_COMM_WORLD  : communicator.
   */
  MPI_Alltoall(
    send_values,
    1,
    MPI_INT,
    recv_values,
    1,
    MPI_INT,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Buffer ricevuto: ", my_rank);

  for(int i = 0; i < comm_sz; i++) {
    printf("%d ", recv_values[i]);
  }

  printf("\n");

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Alltoall è una comunicazione collettiva.
 *
 * Ogni processo invia dati a tutti gli altri processi.
 *
 * Contemporaneamente ogni processo riceve dati da tutti gli altri.
 *
 * A differenza di MPI_Bcast, dove un processo invia a tutti, e
 * a differenza di MPI_Allgather, dove tutti ricevono lo stesso buffer
 * completo, con MPI_Alltoall ogni processo riceve dati differenti
 * da ogni altro processo.
 *
 * Questa comunicazione è spesso utilizzata nella redistribuzione
 * di dati tra processi e nelle trasposizioni di matrici distribuite.
 * ---------------------------------------------------------------------- */
