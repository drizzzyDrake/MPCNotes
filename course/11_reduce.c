/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./11_reduce
 *
 * Utilizzeremo:
 *
 *    tutti i processi --> calcolano un valore locale
 *    rank 0           --> riceve il risultato della riduzione
 *
 * L'obiettivo è mostrare come MPI_Reduce permetta di combinare i dati
 * provenienti da tutti i processi tramite una singola comunicazione
 * collettiva.
 *
 * In questo esempio utilizzeremo l'operazione MPI_SUM per calcolare
 * la somma dei valori locali.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Reduce è una comunicazione collettiva.
 *
 * A differenza delle comunicazioni point-to-point:
 *
 *    MPI_Send
 *    MPI_Recv
 *
 * tutti i processi del communicator devono chiamare MPI_Reduce.
 *
 * Ogni processo fornisce un valore locale. MPI combina automaticamente tutti 
 * i valori utilizzando un'operazione (ad esempio somma, massimo o minimo).
 * Solamente il processo root riceve il risultato finale. 
 *
 * L'implementazione della riduzione è gestita internamente da MPI, che può
 * utilizzare algoritmi ottimizzati, ad esempio strutture ad albero.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* Ogni processo genera un valore diverso. */
  int local_value = (my_rank + 1) * 10;

  printf("[Processo %d] Valore locale: %d\n", my_rank, local_value);

  int global_sum;

  /* MPI_Reduce combina i valori di tutti i processi.
   *
   * Parametri:
   *
   * - &local_value   : dato locale da inviare;
   * - &global_sum    : risultato della riduzione (valido solo sul root);
   * - 1              : numero di elementi;
   * - MPI_INT        : tipo di dato;
   * - MPI_SUM        : operazione da applicare;
   * - 0              : processo root;
   * - MPI_COMM_WORLD : communicator.
   */
  MPI_Reduce(
    &local_value,
    &global_sum,
    1,
    MPI_INT,
    MPI_SUM,
    0,
    MPI_COMM_WORLD
  );

  /* Solo il processo root possiede il risultato finale. */
  if(my_rank == 0) {

    printf("\n[Processo %d] Somma globale: %d\n", my_rank, global_sum);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Reduce è una comunicazione collettiva.
 *
 * Tutti i processi del communicator devono chiamare la funzione.
 *
 * Ogni processo fornisce un valore locale.
 *
 * MPI combina automaticamente tutti i valori utilizzando
 * un'operazione, ad esempio:
 *
 *    MPI_SUM
 *    MPI_MAX
 *    MPI_MIN
 *    MPI_PROD
 *
 * Solo il processo root riceve il risultato finale.
 *
 * Rispetto ad una soluzione basata su molte MPI_Send e MPI_Recv,
 * MPI_Reduce permette all'implementazione MPI di utilizzare algoritmi
 * più efficienti, spesso basati su strutture ad albero.
 * ---------------------------------------------------------------------- */
