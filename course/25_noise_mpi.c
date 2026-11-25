/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./25_noise_mpi
 *
 * Utilizzeremo:
 *
 *   MPI_Wtime
 *   MPI_Barrier
 *   MPI_Reduce con MPI_MAX
 *
 * L'obiettivo è mostrare che il tempo di esecuzione parallelo varia tra
 * esecuzioni diverse a causa del rumore di sistema.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
 
  MPI_Init(&argc, &argv);
  
  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* Ripetiamo più volte la stessa misura perché una singola esecuzione
   * potrebbe essere influenzata dal rumore di sistema.
   */
  int executions = 5;

  for(int e = 0; e < executions; e++) {

    /* Sincronizziamo i processi prima di ogni misura. 
     */
    MPI_Barrier(MPI_COMM_WORLD);

    double start = MPI_Wtime();

    /* Simuliamo il lavoro. 
     */
    volatile long long result = 0;

    for(long long i = 0; i < 100000000; i++) {
      result += i;
    }

    double finish = MPI_Wtime();
    double local_elapsed = finish - start;
    double max_elapsed;

    /* Il tempo dell'applicazione è il massimo tra tutti i processi.
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

    if(my_rank == 0) {
      printf("[Esecuzione %d] Tempo totale: %.6f secondi\n", e + 1, max_elapsed);
    }
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Una singola esecuzione non è sufficiente per valutare correttamente
 * le prestazioni di un programma parallelo.
 *
 * Il tempo di esecuzione può variare tra esecuzioni diverse a causa del
 * rumore di sistema (noise).
 *
 * Le principali cause del rumore sono:
 *
 * - processi in esecuzione sul sistema operativo;
 * - context switch;
 * - utilizzo della cache;
 * - competizione per le risorse hardware;
 * - traffico sulla rete nei sistemi distribuiti.
 *
 * Il rumore può causare variazioni nei tempi anche quando:
 *
 * - il codice è identico;
 * - il numero di processi è identico;
 * - i dati di ingresso sono gli stessi.
 *
 * Aumentando il numero di processi il problema aumenta, perché cresce la
 * probabilità che almeno un processo venga rallentato.
 *
 * Poiché il tempo parallelo è determinato dal processo più lento:
 *
 *   T_parallel = max(tempi locali)
 *
 * un singolo processo rallentato può aumentare il tempo totale
 * dell'applicazione.
 *
 * Per ottenere misure affidabili è necessario:
 *
 * - eseguire il programma più volte;
 * - raccogliere tutti i tempi ottenuti;
 * - analizzare la distribuzione dei risultati.
 *
 * Le statistiche utilizzate possono includere:
 *
 * - minimo;
 * - massimo;
 * - media;
 * - mediana;
 * - intervalli di confidenza.
 *
 * In passato spesso veniva riportato il tempo minimo, assumendo che il
 * rumore potesse solo rallentare l'esecuzione.
 *
 * Tuttavia una valutazione più completa considera l'intera distribuzione
 * dei tempi ottenuti.
 * ---------------------------------------------------------------------- */
