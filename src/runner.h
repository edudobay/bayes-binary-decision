#ifndef __RUNNER_H_
#define __RUNNER_H_

struct RunInfo {
   const char *agentClass;
   int agentOrder;
   double *agentLC;
   uint32_t agentSeed;
   int machineOrder;
   double *machineProbs;
   uint32_t machineSeed;
   int nIter;
   char *fileName;
   int printScore;
   int outputMode; // for possible values, see enum OutputMode
   int nRepetitions;
   int nScore; // number of results to score (counting from end)
   int shiftProbsStep; // print probabilities every `N` steps
                       // (N = 0: print only the final value)
};

// Possible modes of outputting simulation data, set via
// the RunInfo::outputMode parameter
enum OutputMode {
   OutputEvolution  = 0, // Output time series with x/y values, error rates
                         // and extra parameters of the agent.
   OutputScore      = 1, // Output only the final error rate of the agent.
   OutputShiftProbs = 2, // Output only the computed P(shift|{win,loss}).
};

// extern "C"
// void runModel(const RunInfo& info);

#endif /* __RUNNER_H_ */
