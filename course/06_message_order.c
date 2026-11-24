/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 2 ./06_message_order
 *
 * Utilizzeremo:
 *
 *    rank 0 --> invia due messaggi consecutivi
 *    rank 1 --> riceve i due messaggi
 *
 * L'obiettivo è mostrare la proprietà di ordinamento dei messaggi MPI,
 * chiamata non-overtaking property.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* MPI garantisce una proprietà importante sull'ordine dei messaggi.
 *
 * Se un processo invia più messaggi allo stesso destinatario utilizzando:
 *
 * - lo stesso communicator;
 * - lo stesso tag;
 * - lo stesso tipo di dato;
 *
 * il destinatario riceverà i messaggi nello stesso ordine con cui sono
 * stati inviati. Ad esempio:
 *
 * Processo 0:
 *
 *    Send(A)
 *    Send(B)
 *
 * Processo 1:
 *
 *    Recv()
 *    Recv()
 *
 * La prima ricezione restituirà sempre A e la seconda sempre B. MPI non 
 * permette che il secondo messaggio "sorpassi" il primo. Questa proprietà 
 * è chiamata non-overtaking property.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* Questo esempio richiede esattamente due processi:
   *
   *    Processo 0 -----------------> Processo 1
   *
   *    invia due messaggi            riceve due messaggi
   *
   * Entrambi i messaggi utilizzano lo stesso tag. L'ordine di ricezione dipende 
   * quindi dalla proprietà non-overtaking di MPI. */

  if(my_rank == 0) {

    int first_value = 10;
    int second_value = 20;

    /* Primo messaggio inviato. Questo messaggio viene inserito nella sequenza 
     * di comunicazione prima del secondo messaggio. */
    MPI_Send(
      &first_value,
      1,
      MPI_INT,
      1,
      0,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Primo messaggio inviato: %d\n", my_rank, first_value);

    /* Secondo messaggio inviato. Utilizza lo stesso:
     *
     * - destinatario;
     * - tag;
     * - tipo di dato.
     *
     * Per questo motivo MPI deve mantenere l'ordine rispetto al primo messaggio. */
    MPI_Send(
      &second_value,
      1,
      MPI_INT,
      1,
      0,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Secondo messaggio inviato: %d\n", my_rank, second_value);
  }

  else if(my_rank == 1) {

    int value;

    /* Prima ricezione. Nonostante non specifichiamo quale messaggio ricevere, 
     * MPI sceglierà il primo messaggio inviato dal processo 0 compatibile con 
     * questa ricezione. */
    MPI_Recv(
      &value,
      1,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] Primo valore ricevuto: %d\n", my_rank, value);

    /* Seconda ricezione. Grazie alla non-overtaking property questo messaggio 
     * sarà il secondo inviato dal processo 0. */
    MPI_Recv(
      &value,
      1,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    printf("[Processo %d] Secondo valore ricevuto: %d\n", my_rank, value);
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * La non-overtaking property garantisce che:
 *
 *    stesso mittente
 *    stesso destinatario
 *    stesso communicator
 *    stesso tag
 *
 * mantengano l'ordine dei messaggi.
 *
 * Esempio:
 *
 * Processo 0:
 *
 *    Send(A)
 *    Send(B)
 *
 * Processo 1:
 *
 *    Receive()
 *    Receive()
 *
 * Risultato garantito:
 *
 *    Receive() -> A
 *    Receive() -> B
 *
 * Attenzione: MPI NON garantisce l'ordine tra messaggi inviati da processi
 * DIFFERENTI.
 *
 * Esempio:
 *
 *    Processo 0 -----
 *                    *
 *                    * ---> Processo 2
 *                    *
 *    Processo 1 -----*
 *
 * Il processo 2 potrebbe ricevere prima il messaggio del processo 0 oppure
 * quello del processo 1, perché dipende dall'esecuzione del sistema.
 * ---------------------------------------------------------------------- */
