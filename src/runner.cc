#include "agent.h"
#include "markov.h"
#include "runner.h"
#include <iostream>
#include <fstream>

using namespace std;

struct RunData {
   const RunInfo& info;
   MarkovMachine& markov;
   Agent& agent;
   shared_ptr<Random> machineRand;
   shared_ptr<Random> agentRand;
};

//void runModel(const RunInfo& info, MarkovMachine& markov, Agent& agent);
void runModel(RunData& run);

#ifdef __MINGW32__
# define EXPORT extern "C" __declspec(dllexport)
#else
# define EXPORT extern "C"
#endif

void prepareRandom(RunData& run)
{
   // Setup random generators
   run.machineRand = make_shared<Random> ();
   run.agentRand = make_shared<Random> ();

   if (run.info.agentSeed != 0)
      run.agentRand->set_x(run.info.agentSeed);
   else
      run.agentRand->seed();

   if (run.info.machineSeed != 0)
      run.machineRand->set_x(run.info.machineSeed);
   else
      run.machineRand->seed(run.agentRand->get_x());

   (void) run.agentRand->gen_unit();

   run.markov.setRand(run.machineRand);
   run.agent.setRand(run.agentRand);
}

EXPORT
void runModel(RunInfo& info)
{
   // Setup machine
   MarkovMachine markov(info.machineOrder);
   markov.setProb(info.machineProbs);

   // Setup agent
   unique_ptr<Agent> agent(Agent::create(info.agentClass, info.agentOrder));

   RunData run = {info, markov, *agent};

   // Setup random generators
   prepareRandom(run);
   markov.prepare();

   // Settings specific to WS agent
   AgentWS* ws = dynamic_cast<AgentWS*> (agent.get());
   if (ws) {
      ws->set_constants(info.agentLC[0], info.agentLC[1]);
   }

   runModel(run);
}

struct ModelResults {
   int n, nError, nScore;
   int pShift[2]; // {P(shift|win), P(shift|loss)}
};

void runNormal(RunData& run, ModelResults& results)
{
   int x, y, n, start = 0;
   results.n = results.nScore = run.info.nIter;
   results.nError = 0;

   if (run.info.nScore > 0 && run.info.nScore < run.info.nIter) {
      start = run.info.nIter - run.info.nScore;
      results.nScore = run.info.nScore;
   }

   for (n = 0; n < run.info.nIter; n++) {
      y = run.agent.respond();
      x = run.markov.step();
      run.agent.learn(x);
      if (n >= start && x != y) results.nError++;
   }
}

// Run the simulation counting the frequencies of response-shifts after wins
// and losses.
void runCountShiftProbs(RunData& run, ModelResults& results)
{
   int x, y, prevWon, prevY;
   int n, nWin = 0, nWinShift = 0, nLoss = 0, nLossShift = 0, start = 0;

   results.n = results.nScore = run.info.nIter;

   if (run.info.nScore > 0 && run.info.nScore < run.info.nIter) {
      start = run.info.nIter - run.info.nScore;
      results.nScore = run.info.nScore;
   }

   // add 1 because the first iteration only stores the X/Y values,
   // without doing any comparison
   start++;

   for (n = 0; n < run.info.nIter; n++) {
      y = run.agent.respond();
      x = run.markov.step();
      run.agent.learn(x);
      if (n >= start) {
         if (prevWon) {
            nWin++;
            if (y != prevY)
               nWinShift++;
         } else {
            nLoss++;
            if (y != prevY)
               nLossShift++;
         }
      }
      prevWon = (x == y);
      prevY = y;
   }

   results.pShift[0] = nWinShift / (double) nWin;
   results.pShift[1] = nLossShift / (double) nLoss;
}

void runWithOutput(RunData& run, ModelResults& results, std::ostream& output)
{
   int x, y, n;
   results.n = run.info.nIter;
   results.nError = 0;

   for (n = 0; n < run.info.nIter; n++) {
      y = run.agent.respond();
      x = run.markov.step();
      run.agent.learn(x);
      if (x != y) results.nError++;
      output << x << '\t' << y << '\t' << results.nError;
      for (double v : run.agent.get_metrics())
         output << '\t' << v;
      output << '\n';
   }
}

void runModel(RunData& run)
{
   const RunInfo& info = run.info;
   MarkovMachine& markov = run.markov;
   Agent& agent = run.agent;

   // markov.clean();
   ModelResults results;
   if (info.fileName) {
      ofstream fl(info.fileName);
      switch (info.outputMode) {
      case OutputEvolution:
         runWithOutput(run, results, fl);
         break;

      case OutputScore:
         for (int i = 0; i < info.nRepetitions; i++) {
            runNormal(run, results);
            fl << (double)results.nError / results.nScore << "\n";

            markov.clean();
            agent.reset();
            prepareRandom(run);
            markov.prepare();
         }
         break;

      case OutputShiftProbs:
         for (int i = 0; i < info.nRepetitions; i++) {
            runCountShiftProbs(run, results);
            fl << results.pShift[0] << '\t' << results.pShift[1] << "\n";

            markov.clean();
            agent.reset();
            prepareRandom(run);
            markov.prepare();
         }
         break;

      }
   }
   else {
      runNormal(run, results);
   }

   if (info.printScore) {
      cout << "Error rate: " << (double)results.nError / results.n
           << " | " << results.nError << " / " << results.n << "\n";
   }

}

void printInfo(const RunInfo& info)
{
   cout << "Agent order: " << info.agentOrder << "\n"
        << "Machine order: " << info.machineOrder << "\n";

}

