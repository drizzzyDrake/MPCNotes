/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n p ./29_weak_scaling
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
 * L'obiettivo è mostrare il Weak Scaling:
 *
 *   aumentare la dimensione del problema insieme al numero di processi,
 *   mantenendo costante il lavoro assegnato a ciascun processo.
 *
 * Vengono calcolati anche:
 *
 *   - Speedup scalato
 *   - Efficienza
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define LOCAL_N 10000000

int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /* La dimensione totale cresce con il numero di processi.
   * Ogni processo riceve sempre LOCAL_N elementi. */
  int total_n = LOCAL_N * comm_sz;

  int* vector = NULL;

  double serial_time = 0.0;

  if(my_rank == 0) {

    vector = malloc(total_n * sizeof(int));

    for(int i = 0; i < total_n; i++) {
      vector[i] = 1;
    }
  }

  /* VERSIONE SEQUENZIALE
   *
   * La dimensione del problema è quella totale:
   *
   *      N = LOCAL_N * numero processi
   *
   * In questo modo confrontiamo il tempo necessario
   * per risolvere lo stesso problema in modo sequenziale.
   */
  if(my_rank == 0) {

    long long serial_sum = 0;

    double start_serial = MPI_Wtime();

    for(int i = 0; i < total_n; i++) {
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

  int* local_vector = malloc(LOCAL_N * sizeof(int));

  /* Sincronizzazione prima della misura. */
  MPI_Barrier(MPI_COMM_WORLD);

  double start = MPI_Wtime();

  /* Ogni processo riceve sempre lo stesso numero di elementi:
   *
   *   LOCAL_N
   *
   * Cambia solo il numero totale di elementi. */
  MPI_Scatter(
    vector,
    LOCAL_N,
    MPI_INT,
    local_vector,
    LOCAL_N,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  long long local_sum = 0;

  for(int i = 0; i < LOCAL_N; i++) {
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

  /* Il tempo totale è determinato dal processo più lento. */
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
    printf("Dimensione problema: %d elementi\n", total_n);
    printf("Lavoro per processo: %d elementi\n", LOCAL_N);
    printf("Tempo parallelo: %.6f secondi\n", parallel_time);
    printf("Speedup scalato: %.3f\n", speedup);
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
 * Il Weak Scaling analizza il comportamento di un programma quando:
 *
 * - aumenta il numero di processi;
 * - aumenta anche la dimensione del problema.
 *
 * L'obiettivo è mantenere costante il lavoro assegnato a ciascun processo.
 *
 * In formula:
 *
 *    N
 *   ---
 *    p
 *
 * deve rimanere costante.
 *
 * Nel nostro esempio:
 *
 *   ogni processo somma sempre LOCAL_N elementi.
 *
 * Aumentando i processi:
 *
 *   p aumenta
 *   N aumenta proporzionalmente
 *
 * A differenza dello Strong Scaling, il problema non rimane fisso.
 *
 * Lo scopo non è risolvere lo stesso problema più velocemente, ma
 * utilizzare più risorse per risolvere problemi più grandi mantenendo
 * un tempo di esecuzione simile.
 *
 * Lo speedup utilizzato nel Weak Scaling è lo speedup scalato:
 *
 *              T_serial(n)
 *   S(n,p) = ---------------
 *            T_parallel(n,p)
 *
 * dove il problema cresce insieme al numero di processi.
 *
 * L'efficienza viene calcolata come:
 *
 *             S(n,p)
 *   E(n,p) = --------
 *                p
 *
 * Il caso ideale è:
 *
 *   Speedup elevato
 *   Efficienza vicina a 1
 *
 * Un programma è weakly scalable quando il tempo di esecuzione rimane
 * circa costante aumentando contemporaneamente:
 *
 * - numero di processi;
 * - dimensione del problema.
 *
 * Le cause che riducono la scalabilità sono:
 *
 * - comunicazioni;
 * - sincronizzazioni;
 * - congestione della rete;
 * - overhead parallelo.
 * ---------------------------------------------------------------------- */
