/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./10_collectives_intro
 *
 * Utilizzeremo:
 *
 *    rank 0 --> raccoglie i risultati degli altri processi
 *    rank 1-3 --> calcolano un valore locale e lo inviano
 *
 * L'obiettivo è mostrare il problema dell'approccio point-to-point quando
 * tutti i processi devono comunicare il proprio risultato ad un singolo
 * processo.
 *
 * Questo approccio può creare un collo di bottiglia perché il processo 0
 * deve gestire tutte le comunicazioni.
 *
 * Le comunicazioni collettive MPI permettono di utilizzare algoritmi
 * ottimizzati, come strutture ad albero, per ridurre il costo della
 * comunicazione.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* In un programma parallelo spesso ogni processo calcola un risultato locale.
 * Ad esempio, nella somma globale:
 *
 *    Processo 0 --> valore locale 10
 *    Processo 1 --> valore locale 20
 *    Processo 2 --> valore locale 30
 *    Processo 3 --> valore locale 40
 *
 * Una soluzione semplice consiste nel far inviare tutti i risultati al
 * processo 0. Questo approccio però non scala bene:
 *
 *          1
 *          *
 *          *
 *    2 --- 0 --- 3
 *          *
 *          *
 *          4
 *
 * Il processo 0 deve ricevere p-1 messaggi. La complessità della comunicazione
 * cresce quindi linearmente:
 *
 *    O(p-1)
 *
 * Le comunicazioni collettive MPI utilizzano invece algoritmi più efficienti,
 * spesso basati su strutture ad albero.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  int local_value = (my_rank + 1) * 10;

  /* APPROCCIO POINT-TO-POINT ---------------------------------------------
   * Ogni processo invia il proprio valore al processo 0 tramite MPI_Send.
   * Il processo 0 deve quindi eseguire molteplici MPI_Recv.
   * -------------------------------------------------------------------- */
  if(my_rank == 0) {

    int total = local_value;

    printf("[Processo %d] Valore locale: %d\n", my_rank, local_value);

    for(int source = 1; source < comm_sz; source++) {

      int received_value;

      MPI_Recv(
        &received_value,
        1,
        MPI_INT,
        source,
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
      );

      printf("[Processo %d] Ricevuto valore %d dal processo %d\n", my_rank, received_value, source);

      total += received_value;
    }

    printf("[Processo %d] Somma totale: %d\n", my_rank, total);
  }

  else {

    MPI_Send(
      &local_value,
      1,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviato valore locale: %d\n", my_rank, local_value);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Un possibile approccio per raccogliere risultati locali consiste
 * nell'utilizzare solamente comunicazioni point-to-point:
 *
 *    MPI_Send
 *    MPI_Recv
 *
 * Ogni processo invia il proprio risultato ad un processo centrale che
 * raccoglie tutti i dati.
 *
 * Problema:
 *
 *    - il processo centrale diventa un collo di bottiglia;
 *    - il numero di comunicazioni cresce con il numero di processi.
 *
 * Con p processi:
 *
 *    numero di messaggi = p - 1
 *
 *    complessità = O(p-1)
 *
 * MPI introduce quindi le comunicazioni collettive:
 *
 *    REDUCE    : tutti ---> uno   
 *    (combina i dati di tutti in un unico risultato)
 *
 *    BROADCAST : uno   ---> tutti
 *    (invia lo stesso dato a tutti)
 *
 *    ALLREDUCE : tutti ---> tutti
 *    (combina i dati e invia lo stesso risultato a tutti)
 *
 *    SCATTER   : uno   ---> tutti
 *    (invia un pezzo diverso a ciascuno)
 *
 *    GATHER    : tutti ---> uno
 *    (raccoglie i pezzi diversi di tutti in un solo processo)
 *
 *    ALLGATHER : tutti ---> tutti
 *    (ogni processo condivide il proprio dato con tutti:
 *    risultato uguale per tutti)
 *
 *    ALLTOALL  : tutti ---> tutti
 *    (ogni processo invia un dato diverso a ciascuno:
 *    risultato diverso per tutti)
 *
 * che permettono di esprimere operazioni comuni in modo più efficiente.
 *
 * Le implementazioni MPI possono scegliere automaticamente algoritmi
 * ottimizzati, ad esempio strutture ad albero con complessità:
 *
 *    O(log2 p)
 * ---------------------------------------------------------------------- */
