/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./27_efficiency_mpi
 *
 * Utilizzeremo:
 *
 *   MPI_Scatter
 *   MPI_Reduce
 *   MPI_Barrier
 *   MPI_Wtime
 *
 * L'obiettivo è mostrare come calcolare l'efficienza di un programma
 * parallelo a partire dallo speedup ottenuto.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 40000000

int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  if(N % comm_sz != 0) {

    if(my_rank == 0) {
      printf("N deve essere divisibile per il numero di processi.\n");
    }

    MPI_Finalize();
    return 0;
  }

  int local_n = N / comm_sz;
  int* vector = NULL;

  if(my_rank == 0) {

    vector = malloc(N * sizeof(int));

    for(int i = 0; i < N; i++) {
      vector[i] = 1;
    }
  }

  int* local_vector = malloc(local_n * sizeof(int));

  /* VERSIONE SEQUENZIALE */

  double serial_time = 0.0;

  if(my_rank == 0) {

    long long serial_sum = 0;

    double start = MPI_Wtime();

    for(int i = 0; i < N; i++) {
      serial_sum += vector[i];
    }

    double finish = MPI_Wtime();

    serial_time = finish - start;

    printf("Somma sequenziale: %lld\n", serial_sum);
    printf("Tempo sequenziale: %.6f secondi\n\n", serial_time);
  }

  /* Tutti i processi devono conoscere il tempo sequenziale.
   */
  MPI_Bcast(
    &serial_time,
    1,
    MPI_DOUBLE,
    0,
    MPI_COMM_WORLD
  );

  /* VERSIONE PARALLELA.
   */

  MPI_Barrier(MPI_COMM_WORLD);

  double start = MPI_Wtime();

  /* Distribuiamo il vettore tra i processi. 
   */
  MPI_Scatter(
    vector,
    local_n,
    MPI_INT,
    local_vector,
    local_n,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  /* Ogni processo calcola la propria somma locale. 
   */
  long long local_sum = 0;

  for(int i = 0; i < local_n; i++) {
    local_sum += local_vector[i];
  }

  /* Sommiamo tutti i risultati locali. 
   */
  long long global_sum;

  MPI_Reduce(
    &local_sum,
    &global_sum,
    1,
    MPI_LONG_LONG,
    MPI_SUM,
    0,
    MPI_COMM_WORLD
  );

  double finish = MPI_Wtime();
  double local_elapsed = finish - start;
  double parallel_time;

  /* Il tempo dell'applicazione è quello del processo più lento. 
   */
  MPI_Reduce(
    &local_elapsed,
    &parallel_time,
    1,
    MPI_DOUBLE,
    MPI_MAX,
    0,
    MPI_COMM_WORLD
  );

  if(my_rank == 0) {

    /* Calcoliamo lo speedup. 
     */
    double speedup = serial_time / parallel_time;

    /* L'efficienza misura quanto bene vengono utilizzati i processi
     * disponibili.
     */
    double efficiency = speedup / comm_sz;

    printf("Somma parallela : %lld\n", global_sum);
    printf("Tempo parallelo : %.6f secondi\n", parallel_time);
    printf("Speedup         : %.3f\n", speedup);
    printf("Efficienza      : %.3f (%.2f%%)\n", efficiency, efficiency * 100.0);
  }

  free(local_vector);

  if(my_rank == 0) {
    free(vector);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * L'efficienza misura quanto bene vengono sfruttate le risorse di calcolo
 * disponibili durante l'esecuzione parallela.
 *
 * La formula è:
 *
 *              S(n,p)
 *   E(n,p) = ---------
 *                p
 *
 * oppure:
 *
 *                T_serial(n)
 *   E(n,p) = --------------------
 *            p * T_parallel(n,p)
 *
 * dove:
 *
 *   S(n,p)          : speedup.
 *   T_serial(n)     : tempo della versione sequenziale.
 *   T_parallel(n,p) : tempo della versione parallela con p processi.
 *   p               : numero di processi.
 *
 * Il valore ideale è:
 *
 *   E(n,p) = 1
 *
 * cioè ogni processo contribuisce completamente all'accelerazione
 * del programma.
 *
 * Nella pratica:
 *
 *   E(n,p) <= 1
 *
 * a causa di:
 *
 * - comunicazioni;
 * - sincronizzazioni;
 * - overhead della parallelizzazione;
 * - parti non parallelizzabili.
 *
 * Ad esempio:
 *
 *   Speedup = 3.2
 *   Processi = 4
 *
 *   Efficienza = 3.2 / 4 = 0.8 = 80%
 *
 * Questo significa che il programma sfrutta circa l'80% del potenziale
 * teorico dei quattro processi disponibili.
 *
 * All'aumentare del numero di processi, mantenendo fissa la dimensione
 * del problema, l'efficienza tende generalmente a diminuire.
 * ---------------------------------------------------------------------- */
