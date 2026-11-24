/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n p ./28_strong_scaling
 *
 * dove p rappresenta il numero di processi utilizzati.
 *
 * Utilizzeremo:
 *
 *   MPI_Scatter
 *   MPI_Reduce
 *   MPI_Barrier
 *   MPI_Wtime
 *
 * L'obiettivo è mostrare lo Strong Scaling:
 *
 *   mantenere fisso il problema e aumentare il numero di processi.
 *
 * Vengono calcolati anche:
 *
 *   - Speedup
 *   - Efficienza
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

  /* VERSIONE SEQUENZIALE */
  double serial_time = 0.0;

  if(my_rank == 0) {

    long long serial_sum = 0;
    double start_serial = MPI_Wtime();

    for(int i = 0; i < N; i++) {
      serial_sum += vector[i];
    }

    double finish_serial = MPI_Wtime();
    serial_time = finish_serial - start_serial;

    printf("Somma sequenziale: %lld\n", serial_sum);
    printf("Tempo sequenziale: %.6f secondi\n\n", serial_time);
  }

  /* Comunichiamo il tempo sequenziale a tutti i processi. */
  MPI_Bcast(
    &serial_time,
    1,
    MPI_DOUBLE,
    0,
    MPI_COMM_WORLD
  );

  int* local_vector = malloc(local_n * sizeof(int));

  /* Sincronizziamo tutti i processi prima della misura parallela. */
  MPI_Barrier(MPI_COMM_WORLD);

  double start = MPI_Wtime();

  /* Distribuiamo sempre lo stesso problema:
   *
   *      N = 40000000
   *
   * Cambia solamente il numero di processi. */
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

  long long local_sum = 0;

  for(int i = 0; i < local_n; i++) {
    local_sum += local_vector[i];
  }

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

  /* Il tempo dell'esecuzione parallela è determinato
   * dal processo più lento. */
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
    double efficiency = speedup / comm_sz;

    printf("Somma parallela: %lld\n", global_sum);
    printf("Processi utilizzati: %d\n", comm_sz);
    printf("Dimensione problema: %d\n", N);
    printf("Tempo parallelo: %.6f secondi\n", parallel_time);
    printf("Speedup: %.3f\n", speedup);
    printf("Efficienza: %.3f (%.2f%%)\n", efficiency, efficiency * 100.0);
    printf("\n");
  }

  free(local_vector);

  if(my_rank == 0) {
    free(vector);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Lo Strong Scaling analizza il comportamento di un programma quando:
 *
 * - la dimensione del problema rimane costante;
 * - aumenta il numero di processi.
 *
 * In formula:
 *
 *   N = costante
 *   p aumenta
 *
 * L'obiettivo è ridurre il tempo di esecuzione aumentando le risorse.
 * Lo speedup viene calcolato come:
 *
 *              T_serial(n)
 *   S(n,p) = ----------------
 *            T_parallel(n,p)
 *
 * dove:
 *
 *      T_serial(n)     : tempo della versione sequenziale.
 *      T_parallel(n,p) : tempo della versione parallela con p processi.
 *
 * L'efficienza invece misura quanto bene vengono utilizzati
 * i processi disponibili:
 *
 *             S(n,p)
 *   E(n,p) = --------
 *               p
 *
 * Il caso ideale è:
 *
 *   Speedup = p
 *   Efficienza = 1
 *
 * Nella pratica il comportamento ideale non viene raggiunto a causa di:
 *
 * - comunicazioni tra processi;
 * - sincronizzazioni;
 * - overhead di gestione dei processi;
 * - parti sequenziali del programma.
 *
 * Un programma è detto strongly scalable quando mantiene una buona
 * efficienza anche aumentando il numero di processi.
 * ---------------------------------------------------------------------- */
