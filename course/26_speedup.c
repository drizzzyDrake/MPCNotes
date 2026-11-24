/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./26_speedup
 *
 * Utilizzeremo:
 *
 *   MPI_Scatter
 *   MPI_Reduce
 *   MPI_Barrier
 *   MPI_Wtime
 *
 * L'obiettivo è mostrare come calcolare lo speedup confrontando
 * l'esecuzione sequenziale e quella parallela dello stesso algoritmo.
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

  /* VERSIONE SEQUENZIALE  */

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

  /* Tutti i processi devono conoscere il tempo seriale. */
  MPI_Bcast(
    &serial_time,
    1,
    MPI_DOUBLE,
    0,
    MPI_COMM_WORLD
  );

  /* VERSIONE PARALLELA */

  MPI_Barrier(MPI_COMM_WORLD);

  double start = MPI_Wtime();

  /* Distribuiamo il vettore tra i processi. */
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

  /* Ogni processo calcola la propria somma locale. */
  long long local_sum = 0;

  for(int i = 0; i < local_n; i++) {
    local_sum += local_vector[i];
  }

  /* Sommiamo tutti i risultati locali. */
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

  /* Il tempo dell'applicazione è quello del processo più lento. */
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

    double speedup = serial_time / parallel_time;

    printf("Somma parallela : %lld\n", global_sum);
    printf("Tempo parallelo : %.6f secondi\n", parallel_time);
    printf("Speedup         : %.3f\n", speedup);
  }

  free(local_vector);

  if(my_rank == 0) {
    free(vector);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Lo speedup misura il miglioramento ottenuto utilizzando il parallelismo.
 *
 *              T_serial(n)
 *   S(n,p) = ----------------
 *            T_parallel(n,p)
 *
 * dove:
 *
 *   T_serial(n)     : tempo della versione sequenziale.
 *   T_parallel(n,p) : tempo della versione parallela con p processi.
 *
 * Per ottenere una misura corretta è fondamentale confrontare
 * la stessa applicazione nelle due versioni.
 *
 * In questo esempio:
 *
 * - la versione sequenziale somma l'intero vettore;
 * - la versione parallela divide il vettore tra tutti i processi con
 *   MPI_Scatter;
 * - ogni processo calcola una somma locale;
 * - MPI_Reduce con MPI_SUM ricostruisce la somma finale;
 * - MPI_Reduce con MPI_MAX individua il tempo reale dell'applicazione
 *   parallela.
 *
 * Lo speedup ideale è:
 *
 *   S(n,p) = p
 *
 * Nella pratica risulta generalmente inferiore a causa di:
 *
 * - comunicazioni;
 * - sincronizzazioni;
 * - overhead di MPI;
 * - parti non parallelizzabili.
 * ---------------------------------------------------------------------- */
