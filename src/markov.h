#ifndef __MARKOV_H
#define __MARKOV_H

#include "rand.h"
#include <vector>

using std::vector;

class MarkovMachine : public UsesRandom {
public:
   MarkovMachine(int order);
   void setProb(double *probs);
   void prepare();
   void clean();
   int step();

private:
   int order;
   int time;
   int state;
   int vsize;
   int mask;
   vector<double> prob;
};

#endif /* __MARKOV_H */
