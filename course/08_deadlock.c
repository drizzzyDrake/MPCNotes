/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 2 ./08_deadlock
 *
 * Utilizzeremo:
 *
 *    rank 0 --> invia e poi riceve
 *    rank 1 --> invia e poi riceve
 *
 * L'obiettivo è mostrare come lo stesso identico schema di comunicazione
 * possa funzionare con messaggi piccoli ma andare in deadlock con messaggi
 * grandi.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* Un deadlock si verifica quando due o più processi rimangono bloccati
 * aspettando un evento che non potrà mai verificarsi. Nella comunicazione
 * point-to-point un caso classico è il seguente:
 *
 * Processo 0:
 *
 *    MPI_Send(...)
 *    MPI_Recv(...)
 *
 * Processo 1:
 *
 *    MPI_Send(...)
 *    MPI_Recv(...)
 *
 * Se entrambe le MPI_Send attendono che il destinatario esegua la relativa
 * MPI_Recv, nessuno dei due processi potrà mai raggiungere la ricezione. Entrambi
 * rimarranno quindi bloccati nella MPI_Send. 
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* ESEMPIO 1 ------------------------------------------------------------
   * Messaggi piccoli:
   * In molte implementazioni MPI questo esempio termina correttamente perché
   * il protocollo Eager permette di completare la MPI_Send copiando il dato
   * in un buffer interno.
   * -------------------------------------------------------------------- */
  if(my_rank == 0) {

    int value = 10;

    MPI_Send(
      &value,
      1,
      MPI_INT,
      1,
      0,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Send completata (messaggio piccolo)\n", my_rank);

    MPI_Recv(
      &value,
      1,
      MPI_INT,
      1,
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] Ricevuto valore %d\n", my_rank, value);
  }

  else if(my_rank == 1) {

    int value = 20;

    MPI_Send(
      &value,
      1,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Send completata (messaggio piccolo)\n", my_rank);

    MPI_Recv(
      &value,
      1,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] Ricevuto valore %d\n", my_rank, value);
  }

  /* Entrambi i processi devono terminare il primo esempio prima di iniziare
   * il secondo. */
  MPI_Barrier(MPI_COMM_WORLD);

  /* ESEMPIO 2 ------------------------------------------------------------
   * Messaggi grandi:
   * Da questo punto in poi il programma potrebbe bloccarsi. È proprio questo
   * il comportamento che vogliamo osservare. Se l'implementazione MPI utilizza
   * il protocollo Rendezvous, entrambe le MPI_Send potrebbero attendere che
   * il destinatario esegua MPI_Recv, causando un deadlock. Per interrompere
   * l'esecuzione utilizzare CTRL+C.
   * -------------------------------------------------------------------- */
  int size = 1000000;

  int* values = malloc(size * sizeof(int));

  if(values == NULL) {

    printf("Errore allocazione memoria\n");

    MPI_Abort(
      MPI_COMM_WORLD,
      1
    );
  }

  if(my_rank == 0) {

    printf("\n[Processo %d] Invio messaggio grande...\n", my_rank);

    MPI_Send(
      values,
      size,
      MPI_INT,
      1,
      1,
      MPI_COMM_WORLD
    );

    /* Se compare questa stampa significa che MPI_Send è terminata.
     * In presenza di deadlock questa istruzione potrebbe non essere mai
     * eseguita. */
    printf("[Processo %d] Send completata (messaggio grande)\n", my_rank);

    MPI_Recv(
      values,
      size,
      MPI_INT,
      1,
      1,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );
  }

  else if(my_rank == 1) {

    printf("\n[Processo %d] Invio messaggio grande...\n", my_rank);

    MPI_Send(
      values,
      size,
      MPI_INT,
      0,
      1,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Send completata (messaggio grande)\n", my_rank);

    MPI_Recv(
      values,
      size,
      MPI_INT,
      0,
      1,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );
  }

  free(values);

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Lo schema:
 *
 *    Send()
 *    Recv()
 *
 * eseguito contemporaneamente da due processi NON è sempre sicuro.
 *
 * Con messaggi piccoli molte implementazioni MPI utilizzano il protocollo
 * Eager e la comunicazione può terminare correttamente.
 *
 * Con messaggi grandi può invece essere utilizzato il protocollo
 * Rendezvous.
 *
 * In questo caso entrambe le MPI_Send possono rimanere in attesa che il
 * destinatario esegua MPI_Recv.
 *
 * Si crea quindi un'attesa circolare:
 *
 *    Processo 0 aspetta il Processo 1
 *    Processo 1 aspetta il Processo 0
 *
 * Nessuno dei due processi può proseguire e il programma rimane bloccato.
 *
 * Per evitare questo problema è possibile:
 *
 * - invertire l'ordine di Send e Recv in uno dei due processi;
 * - utilizzare comunicazioni non bloccanti;
 * - utilizzare funzioni MPI che eseguono invio e ricezione
 *   con un'unica operazione.
 * ---------------------------------------------------------------------- */
