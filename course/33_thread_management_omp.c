/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   OMP_NUM_THREADS=8 ./33_thread_management_omp
 *
 * Utilizzeremo:
 *
 *   omp_get_num_threads()
 *   omp_get_max_threads()
 *   omp_get_thread_num()
 *   omp_set_num_threads()
 *   #pragma omp parallel
 *
 * L'obiettivo è mostrare come controllare il numero di thread
 * utilizzati da OpenMP tramite la variabile d'ambiente
 * OMP_NUM_THREADS e la funzione omp_set_num_threads().
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <omp.h>

int main() {

  printf("Numero massimo iniziale di thread: %d\n\n", omp_get_max_threads());

  /* Prima regione parallela.
   *
   * Il numero di thread è determinato dalla configurazione
   * corrente di OpenMP (ad esempio OMP_NUM_THREADS).
   */
  printf("=== Prima regione parallela ===\n");

#pragma omp parallel
  {

    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();

    printf("Thread %d su %d\n", thread_id, num_threads);
  }

  /* Modifichiamo il numero di thread che verrà utilizzato
   * nella successiva regione parallela.
   */
  omp_set_num_threads(4);

  printf("\nNumero massimo dopo omp_set_num_threads(4): %d\n\n",
         omp_get_max_threads());

  /* Seconda regione parallela.
   */
  printf("=== Seconda regione parallela ===\n");

#pragma omp parallel
  {

    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();

    printf("Thread %d su %d\n", thread_id, num_threads);
  }

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 *
 * NUMERO DI THREAD
 * ----------------
 *
 * OpenMP utilizza un gruppo di thread (thread team) per eseguire una
 * regione parallela.
 *
 * Il numero di thread può essere determinato:
 *
 * - automaticamente dall'implementazione;
 * - tramite la variabile d'ambiente OMP_NUM_THREADS;
 * - tramite la funzione omp_set_num_threads().
 *
 *
 * OMP_NUM_THREADS
 * ---------------
 *
 * La variabile d'ambiente:
 *
 *   OMP_NUM_THREADS
 *
 * imposta il numero di thread utilizzato da OpenMP prima
 * dell'avvio del programma.
 *
 * Ad esempio:
 *
 *   OMP_NUM_THREADS=8 ./programma
 *
 * richiede la creazione di 8 thread.
 *
 *
 * omp_set_num_threads()
 * ---------------------
 *
 * La funzione:
 *
 *   omp_set_num_threads(int n)
 *
 * modifica il numero di thread che OpenMP utilizzerà
 * nelle successive regioni parallele.
 *
 *
 * omp_get_max_threads()
 * ---------------------
 *
 * La funzione:
 *
 *   omp_get_max_threads()
 *
 * restituisce il numero massimo di thread che OpenMP
 * utilizzerà per creare una nuova regione parallela.
 *
 * Il valore può cambiare durante l'esecuzione del programma,
 * ad esempio dopo una chiamata a:
 *
 *   omp_set_num_threads()
 *
 *
 * omp_get_num_threads()
 * ---------------------
 *
 * La funzione:
 *
 *   omp_get_num_threads()
 *
 * restituisce il numero di thread presenti nella regione
 * parallela corrente.
 *
 * Se viene invocata al di fuori di una regione parallela,
 * restituisce 1.
 *
 *
 * omp_get_thread_num()
 * --------------------
 *
 * La funzione:
 *
 *   omp_get_thread_num()
 *
 * restituisce l'identificativo del thread corrente.
 *
 * Gli identificativi sono compresi tra:
 *
 *   0 ... N - 1
 *
 * dove N rappresenta il numero totale di thread presenti
 * nella regione parallela.
 *
 * Il thread con identificativo 0 è il thread principale
 * (master).
 * ---------------------------------------------------------------------- */
