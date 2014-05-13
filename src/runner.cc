#include "agent.h"
#include "markov.h"
#include "runner.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

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
   double pShift[2]; // {P(shift|win), P(shift|loss)}
   int run = 0;
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
void runCountShiftProbs(RunData& run, ModelResults& results, std::ostream& fOut)
{
   int x, y, wonPrev, yPrev;
   int n, nWin = 0, nWinShift = 0, nLoss = 0, nLossShift = 0, start = 0;
   int printStep = run.info.shiftProbsStep;

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
         if (wonPrev) {
            nWin++;
            if (y != yPrev)
               nWinShift++;
         } else {
            nLoss++;
            if (y != yPrev)
               nLossShift++;
         }
         if (printStep != 0 && (n + 1) % printStep == 0) {
            results.pShift[0] = nWinShift / (double) nWin;
            results.pShift[1] = nLossShift / (double) nLoss;
            fOut << results.pShift[0] << '\t' << results.pShift[1] << "\n";
         }
      }
      wonPrev = (x == y);
      yPrev = y;
   }

   results.pShift[0] = nWinShift / (double) nWin;
   results.pShift[1] = nLossShift / (double) nLoss;
   if (printStep == 0)
      fOut << results.pShift[0] << '\t' << results.pShift[1] << "\n";
   else // add an extra line break if outputting multiple lines per run
      fOut << "\n";

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

void runNormalAverage(RunData& run, ModelResults& results, vector<double>& data)
{
   int x, y, n;
   results.n = run.info.nIter;
   results.nError = 0;

   vector<double>::iterator output = data.begin();

   for (n = 0; n < run.info.nIter; n++) {
      y = run.agent.respond();
      x = run.markov.step();
      run.agent.learn(x);
      if (x != y) results.nError++;
      vector<double> metrics = run.agent.get_metrics();

      // accumulate error and metrics
      *output++ += results.nError;
      for (double m : metrics)
         *output++ += m;
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
      ofstream fOut(info.fileName);
      switch (info.outputMode) {
      case OutputEvolution:
         runWithOutput(run, results, fOut);
         break;

      case OutputScore:
         for (int i = 0; i < info.nRepetitions; i++) {
            runNormal(run, results);
            fOut << (double)results.nError / results.nScore << "\n";

            markov.clean();
            agent.reset();
            prepareRandom(run);
            markov.prepare();
         }
         break;

      case OutputShiftProbs:
         for (int i = 0; i < info.nRepetitions; i++) {
            runCountShiftProbs(run, results, fOut);

            markov.clean();
            agent.reset();
            prepareRandom(run);
            markov.prepare();
         }
         break;

      case OutputAveraged: {
         // zero-initialized matrix for accumulating sums
         int row_size = 1 + agent.get_metrics_size();
         vector<double> data(info.nIter * row_size, 0.0);

         for (int i = 0; i < info.nRepetitions; i++) {
            runNormalAverage(run, results, data);

            markov.clean();
            agent.reset();
            prepareRandom(run);
            markov.prepare();
         }

         // average
         for (double& v : data) {
            v /= info.nRepetitions;
         }

         for (int i = 0; i < info.nIter; i++) {
            fOut << data[i*row_size];
            for (int j = 1; j < row_size; j++)
               fOut << '\t' << data[i*row_size + j];
            fOut << '\n';
         }

         break;
      }

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

