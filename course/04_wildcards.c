/* NOTE --------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 3 ./04_wildcards
 *
 * Utilizzeremo:
 *
 *    rank 0 --> invia un messaggio
 *    rank 1 --> invia un messaggio
 *    rank 2 --> riceve due messaggi
 *
 * A differenza dell'esempio precedente, il processo destinatario NON
 * conosce a priori quale processo invierà il primo messaggio. Per questo 
 * motivo utilizzeremo le wildcard MPI_ANY_SOURCE e MPI_ANY_TAG.
 * ----------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* Negli esempi precedenti il processo ricevente conosceva già:
 *
 * - il mittente;
 * - il tag.
 *
 * In molte applicazioni reali questa informazione non è disponibile. Ad 
 * esempio, un processo potrebbe dover gestire richieste provenienti da 
 * decine di altri processi senza sapere quale terminerà per primo. MPI mette 
 * quindi a disposizione due costanti speciali chiamate wildcard:
 *
 * - MPI_ANY_SOURCE
 * - MPI_ANY_TAG
 *
 * che permettono di ricevere messaggi senza conoscere anticipatamente
 * alcune delle loro caratteristiche. 
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   /* Questo è lo schema dei tre processi:
   *
   *    Processo 0 --- tag = 100 --> Processo 2
   *    Processo 1 --- tag = 200 --> Processo 2
   *
   * Il processo 2 utilizzerà le wildcard per ricevere i messaggi e
   * MPI_Status per conoscere il mittente ed il tag effettivi. */
  if(my_rank == 0) {

    int value = 10;

    MPI_Send(
      &value,
      1,
      MPI_INT,
      2,
      100,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviato valore %d con tag 100\n", my_rank, value);
  }

  else if(my_rank == 1) {

    int value = 20;

    MPI_Send(
      &value,
      1,
      MPI_INT,
      2,
      200,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviato valore %d con tag 200\n",my_rank, value);
  }

  else if(my_rank == 2) {

    int value;
    MPI_Status status;

    /* Prima ricezione: MPI_ANY_SOURCE. */
    MPI_Recv(
      &value,
      1,
      MPI_INT,
      /* Non sappiamo quale processo invierà per primo il messaggio.
       * Accettiamo quindi messaggi provenienti da qualsiasi mittente. */
      MPI_ANY_SOURCE,
      /* Ci interessa solamente ricevere il messaggio avente tag 100. */
      100,
      MPI_COMM_WORLD,
      &status
    );

    /* Poiché abbiamo utilizzato MPI_ANY_SOURCE, il mittente effettivo è noto 
     * solamente dopo la ricezione. MPI salva questa informazione all'interno 
     * della struttura MPI_Status. */
    printf("[Processo %d] Ricevuto valore %d con MPI_ANY_SOURCE e tag 100\n", my_rank, value);
    printf("[Processo %d] Mittente reale: %d\n", my_rank, status.MPI_SOURCE);

    /* Seconda ricezione: MPI_ANY_SOURCE e MPI_ANY_TAG. */
    MPI_Recv(
      &value,
      1,
      MPI_INT,
      MPI_ANY_SOURCE,
      /* Questa volta accettiamo anche qualsiasi tag. */
      MPI_ANY_TAG,
      MPI_COMM_WORLD,
      &status
    );

    /* Poiché abbiamo utilizzato MPI_ANY_TAG, il tag effettivo è noto solamente
     * dopo la ricezione. MPI salva questa informazione all'interno  della 
     * struttura MPI_Status. */
    printf("[Processo %d] Ricevuto valore %d con MPI_ANY_SOURCE e MPI_ANY_TAG\n", my_rank, value);
    printf("[Processo %d] Mittente reale: %d\n", my_rank, status.MPI_SOURCE);
    printf("[Processo %d] Tag reale: %d\n", my_rank, status.MPI_TAG);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Quando utilizziamo:
 *
 *    source = 0
 *    tag = 10
 *
 * il programma conosce già tutte le caratteristiche del messaggio.
 *
 * Utilizzando invece le wildcard:
 *
 *    MPI_ANY_SOURCE
 *    MPI_ANY_TAG
 *
 * alcune informazioni vengono determinate solamente quando il messaggio
 * viene effettivamente ricevuto.
 *
 * MPI restituisce queste informazioni attraverso la struttura
 * MPI_Status.
 *
 * I campi principali sono:
 *
 * - status.MPI_SOURCE
 * - status.MPI_TAG
 * - status.MPI_ERROR
 * ---------------------------------------------------------------------- */
