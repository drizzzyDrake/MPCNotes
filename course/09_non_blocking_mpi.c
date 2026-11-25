/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 2 ./09_non_blocking_mpi
 *
 * Utilizzeremo:
 *
 *   rank 0 --> invia un messaggio con MPI_Isend
 *   rank 1 --> riceve un messaggio con MPI_Irecv
 *
 * L'obiettivo è mostrare come le comunicazioni non bloccanti permettano di
 * separare:
 *
 * - avvio della comunicazione;
 * - completamento della comunicazione.
 *
 * A differenza di MPI_Send e MPI_Recv, le operazioni non bloccanti
 * restituiscono immediatamente il controllo al programma.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* Le operazioni MPI_Send e MPI_Recv sono bloccanti:
 *
 * il processo rimane fermo fino a quando la comunicazione non può essere
 * completata.
 *
 * MPI mette a disposizione due operazioni non bloccanti:
 *
 *   MPI_Isend
 *   MPI_Irecv
 *
 * Queste funzioni avviano la comunicazione ma non aspettano il suo termine.
 * Per sapere quando la comunicazione è conclusa si utilizza MPI_Wait. Ogni
 * comunicazione non bloccante è associata ad una MPI_Request.
 *
 * MPI_Request rappresenta una comunicazione in corso e permette a MPI di
 * tenere traccia del suo stato.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if(my_rank == 0) {

    int size = 1000000;

    int* values = malloc(size * sizeof(int));

    for(int i = 0; i < size; i++) {
      values[i] = i;
    }

    /* MPI_Request rappresenta una comunicazione non bloccante in corso. Quando
     * chiamiamo MPI_Isend o MPI_Irecv, MPI avvia la comunicazione ma non aspetta
     * che venga completata. La variabile MPI_Request permette a MPI di identificare
     * questa operazione e controllarne successivamente lo stato tramite funzioni
     * come MPI_Wait.
     */
    MPI_Request request;

    /* MPI_Isend avvia l'invio del messaggio ma non aspetta che il
     * destinatario abbia completato la ricezione. Dopo questa chiamata il
     * processo può continuare ad eseguire altre operazioni.
     */
    MPI_Isend(
      values,
      size,
      MPI_INT,
      1,
      0,
      MPI_COMM_WORLD,
      /* &request passa l'indirizzo della variabile request. MPI_Isend deve
       * modificare la variabile request per memorizzare l'identificativo della
       * comunicazione appena avviata. Successivamente questo identificativo
       * verrà utilizzato da MPI_Wait per sapere quale comunicazione deve essere
       * completata.
       */
      &request
    );
    printf("[Processo %d] MPI_Isend avviata (invio di %d elementi)\n", my_rank, size);

    /* Qui il processo potrebbe eseguire altre operazioni indipendenti dalla
     * comunicazione. Per semplicità mostriamo solamente una stampa.
     */
    printf("[Processo %d] Eseguo altre operazioni mentre la comunicazione procede\n", my_rank);
    printf("[Processo %d] Attendo il completamento della comunicazione con MPI_Wait\n", my_rank);

    /* MPI_Wait blocca il processo fino al completamento della comunicazione.
     * Dopo questa chiamata il buffer può essere riutilizzato.
     */
    MPI_Wait(
      &request,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] MPI_Isend completata\n", my_rank);

    free(values);
  }

  else if(my_rank == 1) {

    int size = 1000000;

    int* values = malloc(size * sizeof(int));

    MPI_Request request;

    /* MPI_Irecv avvia una ricezione non bloccante. Anche in questo caso la
     * funzione ritorna immediatamente e la ricezione viene completata
     * successivamente tramite MPI_Wait.
     */
    MPI_Irecv(
      values,
      size,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD,
      &request
    );

    printf("[Processo %d] MPI_Irecv avviata (attendo %d elementi)\n", my_rank, size);
    printf("[Processo %d] Posso eseguire altre operazioni mentre aspetto il messaggio\n", my_rank);
    printf("[Processo %d] Attendo il completamento della ricezione con MPI_Wait\n", my_rank);

    /* Attendiamo che il messaggio sia effettivamente arrivato. Dopo MPI_Wait
     * i valore ricevuti sono disponibili nel buffer.
     */
     MPI_Wait(
      &request,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] Ricevuto messaggio (%d elementi)\n", my_rank, size);

    free(values);
  }

  MPI_Finalize();

  return 0;
}
/* RIASSUNTO --------------------------------------------------------------
 * Le comunicazioni bloccanti:
 *
 *   MPI_Send
 *   MPI_Recv
 *
 * fermano il processo fino al completamento dell'operazione. 
 *
 * Le comunicazioni non bloccanti:
 *
 *   MPI_Isend
 *   MPI_Irecv
 *
 * avviano la comunicazione e restituiscono immediatamente il controllo al
 * programma.
 *
 * Ogni operazione non bloccante restituisce una MPI_Request che identifica
 * la comunicazione in corso.
 *
 * Per completare la comunicazione si utilizza:
 *
 *   MPI_Wait
 *
 * Vantaggi principali:
 *
 * - possibilità di sovrapporre comunicazione e computazione;
 * - riduzione dei tempi di attesa;
 * - possibilità di evitare alcuni deadlock causati dall'ordine delle
 *   operazioni bloccanti.
 *
 * Tuttavia è necessario ricordare che il buffer utilizzato in una
 * comunicazione non bloccante non deve essere modificato fino al completamento
 * della comunicazione tramite MPI_Wait.
 * ---------------------------------------------------------------------- */
