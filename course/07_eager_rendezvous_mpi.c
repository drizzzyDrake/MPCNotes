/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   mpiexec -n 2 ./07_eager_rendezvous_mpi
 *
 * Utilizzeremo:
 *
 *   rank 0 --> invia due messaggi di dimensione differente
 *   rank 1 --> riceve entrambi i messaggi
 *
 * L'obiettivo è mostrare che MPI_Send può utilizzare internamente protocolli
 * differenti in base alla dimensione del messaggio:
 *
 * - Eager protocol
 * - Rendezvous protocol
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* Quando MPI_Send termina, NON significa necessariamente che il messaggio sia
 * già stato ricevuto dal destinatario.
 *
 * L'unica garanzia fornita da MPI è che il buffer utilizzato per l'invio può
 * essere riutilizzato o modificato dopo il ritorno della funzione.
 *
 * L'implementazione MPI decide internamente come gestire il trasferimento.
 *
 * Per bilanciare prestazioni e consumo di memoria vengono utilizzati due
 * protocolli principali:
 * 
 * EAGER PROTOCOL:
 * Utilizzato generalmente per messaggi piccoli. Il mittente copia il contenuto
 * del buffer in un buffer interno gestito da MPI e può terminare MPI_Send 
 * senza aspettare che il destinatario abbia eseguito MPI_Recv.
 *
 * RENDEZVOUS PROTOCOL:
 * Utilizzato generalmente per messaggi grandi. Prima del trasferimento dei 
 * dati vengono scambiati messaggi di controllo tra mittente e destinatario 
 * per verificare che il ricevente sia pronto.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* Questo esempio richiede esattamente due processi:
   *
   *   Processo 0 -----------------> Processo 1
   *
   *   messaggio piccolo             ricezione
   *
   *   messaggio grande              ricezione
   *
   * La scelta del protocollo utilizzato da MPI dipende dalla dimensione del
   * messaggio e dalla soglia interna chiamata EAGER THRESHOLD.
   */
  if(my_rank == 0) {

    int small_value = 42;

    /* Messaggio piccolo: per messaggi di dimensione ridotta MPI può utilizzare 
     * il protocollo Eager. Internamente il messaggio viene copiato in un buffer 
     * gestito da MPI. In questo modo il mittente può proseguire senza aspettare
     * che il destinatario abbia già chiamato MPI_Recv. 
     */
    MPI_Send(
      &small_value,
      1,
      MPI_INT,
      1,
      0,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviato messaggio piccolo: %d\n", my_rank, small_value);

    /* Messaggio grande: creiamo un array molto più grande rispetto al messaggio 
     * precedente. La dimensione è volutamente elevata per rappresentare un messaggio
     * che potrebbe superare la eager threshold.
     */
    int size = 1000000;
    int* large_values = malloc(size * sizeof(int));

    for(int i = 0; i < size; i++) {
      large_values[i] = i;
    }

    /* Per messaggi grandi MPI può utilizzare il protocollo Rendezvous.
     *
     * Prima del trasferimento dei dati vengono scambiati messaggi di
     * controllo:
     *
     *   RTS (Request To Send)
     *
     * inviato dal mittente per chiedere il permesso di trasferire i dati.
     *
     *   CTS (Clear To Send)
     *
     * inviato dal destinatario quando è pronto alla ricezione.
     *
     * Solo dopo questo handshake viene effettuato il trasferimento vero
     * e proprio.
     */
    MPI_Send(
      large_values,
      size,
      MPI_INT,
      1,
      1,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviato messaggio grande (%d elementi)\n", my_rank, size);

    free(large_values);
  }

  else if(my_rank == 1) {

    int small_value;

   /* Riceviamo il messaggio piccolo inviato dal processo 0. Se il mittente 
    * ha utilizzato il protocollo Eager, è possibile che il messaggio sia già 
    * stato copiato in un buffer interno di MPI prima ancora che questa MPI_Recv
    * venga eseguita. Questo comportamento dipende dall'implementazione MPI e 
    * non è garantito dallo standard.
    */
    MPI_Recv(
      &small_value,
      1,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] Ricevuto messaggio piccolo: %d\n", my_rank, small_value);

    /* Prepariamo un buffer sufficientemente grande per contenere il secondo
     * messaggio. La dimensione del buffer deve essere almeno pari al numero 
     * di elementi che il mittente potrebbe inviare.
     */
    int size = 1000000;
    int* large_values = malloc(size * sizeof(int));

    /* Riceviamo il messaggio grande. Se il messaggio supera la eager threshold, 
     * molte implementazioni MPI possono utilizzare il protocollo Rendezvous. 
     * In questo caso il trasferimento dei dati può iniziare solamente quando
     * il processo destinatario è pronto ad eseguire questa MPI_Recv. 
     */
    MPI_Recv(
      large_values,
      size,
      MPI_INT,
      0,
      1,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] Ricevuto messaggio grande (%d elementi)\n", my_rank, size);

    free(large_values);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * MPI_Send non utilizza necessariamente sempre lo stesso comportamento
 * interno.
 *
 * EAGER PROTOCOL:
 * Utilizzato per messaggi piccoli. Il mittente:
 *
 *   1) copia i dati in un buffer interno MPI;
 *   2) termina MPI_Send;
 *   3) il destinatario riceverà il messaggio successivamente.
 *
 * Vantaggi:
 *
 * - bassa latenza;
 * - non richiede che il ricevente sia già pronto.
 *
 * Svantaggi:
 *
 * - richiede memoria aggiuntiva per i buffer interni.
 *
 * RENDEZVOUS PROTOCOL:
 * Utilizzato per messaggi grandi. Prima del trasferimento vengono scambiati 
 * messaggi di controllo:
 *
 *   RTS (Request To Send)
 *   CTS (Clear To Send)
 *
 * Vantaggi:
 *
 * - evita copie inutili;
 * - migliore efficienza per grandi quantità di dati.
 *
 * Svantaggi:
 *
 * - richiede un handshake aggiuntivo.
 *
 * La scelta del protocollo dipende dall'implementazione MPI e dalla soglia
 * eager threshold.
 * ---------------------------------------------------------------------- */
