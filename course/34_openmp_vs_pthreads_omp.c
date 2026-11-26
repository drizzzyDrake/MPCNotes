/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   OMP_NUM_THREADS=4 ./34_openmp_vs_pthreads_omp
 *
 * Utilizzeremo:
 *
 *   pthread_create()
 *   pthread_join()
 *   #pragma omp parallel
 *
 * L'obiettivo è confrontare il modello di programmazione Pthreads
 * con quello OpenMP, mostrando la differenza tra gestione manuale
 * e gestione automatica dei thread.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <pthread.h>
#include <omp.h>

/* Funzione eseguita dal thread creato tramite Pthreads.
 */
void* pthread_function(void* arg) {

  int id = *(int*)arg;

  printf("Pthread %d in esecuzione.\n", id);

  return NULL;
}

int main() {

  printf("=== ESEMPIO CON PTHREADS ===\n\n");

  pthread_t threads[4];
  int ids[4];

  /* Creiamo manualmente quattro thread.
   *
   * Il programmatore deve occuparsi:
   *
   * - della creazione dei thread;
   * - della gestione degli identificativi;
   * - dell'attesa della terminazione.
   */
  for(int i = 0; i < 4; i++) {

    ids[i] = i;

    pthread_create(
      &threads[i],
      NULL,
      pthread_function,
      &ids[i]
    );
  }

  /* Attendiamo la terminazione di tutti i thread.
   */
  for(int i = 0; i < 4; i++) {

    pthread_join(
      threads[i],
      NULL
    );
  }

  printf("\n=== ESEMPIO CON OPENMP ===\n\n");

  /* OpenMP nasconde la gestione diretta dei thread.
   *
   * Il programmatore specifica solamente che il blocco
   * deve essere eseguito in parallelo.
   */
#pragma omp parallel
  {

    int id = omp_get_thread_num();

    printf("OpenMP thread %d in esecuzione.\n", id);
  }

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 *
 * PTHREADS
 * --------
 *
 * Pthreads (POSIX Threads) è una libreria che permette di creare e
 * gestire thread direttamente tramite primitive del sistema operativo.
 *
 * Il programmatore controlla esplicitamente:
 *
 * - creazione dei thread;
 * - terminazione dei thread;
 * - sincronizzazione;
 * - gestione delle risorse condivise.
 *
 *
 * Esempio:
 *
 *   pthread_create()
 *
 * crea un nuovo thread associandogli una funzione da eseguire.
 *
 * La funzione:
 *
 *   pthread_join()
 *
 * permette ad un thread di attendere la terminazione di un altro thread.
 *
 *
 * OPENMP
 * ------
 *
 * OpenMP è un'interfaccia di programmazione ad alto livello per sistemi
 * a memoria condivisa.
 *
 * Il programmatore non crea direttamente i thread, ma utilizza direttive
 * del compilatore come:
 *
 *   #pragma omp parallel
 *
 * lasciando a OpenMP la gestione del gruppo di thread.
 *
 *
 * LIVELLO DI ASTRAZIONE
 * ---------------------
 *
 * Pthreads fornisce un controllo più dettagliato:
 *
 * - maggiore flessibilità;
 * - gestione manuale delle risorse;
 * - maggiore complessità del codice.
 *
 *
 * OpenMP fornisce un livello di astrazione maggiore:
 *
 * - codice più semplice;
 * - maggiore portabilità;
 * - minore controllo diretto sui thread.
 *
 *
 * PORTABILITÀ
 * -----------
 *
 * Su sistemi POSIX come Linux e macOS, OpenMP viene generalmente
 * implementato utilizzando Pthreads.
 *
 * Tuttavia il programmatore OpenMP non deve conoscere i dettagli
 * dell'implementazione sottostante.
 *
 * Lo stesso codice OpenMP può essere eseguito su sistemi differenti
 * senza modifiche significative.
 *
 *
 * CONFRONTO
 * ---------
 *
 * Pthreads:
 *
 * - controllo fine-grained;
 * - maggiore complessità;
 * - gestione manuale dei thread.
 *
 *
 * OpenMP:
 *
 * - alto livello di astrazione;
 * - maggiore semplicità;
 * - gestione automatica dei thread.
 *
 *
 * Utilizzare contemporaneamente OpenMP e Pthreads nello stesso programma
 * è possibile, ma può introdurre problemi di sincronizzazione e casi
 * limite difficili da gestire.
 *
 * Per questo motivo generalmente si sceglie un solo modello di
 * programmazione.
 * ---------------------------------------------------------------------- */
