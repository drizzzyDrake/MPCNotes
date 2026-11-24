/* NOTE --------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 2 ./05_get_count
 *
 * Utilizzeremo:
 *
 *    rank 0 --> invia un array di interi
 *    rank 1 --> riceve l'array e determina quanti elementi sono arrivati
 *
 * L'obiettivo è mostrare come utilizzare MPI_Get_count per conoscere il 
 * numero effettivo di elementi ricevuti quando la dimensione del messaggio
 * non è nota a priori.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* In una comunicazione MPI il buffer specificato in MPI_Recv rappresenta
 * solamente la capacità massima dello spazio disponibile per ricevere dati.
 *
 * Il valore passato come secondo parametro di MPI_Recv NON indica quanti
 * elementi verranno necessariamente ricevuti.
 *
 * Ad esempio:
 *
 *    MPI_Recv(buffer, 10, MPI_INT, ...)
 *
 * significa:
 *
 *    "Ho spazio per ricevere al massimo 10 interi."
 *
 * Il mittente potrebbe però inviarne solamente 5.
 *
 * Per conoscere quanti elementi sono stati realmente ricevuti si utilizza
 * MPI_Get_count sulla struttura MPI_Status restituita da MPI_Recv.
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
   *    invia 5 interi                riceve il messaggio
   *
   * Il processo 1 possiede un buffer più grande del necessario e utilizzerà
   * MPI_Get_count per sapere quanti elementi sono effettivamente arrivati. */
  if(my_rank == 0) {

    int values[5] = {10, 20, 30, 40, 50};

    /* Il processo 0 invia un array composto da 5 elementi. Il secondo parametro
     * di MPI_Send indica proprio il numero di elementi contenuti nel buffer, 
     * non la dimensione in byte. */
    MPI_Send(
      values,
      5,
      MPI_INT,
      1,
      0,
      MPI_COMM_WORLD
    );

    printf("[Processo %d] Inviati 5 elementi\n", my_rank);
  }

  else if(my_rank == 1) {

    int buffer[10];
    MPI_Status status;

    /* Il processo 1 prepara un buffer capace di contenere 10 interi. Tuttavia 
     * il mittente potrebbe inviare una quantità differente. Il parametro 10 
     * indica solamente la capacità massima del buffer. Non significa che verranno 
     * ricevuti obbligatoriamente 10 elementi. */
    MPI_Recv(
      buffer,
      10,
      MPI_INT,
      0,
      0,
      MPI_COMM_WORLD,
      &status
    );

    int count;

    /* MPI_Get_count permette di conoscere il numero reale di elementi
     * ricevuti. Utilizza:
     *
     * - MPI_Status ottenuto dalla MPI_Recv;
     * - il tipo di dato ricevuto;
     *
     * e restituisce nella variabile count il numero effettivo di elementi. */
    MPI_Get_count(
      &status,
      MPI_INT,
      &count
    );

    printf("[Processo %d] Numero elementi ricevuti: %d\n", my_rank, count);

    /* Ora sappiamo che il messaggio conteneva solamente 'count' elementi.
     * Possiamo quindi utilizzare questo valore per accedere solamente alla
     * parte valida del buffer. */
    printf("[Processo %d] Valori ricevuti: ", my_rank);

    for(int i = 0; i < count; i++) {
      printf("%d ", buffer[i]);
    }

    printf("\n");
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 *
 * Il secondo parametro di MPI_Recv rappresenta la dimensione massima del
 * buffer disponibile, non il numero effettivo di elementi ricevuti.
 *
 * Esempio:
 *
 *    MPI_Recv(buffer, 10, MPI_INT, ...)
 *
 * significa:
 *
 *    "Il buffer può contenere fino a 10 interi."
 *
 * Il mittente potrebbe però inviare:
 *
 *    - 3 elementi;
 *    - 5 elementi;
 *    - 10 elementi.
 *
 * Per conoscere la quantità reale ricevuta si utilizza:
 *
 *    MPI_Get_count()
 *
 * che analizza la struttura MPI_Status prodotta da MPI_Recv.
 * ---------------------------------------------------------------------- */
