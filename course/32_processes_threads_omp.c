/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   OMP_NUM_THREADS=4 ./32_processes_threads_omp
 *
 * Utilizzeremo:
 *
 *   #pragma omp parallel
 *   omp_get_thread_num()
 *   omp_get_num_threads()
 *
 * L'obiettivo è introdurre il modello di programmazione OpenMP e mostrare
 * come un processo possa creare un gruppo di thread che eseguono
 * contemporaneamente lo stesso blocco di codice.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <omp.h>

int main() {

  printf("Prima della regione parallela.\n\n");

  /* Creiamo una regione parallela.
   *
   * Il processo genera automaticamente un gruppo di thread.
   *
   * Ogni thread esegue il codice contenuto nel blocco.
   */
#pragma omp parallel
  {

    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();

    printf("Thread %d su %d in esecuzione.\n", thread_id, num_threads);
  }

  printf("\nDopo la regione parallela.\n");

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 *
 * PROCESSO
 * --------
 *
 * Un processo è un'istanza di un programma in esecuzione.
 *
 * Ogni processo possiede:
 *
 * - uno spazio di indirizzamento privato;
 * - variabili proprie;
 * - stack;
 * - heap;
 * - risorse del sistema operativo.
 *
 *
 * THREAD
 * ------
 *
 * Un thread è un flusso di esecuzione appartenente ad un processo.
 *
 * Viene spesso definito come un processo leggero
 * (light-weight process).
 *
 * Tutti i thread dello stesso processo condividono:
 *
 * - memoria globale;
 * - heap;
 * - file aperti.
 *
 * Ogni thread possiede invece:
 *
 * - program counter;
 * - registri;
 * - stack privato.
 *
 *
 * MODELLO A MEMORIA CONDIVISA
 * ---------------------------
 *
 * OpenMP utilizza un modello di programmazione a memoria condivisa
 * (Shared Memory).
 *
 * Un solo processo crea più thread che condividono lo stesso spazio
 * di indirizzamento e possono accedere direttamente agli stessi dati.
 *
 * Questo elimina la necessità di inviare messaggi tra thread, ma rende
 * necessario sincronizzare l'accesso alle risorse condivise.
 *
 *
 * REGIONE PARALLELA
 * -----------------
 *
 * Una regione parallela è un blocco di codice delimitato dalla direttiva:
 *
 *   #pragma omp parallel
 *
 * Quando il flusso di esecuzione raggiunge questa direttiva:
 *
 * - viene creato un gruppo di thread;
 * - ogni thread esegue lo stesso blocco di codice;
 * - al termine tutti i thread vengono sincronizzati
 *   implicitamente e rimane attivo solo il thread principale.
 *
 *
 * IDENTIFICATIVO DEI THREAD
 * -------------------------
 *
 * OpenMP assegna ad ogni thread un identificativo univoco:
 *
 *   0, 1, 2, ...
 *
 * ottenibile tramite:
 *
 *   omp_get_thread_num()
 *
 * Il thread con identificativo 0 è il thread principale (master).
 *
 *
 * NUMERO DI THREAD
 * ----------------
 *
 * Il numero di thread che partecipano alla regione parallela è ottenibile
 * tramite:
 *
 *   omp_get_num_threads()
 *
 * e può essere controllato, ad esempio, impostando la variabile
 * d'ambiente:
 *
 *   OMP_NUM_THREADS
 *
 *
 * MPI VS OPENMP
 * -------------
 *
 * MPI:
 *
 * - crea più processi indipendenti;
 * - ogni processo possiede memoria privata;
 * - la comunicazione avviene tramite messaggi.
 *
 * OpenMP:
 *
 * - crea più thread nello stesso processo;
 * - tutti i thread condividono la memoria;
 * - la comunicazione avviene tramite memoria condivisa.
 * ---------------------------------------------------------------------- */
