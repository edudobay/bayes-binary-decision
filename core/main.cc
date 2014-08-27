#include "agent.h"
#include "markov.h"
#include "runner.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
   double probs[] = {0.3};
   RunInfo info = {
      .agentClass = "X",
      .agentOrder = 1,
      .machineOrder = 0,
      .machineProbs = probs,
      .nRuns = 10,
   };
   // Setup random generators
   shared_ptr<Random> machineRand = make_shared<Random> (),
      agentRand = make_shared<Random> ();
   machineRand->seed();
   agentRand->seed(machineRand->get_x());

   (void) agentRand->gen_unit();

   // Setup machine
   MarkovMachine markov(info.machineOrder);
   markov.setRand(machineRand);
   markov.setProb(info.machineProbs);
   markov.prepare();

   // Setup agent
   unique_ptr<Agent> agent(Agent::create(info.agentClass, info.agentOrder));
   agent->setRand(agentRand);

   int y = agent->respond();
   cout << y << "\n";

   return 0;
}
