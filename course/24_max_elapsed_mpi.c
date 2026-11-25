/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./24_max_elapsed_mpi
 *
 * Utilizzeremo:
 *
 *   MPI_Barrier
 *   MPI_Wtime
 *   MPI_Reduce con MPI_MAX
 *
 * L'obiettivo è mostrare come ottenere il tempo complessivo di una
 * applicazione parallela considerando il processo più lento.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* Sincronizziamo tutti i processi prima della misura.
   */
  MPI_Barrier(MPI_COMM_WORLD);

  double start = MPI_Wtime();

  /* Simuliamo un carico diverso per ogni processo. 
   */
  long long result = 0;

  for(long long i = 0; i < (my_rank + 1) * 100000000; i++) {
    result += i;
  }

  double finish = MPI_Wtime();
  double local_elapsed = finish - start;
  double max_elapsed;

  /* Ogni processo invia il proprio tempo. MPI_MAX seleziona il
   * valore massimo. Solo il processo root riceve il risultato finale. 
   */
  MPI_Reduce(
    &local_elapsed,
    &max_elapsed,
    1,
    MPI_DOUBLE,
    MPI_MAX,
    0,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Tempo locale: %.6f secondi\n", my_rank, local_elapsed);

  if(my_rank == 0) {
    printf("\n[Processo 0] Tempo totale applicazione parallela: %.6f secondi\n", max_elapsed);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * In un programma parallelo ogni processo misura un proprio tempo locale:
 *
 *   local_elapsed = finish - start
 *
 * Questi valori possono essere differenti perché i processi possono
 * terminare in momenti diversi.
 *
 * Il tempo complessivo dell'applicazione parallela è determinato dal
 * processo più lento:
 *
 *   T_parallel = max(local_elapsed)
 *
 * Per ottenere questo valore globale si utilizza:
 *
 *   MPI_Reduce
 *
 * con l'operazione:
 *
 *   MPI_MAX
 *
 * MPI_Reduce raccoglie i valori prodotti da tutti i processi e applica
 * un'operazione di riduzione. In questo caso:
 *
 *   ogni processo  --> invia il proprio tempo locale
 *   rank 0         --> riceve il tempo massimo
 *
 * Solo il processo root possiede il risultato finale, mentre gli altri
 * processi partecipano solamente alla riduzione.
 *
 * Questo valore rappresenta il tempo reale di esecuzione parallela e viene
 * utilizzato per calcolare metriche di prestazione come:
 *
 * - speedup;
 * - efficienza;
 * - scalabilità.
 * ---------------------------------------------------------------------- */
