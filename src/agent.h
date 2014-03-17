#ifndef __AGENT_H_
#define __AGENT_H_

#include "rand.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

// -----------------------------------------------------------------------------

class Agent : public UsesRandom {
protected:
   Agent(int order, int vector_size);

   void clamp_prob(int state);

public:
   virtual ~Agent();

   virtual void reset();

   virtual int decide_answer() = 0;
   virtual int respond();

   // virtual void update_state(int result) = 0;
   virtual void update_prob(int state) = 0;
   virtual void learn(int result) = 0;

   virtual vector<double> get_metrics() const;

   static Agent* create(string id, int order);

protected:
   int order;
   int time;
   int state;

   int vsize;
   int mask;
   vector<double> prob;
   vector<int> m, j;

   int answer; // last answer
   int correctAnswer;
};

class AgentX : public Agent {
protected:
   using Agent::Agent;
public:
   AgentX(int order);
   virtual int decide_answer();
   virtual void update_state(int result);
   virtual void update_prob(int state);
   virtual void learn(int result);
};

class AgentXIntegral : public AgentX {
public:
   using AgentX::AgentX;
   virtual void update_prob(int state);
};

class AgentXBeta : public AgentX {
public:
   AgentXBeta(int order);
   virtual void reset();
   virtual void update_prob(int state);
   //virtual vector<double> get_metrics() const;
protected:
   vector<double> delta;
};

class AgentXY : public AgentX {
public:
   AgentXY(int order);
   virtual void update_state(int result);
};

class AgentXYIntegral : public AgentXY {
public:
   using AgentXY::AgentXY;
   virtual void update_prob(int state);
};

class AgentXYBeta : public AgentXY {
public:
   AgentXYBeta(int order);
   virtual void reset();
   virtual void update_prob(int state);
protected:
   vector<double> delta;
};

class AgentWS : public Agent {
protected:
   int wasCorrect;
   int didStay;
   double c1, c2;

public:
   AgentWS(int order);
   virtual int decide_stay();
   virtual int decide_answer();
   virtual void learn(int result);
   virtual void update_prob(int state);
   virtual void update_state();
   virtual void set_constants(double _1, double _2);

};

class AgentWSIntegral : public AgentWS {
protected:
   vector<double> qs;

public:
   AgentWSIntegral(int order);
   virtual int decide_stay();
   virtual void update_prob(int state);
};

class AgentWSIntegral2 : public AgentWSIntegral {
public:
   using AgentWSIntegral::AgentWSIntegral;
   virtual void update_prob(int state);
};

// -----------------------------------------------------------------------------

class AgentEvaluator {
   AgentEvaluator(Agent& agent);
   void evaluate(int answer, int model);
   int nTotal, nCorrect;

};

// -----------------------------------------------------------------------------

#endif /* __AGENT_H_ */
