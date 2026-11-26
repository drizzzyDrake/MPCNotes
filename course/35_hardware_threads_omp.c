/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   OMP_NUM_THREADS=4 ./35_hardware_threads_omp
 *
 * Utilizzeremo:
 *
 *   omp_get_num_procs()
 *   omp_get_max_threads()
 *   omp_get_thread_num()
 *   #pragma omp parallel
 *
 * L'obiettivo è mostrare come analizzare le risorse hardware disponibili
 * e collegarle alla scelta del numero di thread OpenMP.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <omp.h>

int main() {

  int processors = omp_get_num_procs();
  int max_threads = omp_get_max_threads();

  printf("Processori disponibili: %d\n", processors);
  printf("Numero massimo di thread OpenMP: %d\n\n", max_threads);

  /* Creiamo una regione parallela per mostrare quanti thread
   * vengono effettivamente creati da OpenMP.
   */
#pragma omp parallel
  {

    int thread_id = omp_get_thread_num();

    printf("Thread OpenMP %d in esecuzione.\n", thread_id);
  }

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 *
 * ARCHITETTURA HARDWARE
 * ---------------------
 *
 * Per utilizzare correttamente il parallelismo a memoria condivisa è
 * importante conoscere le caratteristiche della macchina sulla quale
 * viene eseguito il programma.
 *
 *
 * SOCKET
 * ------
 *
 * Un socket rappresenta un processore fisico installato sulla scheda
 * madre.
 *
 * Nei sistemi server possono essere presenti più socket, ognuno con
 * una propria CPU fisica.
 *
 *
 * CORE
 * ----
 *
 * Ogni processore fisico contiene uno o più core.
 *
 * Un core rappresenta una vera unità di calcolo capace di eseguire
 * istruzioni indipendentemente dagli altri core.
 *
 *
 * THREAD HARDWARE
 * ---------------
 *
 * I moderni processori possono utilizzare tecnologie come:
 *
 *   Hyper-Threading
 *
 * o più generalmente:
 *
 *   Simultaneous Multithreading (SMT)
 *
 * che permettono ad ogni core fisico di gestire più thread hardware.
 *
 * Ad esempio:
 *
 *   8 core fisici
 *   2 thread hardware per core
 *
 * producono:
 *
 *   8 * 2 = 16 processori logici
 *
 *
 * PROCESSORI LOGICI
 * -----------------
 *
 * Il sistema operativo vede i thread hardware come CPU logiche.
 *
 * Il comando:
 *
 *   lscpu
 *
 * permette di visualizzare:
 *
 * - numero di socket;
 * - core per socket;
 * - thread per core;
 * - numero totale di CPU logiche.
 *
 *
 * NUMERO DI THREAD OPENMP
 * -----------------------
 *
 * Il numero ideale di thread OpenMP dipende dal problema e
 * dall'architettura hardware.
 *
 * In generale:
 *
 * - utilizzare più thread dei core fisici può ridurre le prestazioni;
 * - utilizzare pochi thread può lasciare risorse inutilizzate.
 *
 *
 * OVER-SUBSCRIPTION
 * -----------------
 *
 * Creare più thread software rispetto ai thread hardware disponibili
 * viene chiamato over-subscription.
 *
 * In questo caso il sistema operativo deve alternare continuamente
 * l'esecuzione dei thread tramite context switch.
 *
 * Questo introduce overhead e può peggiorare le prestazioni.
 *
 *
 * NUMERO DI PROCESSORI OPENMP
 * ---------------------------
 *
 * La funzione:
 *
 *   omp_get_num_procs()
 *
 * restituisce il numero di processori disponibili per OpenMP.
 *
 * Questo valore corrisponde generalmente al numero di CPU logiche viste
 * dal sistema operativo.
 *
 *
 * ESEMPIO PRATICO
 * ---------------
 *
 * Una macchina con:
 *
 *   1 socket
 *   8 core fisici
 *   2 thread hardware per core
 *
 * presenta:
 *
 *   8 core fisici
 *
 * e:
 *
 *   8 * 2 = 16 CPU logiche
 *
 * In questo caso OpenMP può creare fino a 16 thread hardware,
 * anche se il numero ottimale dipende dal tipo di applicazione.
 * ---------------------------------------------------------------------- */
