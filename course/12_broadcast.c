/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./12_broadcast
 *
 * Utilizzeremo:
 *
 *    rank 0 --> genera un valore
 *    tutti gli altri processi --> ricevono lo stesso valore
 *
 * L'obiettivo è mostrare come MPI_Bcast permetta di distribuire lo
 * stesso dato a tutti i processi del communicator.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI_Bcast è una comunicazione collettiva che permette ad un processo
 * (detto root) di inviare lo stesso dato a tutti gli altri processi.
 *
 * Tutti i processi del communicator devono chiamare MPI_Bcast.
 *
 * Solamente il processo root inizializza il dato da inviare.
 * Al termine della comunicazione tutti i processi possiedono una copia
 * dello stesso valore.
 *
 * Internamente MPI può utilizzare algoritmi ottimizzati, ad esempio
 * strutture ad albero, evitando che il root debba eseguire numerose
 * MPI_Send. 
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int value;

  /* Solo il processo root inizializza il dato. */
  if(my_rank == 0) {

    value = 42;

    printf("[Processo %d] Valore iniziale: %d\n", my_rank, value);
  }

  /* MPI_Bcast invia lo stesso valore a tutti i processi.
   *
   * Parametri:
   *
   * - &value         : buffer contenente il dato;
   * - 1              : numero di elementi;
   * - MPI_INT        : tipo di dato;
   * - 0              : processo root;
   * - MPI_COMM_WORLD : communicator. */
  MPI_Bcast(
    &value,
    1,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  printf("[Processo %d] Valore dopo MPI_Bcast: %d\n", my_rank, value);

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO ---------------------------------------------------------------
 * MPI_Bcast è una comunicazione collettiva.
 *
 * Tutti i processi del communicator devono chiamare la funzione.
 *
 * Il processo root fornisce il dato da distribuire.
 *
 * Al termine della comunicazione tutti i processi possiedono
 * una copia dello stesso valore.
 *
 * MPI_Bcast sostituisce una sequenza di MPI_Send verso tutti
 * i processi e permette a MPI di utilizzare algoritmi di
 * comunicazione più efficienti.
 * ----------------------------------------------------------------------- */
