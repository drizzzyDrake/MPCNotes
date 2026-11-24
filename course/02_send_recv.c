/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 2 ./02_send_recv
 *
 * Utilizzeremo:
 *
 *    rank 0 --> mittente
 *    rank 1 --> destinatario
 *
 * Gli eventuali altri processi non svolgeranno alcuna operazione.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* In un sistema a memoria distribuita ogni processo possiede una memoria
 * privata completamente separata da quella degli altri processi. Una normale 
 * assegnazione: x = y, può avvenire SOLO all'interno dello stesso processo. 
 * Non è invece possibile fare qualcosa del tipo: processo0.x = processo1.y, 
 * poiché i due processi non condividono alcuna memoria. Per scambiarsi 
 * informazioni devono quindi inviare messaggi. 
 */
int main(int argc, char* argv[]) {
  
  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if(my_rank == 0) {

    /* Il processo 0 possiede una variabile locale. Questa variabile esiste 
     * ESCLUSIVAMENTE nella memoria del processo 0. Nessun altro processo può 
     * leggerla direttamente. */
    int value = 42;

    printf("[Processo %d] Valore iniziale = %d\n",my_rank, value);

    /* MPI_Send invia un messaggio al processo avente rank specificato.
     * IMPORTANTE: MPI NON invia "variabili". MPI legge una certa regione 
     * di memoria (buffer), costruisce un messaggio e lo trasferisce al
     * destinatario. */
    MPI_Send(

      /* Indirizzo del buffer contenente i dati. Il simbolo '&' restituisce 
       * l'indirizzo di memoria della variabile. MPI inizierà a leggere proprio 
       * da questo indirizzo. */
      &value,

      /* Numero di elementi da inviare. ATTENZIONE: NON rappresenta il numero
       * di byte. Significa semplicemente: "Invia un elemento." */
      1,

      /* Tipo degli elementi contenuti nel buffer. MPI usa questa informazione
       * per sapere come interpretare i dati presenti in memoria e quanti byte
       * occupa ciascun elemento. */
      MPI_INT,

      /* Rank del destinatario. Solo il processo avente rank 1 riceverà questo
       * messaggio. */
      1,

      /* Tag del messaggio. È un identificatore scelto dal programmatore.
       * Permette al destinatario di distinguere messaggi appartenenti a 
       * comunicazioni differenti. */
      0,

      /*  Communicator in cui avviene la comunicazione. (vedi 01_communicator.c) */
      MPI_COMM_WORLD);

      /* Quando MPI_Send termina NON significa necessariamente che il processo 
       * destinatario abbia già ricevuto il dato. L'unica garanzia fornita 
       * dallo standard è che il buffer mittente può essere riutilizzato o 
       * modificato. L'implementazione MPI deciderà autonomamente se usare
       * protocollo Eager oppure Rendezvous. */
      printf("[Processo %d] Invio completato.\n", my_rank);
    }

    else if(my_rank == 1) {

    /* Il processo destinatario possiede una propria variabile. All'inizio 
     * contiene un valore arbitrario.*/
    int value = -1;

    printf("[Processo %d] Prima della ricezione = %d\n", my_rank, value);

    /* Struttura che MPI riempirà automaticamente con informazioni sul messaggio 
     * ricevuto. */
    MPI_Status status;

    /* MPI_Recv riceve un messaggio proveniente da un altro processo. Se il
     * messaggio richiesto non è ancora disponibile, questa funzione rimane
     * BLOCCATA finché il messaggio non arriva. */
    MPI_Recv(

      /* Buffer di destinazione. Qui verranno copiati i dati ricevuti. */
      &value,

      /* Capacità del buffer. Il valore rappresenta il numero massimo di elementi 
       * che il buffer può contenere. NON indica quanti elementi verranno realmente
       * ricevuti. */
      1,

      /* Tipo atteso */
      MPI_INT,

      /* Mittente atteso. In questo esempio vogliamo ricevere solamente messaggi
       * provenienti dal processo 0. In alternativa sarebbe possibile usare:
       * MPI_ANY_SOURCE per accettare messaggi da qualsiasi processo. */
      0,

      /* Tag atteso */
      0,

      /* Communicator. * Mittente e destinatario devono appartenere allo stesso 
       * communicator. */
      MPI_COMM_WORLD,

      /* Al termine della ricezione questa struttura conterrà informazioni sul
       * messaggio realmente ricevuto. */
      &status);

      printf("[Processo %d] Dopo la ricezione = %d\n", my_rank, value);

      /* La struttura status permette di conoscere l'origine reale del messaggio. */
      printf("[Processo %d] Mittente = %d\n", my_rank, status.MPI_SOURCE);
      printf("[Processo %d] Tag = %d\n", my_rank, status.MPI_TAG);
    }

    else {

      /* Tutti gli altri processi non fanno nulla. */
      printf("[Processo %d] Nessuna operazione.\n", my_rank);
    }

  MPI_Finalize();

  return 0;
}

/* TIPI MPI ---------------------------------------------------------------
 * Alcuni dei tipi di dato predefiniti forniti da MPI.
 *
 *    +----------------------+------------------------+
 *    + Tipo MPI             + Tipo C                 +
 *    +----------------------+------------------------+
 *    + MPI_CHAR             + signed char            +
 *    + MPI_INT              + signed int             +
 *    + MPI_FLOAT            + float                  +
 *    + MPI_DOUBLE           + double                 +
 *    + MPI_LONG             + signed long int        +
 *    + MPI_UNSIGNED         + unsigned int           +
 *    + MPI_BYTE             + byte non interpretato  +
 *    + MPI_PACKED           + dati impacchettati     +
 *    +----------------------+------------------------+
 *
 * MPI utilizza questi tipi per garantire la portabilità tra architetture
 * differenti e per conoscere la rappresentazione dei dati in memoria. 
 * --------------------------------------------------------------------- */

/* RIASSUNTO --------------------------------------------------------------
 *
 * MPI_Send(...)
 * - legge i dati dal buffer mittente;
 * - costruisce un messaggio;
 * - lo invia al destinatario.
 *
 * MPI_Recv(...)
 *  - attende il messaggio richiesto;
 *  - copia i dati nel buffer locale;
 *  - restituisce informazioni sul messaggio tramite MPI_Status.
 *
 * Fondamentale:
 * Ogni processo possiede SEMPRE una memoria privata. I dati non vengono 
 * condivisi ma vengono copiati da un processo all'altro mediante lo scambio
 * di messaggi.
 *
 * MPI_Send e MPI_Recv devono "incontrarsi":
 * Il mittente invia un messaggio destinato ad uno specifico processo,
 * mentre il destinatario deve eseguire una MPI_Recv compatibile
 * (stesso communicator, stesso tipo di dato e, salvo wildcard, stesso 
 * mittente e stesso tag).
 *
 * Questo meccanismo prende il nome di message matching.
 * ---------------------------------------------------------------------- */

