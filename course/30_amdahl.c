/* NOTE -------------------------------------------------------------------
 * Questo esempio è pensato per essere eseguito con:
 *
 *   ./30_amdahl
 *
 * Non utilizzeremo MPI perché la legge di Amdahl è un modello teorico
 * che descrive il limite massimo dello speedup ottenibile aumentando
 * il numero di processi.
 *
 * L'obiettivo è mostrare:
 *
 *   - il rapporto tra parte sequenziale e parallelizzabile;
 *   - lo speedup teorico;
 *   - il limite massimo imposto dalla parte sequenziale.
 * ---------------------------------------------------------------------- */

#include <stdio.h>

int main() {

  /* Frazione del programma che può essere parallelizzata.
   *
   * Esempio:
   *
   *      alpha = 0.8
   *
   * significa che l'80% del lavoro è parallelizzabile,
   * mentre il restante 20% è sequenziale. */
  double alpha = 0.8;

  /* Tempo di esecuzione sequenziale di riferimento.
   * Normalizziamo il problema assumendo:
   *
   *      T_serial = 1 secondo
   *
   * perché la formula dipende solamente dai rapporti
   * tra le parti del programma. */
  double serial_time = 1.0;

  /* Numero di processi utilizzati.
   *
   * Cambiando questo valore possiamo osservare
   * come cresce lo speedup. */
  int processes = 8;

  /* Secondo la legge di Amdahl:
   *
   *   T_parallel(p) = (1 - alpha) * T_serial + alpha * T_serial / p */
  double parallel_time = (1 - alpha) * serial_time + (alpha * serial_time) / processes;

  /* Speedup teorico:
   *
   *            T_serial
   *   S(p) = -------------
   *           T_parallel   */
  double speedup = serial_time / parallel_time;

  /* Limite massimo dello speedup:
   *
   *              1
   *   Smax = ---------
   *          1 - alpha
   *
   * Si ottiene quando il numero di processi tende
   * all'infinito. */
  double max_speedup = 1 / (1 - alpha);

  printf("Frazione parallelizzabile: %.2f%%\n", alpha * 100);
  printf("Frazione sequenziale: %.2f%%\n\n", (1 - alpha) * 100);
  printf("Processi utilizzati: %d\n", processes);
  printf("Tempo sequenziale: %.3f secondi\n", serial_time);
  printf("Tempo parallelo teorico: %.3f secondi\n", parallel_time);
  printf("Speedup teorico: %.3f\n", speedup);
  printf("Speedup massimo teorico: %.3f\n", max_speedup);

  return 0;
}

/* RIASSUNTO --------------------------------------------------------------
 * La legge di Amdahl descrive il limite dello Strong Scaling.
 *
 * Un programma è formato da:
 *
 * - una parte parallelizzabile;
 * - una parte sequenziale.
 *
 * Definiamo:
 *
 *   alpha
 *
 * come la frazione parallelizzabile del programma.
 *
 * Quindi:
 *
 *   1 - alpha
 *
 * rappresenta la parte sequenziale.
 *
 * Il tempo parallelo è:
 *
 *      T_parallel(p) = (1 - alpha)T_serial + alpha * T_serial / p
 *
 * Lo speedup è:
 *
 *            T_serial
 *   S(p) = -------------
 *          T_parallel(p)
 *
 * Sostituendo otteniamo:
 *
 *                    1
 *   S(p)= ------------------------
 *         (1 - alpha) + alpha / p
 *
 * Aumentando il numero di processi:
 *
 *   p aumenta
 *   la parte parallelizzabile diminuisce
 *   la parte sequenziale rimane invariata
 *
 * Per questo esiste un limite massimo:
 *
 *              1
 *   Smax = ---------
 *          1 - alpha
 *
 * Esempio:
 *
 *   alpha = 0.8
 *   80% parallelizzabile
 *   20% sequenziale
 *   Smax = 1 / 0.2 = 5
 *
 * Anche utilizzando infiniti processori lo speedup non potrà mai superare
 * questo valore.
 *
 * La legge di Amdahl mostra quindi che:
 *
 *   una piccola parte sequenziale può limitare
 *   fortemente la scalabilità di un programma.
 * ---------------------------------------------------------------------- */
