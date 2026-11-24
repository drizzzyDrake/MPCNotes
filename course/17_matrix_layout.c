/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *    .\17_matrix_layout
 *
 * Mostriamo due modi diversi di allocare una matrice dinamica.
 *
 * 1) int**  -> ogni riga viene allocata separatamente.
 * 2) int*   -> tutta la matrice occupa un unico blocco contiguo.
 *
 * L'obiettivo è mostrare perché le comunicazioni collettive MPI richiedono
 * generalmente memoria contigua.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

int main() {

  int rows = 3;
  int cols = 4;

  printf("\nMATRICE NON CONTIGUA:\n\n");

  /* Ogni riga viene allocata separatamente. */
  int** matrix = malloc(rows * sizeof(int*));

  for(int i = 0; i < rows; i++) {
    matrix[i] = malloc(cols * sizeof(int));
  }

  for(int i = 0; i < rows; i++) {
    printf("matrix[%d] = %p\n", i, (void*)matrix[i]);
  }
 
  printf("\nMATRICE CONTIGUA:\n\n");

  /* Tutta la matrice occupa un unico blocco di memoria. */
  int* matrix_contiguous = malloc(rows * cols * sizeof(int));

  for(int i = 0; i < rows; i++) {
    printf("riga %d = %p\n", i, (void*)&matrix_contiguous[i * cols]);
  }

  free(matrix_contiguous);

  for(int i = 0; i < rows; i++) {
    free(matrix[i]);
  }

  free(matrix);

  return 0;
}

/* RIASSUNTO ---------------------------------------------------------------
 * Allocazione con int**:
 *
 * - ogni riga viene allocata separatamente;
 * - non è garantito che le righe siano contigue in memoria.
 *
 * Allocazione con int*:
 *
 * - tutta la matrice occupa un unico blocco contiguo.
 *
 * Le comunicazioni collettive MPI lavorano normalmente su blocchi
 * continui di memoria.
 *
 * Per questo motivo una matrice allocata con un unico malloc è
 * generalmente la soluzione corretta.
 * ----------------------------------------------------------------------- */
