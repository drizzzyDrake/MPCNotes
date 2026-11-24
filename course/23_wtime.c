/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./23_wtime
 *
 * Utilizzeremo:
 *
 *    MPI_Barrier
 *    MPI_Wtime
 *
 * L'obiettivo è mostrare come misurare il tempo di esecuzione
 * di una sezione di codice parallela.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Wtime restituisce il tempo corrente in secondi.
 *
 * Per misurare una porzione di codice:
 *
 *    start = MPI_Wtime();
 *    codice da misurare
 *    finish = MPI_Wtime();
 *    elapsed = finish - start;
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* Sincronizzazione iniziale.
   *
   * Tutti i processi aspettano che anche gli altri siano arrivati
   * prima di iniziare la misura. */
  MPI_Barrier(MPI_COMM_WORLD);

  double start = MPI_Wtime();

  /* Simuliamo un lavoro computazionale.
   *
   * Ogni processo esegue un numero diverso di operazioni,
   * così possiamo osservare tempi differenti (lavoro fittizio). */
  volatile long long result = 0;

  for(long long i = 0; i < (my_rank + 1) * 100000000; i++) {
    result += i;
  }

  double finish = MPI_Wtime();
  double elapsed = finish - start;

  printf("[Processo %d] Tempo locale: %.6f secondi\n", my_rank, elapsed);

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Wtime permette di misurare il tempo di esecuzione di una sezione
 * di codice parallela.
 *
 * Il tempo viene calcolato come:
 *
 *    elapsed = finish - start
 *
 * dove:
 *
 *    start  = MPI_Wtime() prima dell'operazione
 *    finish = MPI_Wtime() dopo l'operazione
 *
 * Prima di iniziare la misura è consigliabile utilizzare MPI_Barrier
 * per sincronizzare tutti i processi.
 *
 * In un programma parallelo ogni processo possiede un proprio tempo locale:
 *
 *    rank 0 --> elapsed0
 *    rank 1 --> elapsed1
 *    ...
 *
 * Questi tempi possono essere differenti perché i processi possono
 * terminare in istanti diversi.
 *
 * Il tempo complessivo dell'applicazione parallela non è dato dal tempo
 * medio o dal tempo di un singolo processo, ma dal processo più lento:
 *
 *    T_parallel = max(elapsed0, elapsed1, ..., elapsedp)
 *
 * Per ottenere questo valore globale si utilizza generalmente MPI_Reduce
 * con l'operazione MPI_MAX.
 *
 * Questo valore sarà utilizzato per valutare le prestazioni del programma
 * tramite metriche come:
 *
 * - speedup;
 * - efficienza;
 * - scalabilità.
 * ---------------------------------------------------------------------- */
