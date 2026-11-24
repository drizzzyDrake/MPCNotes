/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    mpiexec -n 4 ./19_matrix_vector_product
 *
 * Utilizzeremo:
 *
 *    rank 0 --> possiede matrice A e vettore x completi
 *    tutti i processi --> calcolano alcune righe del prodotto
 *    rank 0 --> raccoglie il vettore risultato y
 *
 * L'obiettivo è mostrare come parallelizzare il prodotto:
 *
 *    y = A * x
 *
 * utilizzando:
 *
 *    MPI_Bcast
 *    MPI_Scatter
 *    MPI_Gather
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <mpi.h>

/* Il prodotto matrice-vettore consiste nel calcolare:
 *
 *    y[i] = A[i][0]*x[0] + A[i][1]*x[1] + ...
 *
 * Ogni processo riceve alcune righe della matrice A.
 *
 * Il vettore x viene invece copiato su tutti i processi perché
 * ogni riga necessita di tutti gli elementi del vettore.
 */
int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);

  int my_rank;
  int comm_sz;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  int rows = 4;
  int cols = 4;

  int A[4][4];
  int x[4];
  int y[4];

  if(my_rank == 0) {
    int value = 1;
    
    for(int i = 0; i < rows; i++) {
      
      for(int j = 0; j < cols; j++) {
        A[i][j] = value++;
      }
    }
    
    for(int i = 0; i < cols; i++) {
      x[i] = 1;
    }

    printf("[Processo 0] Matrice A:\n");

    for(int i = 0; i < rows; i++) {

      for(int j = 0; j < cols; j++) {
        printf("%4d", A[i][j]);
      }
      printf("\n");
    }

    printf("\n[Processo 0] Vettore x: ");

    for(int i = 0; i < cols; i++) {
      printf("%d ", x[i]);
    }

    printf("\n\n");
  }

  /* Codice per tutti i processi:
   * Distribuzione del vettore x a tutti i processi. Ogni processo deve
   * conoscere tutto x perché ogni riga della matrice richiede tutti gli
   * elementi del vettore. */
  MPI_Bcast(
    x,
    cols,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  int local_rows = rows / comm_sz;
  int local_A[local_rows][cols];
  int local_y[local_rows];

  /* Ogni processo riceve alcune righe della matrice A. */
  MPI_Scatter(
    A,
    local_rows * cols,
    MPI_INT,
    local_A,
    local_rows * cols,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  /* Calcolo locale del prodotto matrice-vettore. */
  for(int i = 0; i < local_rows; i++) {
    local_y[i] = 0;

    for(int j = 0; j < cols; j++) {
      local_y[i] += local_A[i][j] * x[j];
    }
  }

  printf("[Processo %d] Risultato locale: ", my_rank);

  for(int i = 0; i < local_rows; i++) {
    printf("%d ", local_y[i]);
  }

  printf("\n");

  /* Raccolta dei risultati sul processo root.*/
  MPI_Gather(
    local_y,
    local_rows,
    MPI_INT,
    y,
    local_rows,
    MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  if(my_rank == 0) {

    printf("\n[Processo 0] Risultato finale y: ");

    for(int i = 0; i < rows; i++) {
      printf("%d ", y[i]);
    }

    printf("\n");
  }

  MPI_Finalize();

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * Per parallelizzare y = A*x:
 * 
 * - il vettore x viene distribuito con MPI_Bcast perché ogni processo
 *   necessita di tutti gli elementi di x;
 * - la matrice A viene divisa con MPI_Scatter perché ogni processo deve
 *   lavorare solo su alcune righe;
 * - ogni processo esegue il calcolo locale: y_local = A_local * x;
 * - i risultati vengono raccolti con MPI_Gather.
 *
 * Questo schema è molto utilizzato negli algoritmi paralleli:
 *
 *    distribuzione dati --> computazione locale --> raccolta risultati
 * ---------------------------------------------------------------------- */
