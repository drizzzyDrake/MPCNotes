/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./20_scatterv_gatherv
 *
 * Utilizzeremo:
 *
 *    rank 0 --> possiede il vettore completo
 *    tutti i processi --> ricevono una quantità diversa di dati
 *
 * L'obiettivo è mostrare come MPI_Scatterv e MPI_Gatherv permettano
 * di gestire distribuzioni non uniformi dei dati.
 *
 * A differenza di MPI_Scatter e MPI_Gather, il numero di elementi
 * inviati/ricevuti può cambiare per ogni processo.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  int n = 10;

  int values[10];
  int result[10];

  int sendcounts[4];
  int displacements[4];

  /* Il processo root inizializza il vettore. */
  if(my_rank == 0) {

    for(int i = 0; i < n; i++) {
      values[i] = i;
    }

    printf("[Processo 0] Vettore iniziale: ");

    for(int i = 0; i < n; i++) {
      printf("%d ", values[i]);
    }

    printf("\n\n");

    /* Numero di elementi assegnati ad ogni processo. */
    sendcounts[0] = 3;
    sendcounts[1] = 3;
    sendcounts[2] = 2;
    sendcounts[3] = 2;

    /* Posizione iniziale di ogni porzione. */
    displacements[0] = 0;
    displacements[1] = 3;
    displacements[2] = 6;
    displacements[3] = 8;
  }

  /* Codice per tutti i processi:
   * Ogni processo deve sapere quanti elementi riceverà. */
  int local_size;

  /* MPI_Scatter iniziale per distribuire local_size, così ogni
   * processo sa quanti elementi deve allocare */
  MPI_Scatter(
    sendcounts,
    1,
    MPI_INT,
    &local_size,
    1,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  int local_values[local_size];

  /* Distribuzione non uniforme del vettore. */
  MPI_Scatterv(
    values,
    sendcounts,
    displacements,
    MPI_INT,
    local_values,
    local_size,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Ricevuti %d elementi: ", my_rank, local_size);

  for(int i = 0; i < local_size; i++) {
    printf("%d ", local_values[i]);
  }

  printf("\n");

  /* Raccolta con quantità variabili. */
  MPI_Gatherv(
    local_values,
    local_size,
    MPI_INT,
    result,
    sendcounts,
    displacements,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  if(my_rank == 0) {

    printf("\n[Processo 0] Vettore ricostruito: ");

    for(int i = 0; i < n; i++) {
      printf("%d ", result[i]);
    }

    printf("\n");
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Scatter e MPI_Gather richiedono che ogni processo riceva/invii
 * lo stesso numero di elementi.
 *
 * Quando il numero di dati non è divisibile per il numero di processi
 * si utilizzano:
 *
 *    MPI_Scatterv
 *    MPI_Gatherv
 *
 * MPI_Scatterv utilizza:
 *
 * - sendcounts: indica quanti elementi inviare ad ogni processo;
 * - displacements: indica da quale posizione del buffer iniziare la copia.
 *
 * MPI_Gatherv utilizza gli stessi concetti per ricostruire il buffer finale.
 * ---------------------------------------------------------------------- */
