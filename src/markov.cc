#include "markov.h"
#include "rand.h"

MarkovMachine::MarkovMachine(int order)
   : UsesRandom(), order(order), time(0), state(0),
     vsize(1 << order), mask(vsize - 1),
     prob(vsize, 0.5)
{
}

void MarkovMachine::setProb(double *probs)
{
   prob.assign(probs, probs + vsize);
}

void MarkovMachine::prepare()
{
   if (order == 0) return;
   for (int j = 0; j < order; j++) {
      int jmask = mask >> (order - j);
      double jprob = 0;
      for (int i = 0; i < vsize; i++) {
         if ((i & jmask) == state)
            jprob += prob[i];
      }
      jprob /= (1 << (order - j));

      int x = rand->gen_01(jprob);
      state = mask & ((state << 1) | x);
   }

   // generate (and ignore) first numbers to eliminate a "transient"
   for (int j = 0; j < order; j++) {
      int x = rand->gen_01(prob[state]);
      state = mask & ((state << 1) | x);
   }
}

void MarkovMachine::clean()
{
   time = 0;
   state = 0;
}

int MarkovMachine::step()
{
   int x = rand->gen_01(prob[state]);
   if (order > 0)
      state = mask & ((state << 1) | x);
   time++;
   return x;
}

