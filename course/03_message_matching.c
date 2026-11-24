/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 3 ./03_message_matching
 *
 * Utilizzeremo:
 *
 *    rank 0 --> invia un messaggio con tag 10
 *    rank 1 --> invia un messaggio con tag 20
 *    rank 2 --> riceve i messaggi
 *
 * L'obiettivo è mostrare come MPI stabilisce quale messaggio deve essere
 * associato ad una determinata MPI_Recv.
 *
 * Questo meccanismo prende il nome di message matching.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* In MPI un messaggio inviato tramite MPI_Send non viene consegnato
 * automaticamente alla prima MPI_Recv disponibile.
 *
 * Il destinatario deve eseguire una ricezione compatibile con il messaggio
 * inviato.
 *
 * MPI utilizza quindi un meccanismo chiamato message matching, che confronta
 * le informazioni contenute nella MPI_Recv con quelle associate al messaggio.
 *
 * Affinché una ricezione abbia successo devono essere compatibili:
 *
 * - communicator;
 * - tipo di dato;
 * - dimensione del buffer;
 * - source (mittente);
 * - tag. 
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* Questo è lo schema dei tre processi:
   *
   *    Processo 0 --- tag = 10 --> Processo 2
   *    Processo 1 --- tag = 20 --> Processo 2
   *
   * Il processo 2 dovrà distinguere i due messaggi utilizzando il meccanismo
   * di matching. */
  if(my_rank == 0) {

    int value = 10;

    /* Processo 0 --- tag = 10 --> Processo 2 */
    MPI_Send(
      &value,
      1,
      MPI_INT,
      2,
      10,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviato valore %d con tag 10\n", my_rank, value);
  }

  else if(my_rank == 1) {

    int value = 20;

    /* Processo 1 --- tag = 20 --> Processo 2 */
    MPI_Send(
      &value,
      1,
      MPI_INT,
      2,
      20,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviato valore %d con tag 20\n",my_rank, value);
  }

  else if(my_rank == 2) {

    int value;
    MPI_Status status;

    /* Prima ricezione specifica: source = 0 e tag = 10. Se il processo 0 non 
     * ha ancora inviato il messaggio corretto, questa chiamata rimane bloccata
     * anche se sono disponibili altri messaggi. */
    MPI_Recv(
      &value,
      1,
      MPI_INT,
      0,
      10,
      MPI_COMM_WORLD,
      &status
    );

    printf("[Processo %d] Ricevuto valore %d\n", my_rank, value);
    printf("[Processo %d] Mittente: %d\n", my_rank, status.MPI_SOURCE);
    printf("[Processo %d] Tag: %d\n", my_rank, status.MPI_TAG);

    /* Prima ricezione specifica: source = 1 e tag = 20. Se il processo 1 non 
     * ha ancora inviato il messaggio corretto, questa chiamata rimane bloccata
     * anche se sono disponibili altri messaggi. */
    MPI_Recv(
      &value,
      1,
      MPI_INT,
      1,
      20,
      MPI_COMM_WORLD,
      &status
    );

    printf("[Processo %d] Ricevuto valore %d\n", my_rank, value);
    printf("[Processo %d] Mittente: %d\n", my_rank, status.MPI_SOURCE);
    printf("[Processo %d] Tag: %d\n", my_rank, status.MPI_TAG);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Ogni messaggio MPI contiene alcune informazioni utilizzate per 
 * identificarlo:
 *
 * - communicator;
 * - rank del mittente (source);
 * - tag;
 * - tipo di dato;
 *
 * Quando un processo esegue MPI_Recv, MPI cerca un messaggio compatibile
 * confrontando queste informazioni.
 *
 * Esempio:
 *
 *    MPI_Recv(
 *      buffer,
 *      count,
 *      MPI_INT,
 *      0,
 *      10,
 *      MPI_COMM_WORLD,
 *      &status
 *    );
 *
 * può ricevere solamente un messaggio:
 *
 *    - inviato dal processo 0;
 *    - con tag 10;
 *    - appartenente a MPI_COMM_WORLD;
 *    - contenente dati compatibili con MPI_INT.
 *
 * Se nessun messaggio compatibile è disponibile, MPI_Recv rimane bloccata
 * fino a quando il messaggio corretto non arriva. Messaggi ricevuti da MPI 
 * ma non ancora associati ad una MPI_Recv vengono temporaneamente mantenuti 
 * da MPI in una coda chiamata unexpected queue.
 * ---------------------------------------------------------------------- */
