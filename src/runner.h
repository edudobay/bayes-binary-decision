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
   int outputScore;
   int nRepetitions;
   int nScore;
};

// extern "C"
// void runModel(const RunInfo& info);

#endif /* __RUNNER_H_ */
