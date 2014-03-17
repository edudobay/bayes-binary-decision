/*
 * model.h
 *
 *  Created on: 22/07/2012
 *      Author: Eduardo
 */

#ifndef MODEL_H_
#define MODEL_H_

/* ------------------------------------------------------------------------ */

typedef struct _modelo_markov {
   int ordem;
   int estado;
   int mascara;
   int tempo;
   double *prob; /* probabilidades de sair 1 */
} ModeloMarkov;

/* ------------------------------------------------------------------------ */
/* -- Funções do modelo --*/

ModeloMarkov *modelo_inicializa(int ordem);
void modelo_limpa(ModeloMarkov *modelo);
void modelo_finaliza(ModeloMarkov *modelo);
int modelo_evolui(ModeloMarkov *modelo);

/* ------------------------------------------------------------------------ */

#endif /* MODEL_H_ */
