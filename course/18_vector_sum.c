/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./18_vector_sum
 *
 * Utilizzeremo:
 *
 *    rank 0 --> possiede i vettori completi
 *    tutti i processi --> calcolano una parte della somma
 *    rank 0 --> raccoglie il risultato finale
 *
 * L'obiettivo è mostrare come MPI_Scatter e MPI_Gather possano essere
 * utilizzati per parallelizzare un problema semplice:
 *
 *      C = A + B
 *
 * Ogni processo lavora solamente su una porzione dei vettori.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* La strategia utilizzata è:
 *
 * - Il processo 0 possiede A e B completi.
 * - MPI_Scatter distribuisce una porzione di A e B ad ogni processo.
 * - Ogni processo calcola localmente: local_C = local_A + local_B
 * - MPI_Gather raccoglie tutte le porzioni di C sul processo 0. 
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  int n = 8;

  int A[8];
  int B[8];
  int C[8];

  if(my_rank == 0) {

    for(int i = 0; i < n; i++) {

      A[i] = i;
      B[i] = i * 10;
    }

    printf("[Processo %d] Vettore A: ", my_rank);

    for(int i = 0; i < n; i++)
      printf("%d ", A[i]);

    printf("\n");

    printf("[Processo %d] Vettore B: ", my_rank);

    for(int i = 0; i < n; i++)
      printf("%d ", B[i]);

    printf("\n\n");
  }

  int local_size = n / comm_sz;

  int local_A[local_size];
  int local_B[local_size];
  int local_C[local_size];

  MPI_Scatter(
    A,
    local_size,
    MPI_INT,
    local_A,
    local_size,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  MPI_Scatter(
    B,
    local_size,
    MPI_INT,
    local_B,
    local_size,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  for(int i = 0; i < local_size; i++) {
    local_C[i] = local_A[i] + local_B[i];
  }

  printf("[Processo %d] Risultato locale: ", my_rank);

  for(int i = 0; i < local_size; i++) {
    printf("%d ", local_C[i]);
  }

  printf("\n");

  MPI_Gather(
    local_C,
    local_size,
    MPI_INT,
    C,
    local_size,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  if(my_rank == 0) {

    printf("\n[Processo %d] Risultato finale: ", my_rank);

    for(int i = 0; i < n; i++) {
      printf("%d ", C[i]);
    }

    printf("\n");
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Scatter e MPI_Gather permettono di distribuire e raccogliere
 * dati tra processi.
 *
 * In questo esempio:
 *
 *      Processo 0 --> Scatter --> calcolo locale --> Gather --> processo 0
 *
 * Vantaggi:
 *
 * - ogni processo lavora solo su una parte dei dati;
 * - il carico computazionale viene distribuito;
 * - MPI gestisce la comunicazione tra processi.
 *
 * Limite:
 *
 * - funziona direttamente solo quando la dimensione dei dati è divisibile
 *   per il numero di processi.
 *
 * Per gestire dimensioni non divisibili si utilizzano:
 *
 *      MPI_Scatterv
 *      MPI_Gatherv
 * ---------------------------------------------------------------------- */
