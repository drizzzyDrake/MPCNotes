/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   ./31_gustafson
 *
 * Non utilizzeremo MPI perché la legge di Gustafson è un modello teorico
 * che descrive come cresce lo speedup aumentando il numero di processi
 * insieme alla dimensione del problema.
 *
 * L'obiettivo è mostrare:
 *
 * - la differenza rispetto alla legge di Amdahl;
 * - lo speedup scalato;
 * - il comportamento del Weak Scaling.
 * ---------------------------------------------------------------------- */

#include <stdio.h>

int main() {

  /* Frazione del programma che rimane sequenziale.
   *
   * Esempio:
   *
   *   alpha = 0.1
   *
   * significa che il 10% del lavoro rimane sequenziale,
   * mentre il restante 90% può essere parallelizzato.
   */
  double alpha = 0.1;

  /* Numero di processi utilizzati.
   *
   * Aumentando questo valore osserviamo
   * come cresce lo speedup secondo Gustafson. */
  int processes = 8;

  /* Secondo la legge di Gustafson:
   *
   *   S(p) = p - alpha * (p - 1)
   *
   * dove:
   *
   *   p     = numero di processi
   *   alpha = frazione sequenziale del programma
   */
  double speedup = processes - alpha * (processes - 1);


  /*
   * Per confronto calcoliamo anche lo speedup
   * ideale ottenibile.
   */
  double ideal_speedup = processes;

  printf("Frazione sequenziale: %.2f%%\n", alpha * 100);
  printf("Frazione parallelizzabile: %.2f%%\n\n", (1 - alpha) * 100);
  printf("Processi utilizzati: %d\n", processes);
  printf("Speedup secondo Gustafson: %.3f\n", speedup);
  printf("Speedup ideale: %.3f\n", ideal_speedup);

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * La legge di Gustafson descrive il comportamento del Weak Scaling.
 *
 * A differenza della legge di Amdahl:
 *
 * - Amdahl mantiene fisso il problema;
 * - Gustafson aumenta la dimensione del problema insieme ai processi.
 *
 * L'idea principale è:
 *
 *   aumentando le risorse disponibili possiamo risolvere problemi
 *   più grandi mantenendo un tempo di esecuzione simile.
 *
 * Definiamo:
 *
 *   alpha
 *
 * come la frazione sequenziale del programma.
 *
 * La parte parallelizzabile è:
 *
 *   1 - alpha
 *
 * Lo speedup secondo Gustafson è:
 *
 *   S(p) = p - alpha(p - 1)
 *
 * dove:
 *
 *   p = numero di processi.
 *
 * Esempio:
 *
 *   alpha = 0.1
 *
 *   10% sequenziale
 *   90% parallelizzabile
 *
 *   con p = 8:
 *
 *   S(8) = 8 - 0.1(7) = 7.3
 *
 * Lo speedup ottenuto è quindi molto vicino
 * al valore ideale:
 *
 *   S(p) = p
 *
 * La legge di Gustafson mostra che:
 *
 *   aumentando la dimensione del problema insieme al numero di processori
 *   è possibile ottenere una buona scalabilità.
 *
 * Confronto:
 *
 *   Amdahl    : problema fisso
 *               limite imposto dalla parte sequenziale
 *
 *   Gustafson : problema crescente
 *               speedup vicino al numero di processori
 * ---------------------------------------------------------------------- */
