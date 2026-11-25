/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 4 ./13_allreduce_mpi
 *
 * Utilizzeremo:
 *
 *   tutti i processi --> calcolano un valore locale
 *
 * L'obiettivo è mostrare come MPI_Allreduce permetta di combinare i dati
 * provenienti da tutti i processi e distribuire automaticamente il
 * risultato a tutti i partecipanti.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Allreduce è una comunicazione collettiva che combina i dati
 * provenienti da tutti i processi e rende il risultato disponibile
 * su tutti i processi del communicator. 
 *
 * Dal punto di vista logico equivale a:
 *
 *   MPI_Reduce + MPI_Bcast
 *
 * Tuttavia MPI_Allreduce viene implementata direttamente da MPI
 * utilizzando algoritmi ottimizzati, evitando di eseguire due
 * comunicazioni separate. 
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int local_value = (my_rank + 1) * 10;
  int global_sum;

  printf("[Processo %d] Valore locale: %d\n", my_rank, local_value);

  MPI_Barrier(MPI_COMM_WORLD);

  /* MPI_Allreduce combina i valori di tutti i processi e
   * memorizza il risultato su ogni processo.
   *
   * Parametri:
   *
   * - &local_value   : valore locale;
   * - &global_sum    : risultato finale;
   * - 1              : numero di elementi;
   * - MPI_INT        : tipo di dato;
   * - MPI_SUM        : operazione da applicare;
   * - MPI_COMM_WORLD : communicator.
   */
  MPI_Allreduce(
    &local_value,
    &global_sum,
    1,
    MPI_INT,
    MPI_SUM,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Somma globale: %d\n", my_rank, global_sum);

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO ---------------------------------------------------------------
 * MPI_Allreduce è una comunicazione collettiva. Tutti i processi
 * partecipano alla riduzione.
 *
 * MPI combina automaticamente tutti i valori utilizzando
 * un'operazione (MPI_SUM, MPI_MAX, MPI_MIN, ...).
 *
 * A differenza di MPI_Reduce, dove solo il root riceve il risultato,
 * con MPI_Allreduce tutti i processi ricevono il risultato finale.
 *
 * Dal punto di vista logico:
 *
 *   MPI_Allreduce = MPI_Reduce + MPI_Bcast
 *
 * ma MPI può utilizzare implementazioni più efficienti rispetto
 * all'esecuzione separata delle due operazioni.
 * ----------------------------------------------------------------------- */
