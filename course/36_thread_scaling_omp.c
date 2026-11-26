/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   OMP_NUM_THREADS=4 ./36_thread_scaling_omp
 *
 * Per analizzare lo scaling è consigliato provare:
 *
 *   OMP_NUM_THREADS=1
 *   OMP_NUM_THREADS=2
 *   OMP_NUM_THREADS=4
 *   OMP_NUM_THREADS=8
 *   OMP_NUM_THREADS=16
 *   OMP_NUM_THREADS=32
 *
 * Utilizzeremo:
 *
 *   omp_get_wtime()
 *   omp_get_num_threads()
 *   #pragma omp parallel
 *
 * L'obiettivo è mostrare come varia il tempo di esecuzione aumentando
 * il numero di thread e osservare il limite dello scaling.
 *
 * Il programma utilizza un lavoro indipendente per ogni thread, così da
 * evidenziare il rapporto tra lavoro utile e overhead del parallelismo.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <omp.h>

int main() {

  const long long N = 100000000;

  double result = 0.0;

  int num_threads = 1;

  double start = omp_get_wtime();

#pragma omp parallel
  {
    double local_sum = 0.0;

#pragma omp single
    {
      num_threads = omp_get_num_threads();
    }

    /*
     * Ogni thread esegue una parte del lavoro.
     *
     * La variabile local_sum è privata del thread e non richiede
     * sincronizzazione durante il calcolo.
     */
#pragma omp for schedule(static)
    for(long long i = 0; i < N; i++) {

      local_sum += (double)i * 0.000001;
    }

    /*
     * Somma finale dei risultati locali.
     *
     * Questa sincronizzazione avviene una sola volta al termine
     * del lavoro parallelo.
     */
#pragma omp atomic
    result += local_sum;
  }

  double finish = omp_get_wtime();

  printf("Thread utilizzati: %d\n", num_threads);
  printf("Risultato: %.2f\n", result);
  printf("Tempo esecuzione: %.6f secondi\n", finish - start);

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 *
 * SCALING OPENMP
 * --------------
 *
 * Lo scaling misura come cambia il tempo di esecuzione aumentando
 * il numero di thread utilizzati.
 *
 * L'obiettivo del parallelismo è dividere un lavoro tra più thread
 * per ridurre il tempo totale di esecuzione.
 *
 *
 * DIVISIONE DEL LAVORO
 * --------------------
 *
 * La direttiva:
 *
 *   #pragma omp for
 *
 * divide automaticamente le iterazioni del ciclo tra i thread
 * appartenenti alla regione parallela.
 *
 * Utilizzando:
 *
 *   schedule(static)
 *
 * il lavoro viene diviso prima dell'esecuzione in blocchi assegnati
 * ai thread.
 *
 *
 * SCALING IDEALE
 * --------------
 *
 * Se il lavoro viene diviso perfettamente:
 *
 * - aumentando i thread diminuisce il tempo;
 * - ogni thread esegue una parte del calcolo.
 *
 * Tuttavia il miglioramento non è infinito.
 *
 *
 * OVERHEAD DEL PARALLELISMO
 * -------------------------
 *
 * Ogni regione parallela introduce un costo dovuto alla gestione
 * dei thread.
 *
 * Questo costo comprende:
 *
 * - creazione o attivazione dei thread;
 * - assegnazione del lavoro;
 * - sincronizzazione finale.
 *
 * Se ogni thread riceve troppo poco lavoro, l'overhead può diventare
 * significativo rispetto al calcolo eseguito.
 *
 *
 * LIMITE DELLO SCALING
 * --------------------
 *
 * Aumentare il numero di thread non garantisce sempre prestazioni
 * migliori.
 *
 * Dopo un certo numero di thread il beneficio diminuisce perché:
 *
 * - il lavoro disponibile viene suddiviso in parti troppo piccole;
 * - aumenta il costo della gestione dei thread;
 * - le risorse hardware vengono condivise.
 *
 *
 * OSSERVAZIONE SPERIMENTALE
 * -------------------------
 *
 * Il numero ottimale di thread dipende dal programma.
 *
 * Un'applicazione con molto lavoro indipendente può scalare bene,
 * mentre applicazioni con molta sincronizzazione possono peggiorare
 * aumentando troppo il numero di thread.
 * ---------------------------------------------------------------------- */
