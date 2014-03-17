/*
 * model.c
 *
 *  Created on: 22/07/2012
 *      Author: Eduardo
 */

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "model.h"
#include "rand.h"

/* ------------------------------------------------------------------------ */
/* -- Funções do modelo --*/

ModeloMarkov *
modelo_inicializa(int ordem)
{
   ModeloMarkov *modelo;
   if (ordem < 0) {
      fprintf(stderr, "erro: ordem negativa\n");
      return NULL;
   }
   if (ordem >= 20) {
      fprintf(stderr, "erro: ordem muito grande\n");
      return NULL;
   }

   modelo = aloca(ModeloMarkov, 1);
   modelo->ordem = ordem;
   modelo->estado = 0;
   modelo->mascara = (1 << ordem) - 1;
   //modelo->prob = aloca(double, 1 << ordem);
   modelo->prob = NULL;
   modelo->tempo = 0;
   return modelo;
}

void
modelo_limpa(ModeloMarkov *modelo)
{
   modelo->tempo = 0;
   modelo->estado = 0;
}

void
modelo_finaliza(ModeloMarkov *modelo)
{
   //free(modelo->prob);
   free(modelo);
}

int
modelo_evolui(ModeloMarkov *modelo)
{
   double prob, r;
   int x;

   if (modelo->tempo < modelo->ordem) {
      prob = 0;
      int i, imax = 1 << modelo->ordem;
      int mascara = modelo->mascara >> (modelo->ordem - modelo->tempo);
      /* média das probabilidades de transição que resultem
       * no estado atual */
      for (i = 0; i < imax; i++) {
         if ((i & mascara) == modelo->estado)
            prob += modelo->prob[i];
      }
      prob /= 1 << (modelo->ordem - modelo->tempo);
   }
   else
      prob = modelo->prob[modelo->estado];

   r = rand_unit();
   x = (r > prob) ? 0 : 1;

   if (modelo->ordem > 0)
      modelo->estado = modelo->mascara & ((modelo->estado << 1) | x);
   modelo->tempo++;
   return x;
}

