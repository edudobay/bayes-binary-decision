#include "common.h"
#include "agent.h"
#include <stdexcept>
#include <cmath>
#include <iostream>

Agent::Agent(int order, int vector_size)
   : UsesRandom(), order(order), time(0), state(0),
     vsize(vector_size), mask(vsize - 1),
     prob(vsize, 0.5),
     m(vsize, 0), j(vsize, 0)
{}

Agent::~Agent()
{}

void Agent::clamp_prob(int state)
{
   if (prob[state] > 1)
      prob[state] = 1;
   else if (prob[state] < 0)
      prob[state] = 0;
}

void Agent::reset()
{
   prob.assign(vsize, 0.5);
   m.assign(vsize, 0);
   j.assign(vsize, 0);

   time = 0;
   state = 0;
}

int Agent::respond()
{
   answer = decide_answer();
   time++;
   return answer;
}

vector<double> Agent::get_metrics() const {
   return prob;
}

int Agent::get_metrics_size() const {
   return prob.size();
}

Agent* Agent::create(string id, int order)
{
   if (id == "X")
      return new AgentX(order);
   else if (id == "XY")
      return new AgentXY(order);
   else if (id == "Xi")
      return new AgentXIntegral(order);
   else if (id == "XYi")
      return new AgentXYIntegral(order);
   else if (id == "Xb")
      return new AgentXBeta(order);
   else if (id == "XYb")
      return new AgentXYBeta(order);
   else if (id == "WS")
      return new AgentWS(order);
   else if (id == "WSi")
      return new AgentWSIntegral(order);
   else if (id == "WSj")
      return new AgentWSIntegral2(order);

   throw new std::invalid_argument("Agent::create: invalid id");
}

// -----------------------------------------------------------------------------

AgentX::AgentX(int order)
   : Agent(order, 1 << order)
{
}

int AgentX::decide_answer()
{
   return rand->gen_01(prob[state]);
}

void AgentX::update_state(int result)
{
   if (order > 0)
      state = ((state << 1) & mask) | result;
}

void AgentX::update_prob(int state)
{
   prob[state] = as_bool(j[state] > 0.5*m[state]);
}

void AgentX::learn(int result)
{
   correctAnswer = result;
   if (time > order) {
      m[state]++;
      if (result == 1)
         j[state]++;
      update_prob(state);
   }
   update_state(result);
}

// -----------------------------------------------------------------------------

void AgentXIntegral::update_prob(int state)
{
   int m = this->m[state];
   int j = this->j[state];
   // prob[state] = 0.5 * (1 + erf((j - m/2.)/sqrt(m/2.)));
   prob[state] = 0.5 * (1 + erf(
         (j - m/2.) /
         // sqrt(2. * j * (m - j) / m) // gaussian approximation
         sqrt(m / 2.) // better estimate
      ));
}

// -----------------------------------------------------------------------------

AgentXBeta::AgentXBeta(int order)
   : AgentX(order),
     delta(vsize, 0.5)
{}

void AgentXBeta::reset()
{
   AgentX::reset();
   delta.assign(vsize, 0.5);
}

void AgentXBeta::update_prob(int state)
{
   int m = this->m[state];
   int j = this->j[state];
   double& delta = this->delta[state];
   double& q = this->prob[state];

   delta *= m * 0.5;
   if (correctAnswer == 1) {
      q += (delta /= j);
   }
   else {
      q -= (delta /= (m - j));
   }
}

// -----------------------------------------------------------------------------

AgentXY::AgentXY(int order)
   : AgentX(order, 1 << (2 * order))
{
}

void AgentXY::update_state(int result)
{
   if (order > 0)
      state = ((state << 2) & mask) | result | (answer << 1);
}

// -----------------------------------------------------------------------------

void AgentXYIntegral::update_prob(int state)
{
   int m = this->m[state];
   int j = this->j[state];
   prob[state] = 0.5 * (1 + erf((j - m/2.)/sqrt(m/2.)));
}

// -----------------------------------------------------------------------------

AgentXYBeta::AgentXYBeta(int order)
   : AgentXY(order),
     delta(vsize, 0.5)
{}

void AgentXYBeta::reset()
{
   AgentXY::reset();
   delta.assign(vsize, 0.5);
}

void AgentXYBeta::update_prob(int state)
{
   int m = this->m[state];
   int j = this->j[state];
   double& delta = this->delta[state];
   double& q = this->prob[state];

   delta *= m * 0.5;
   if (correctAnswer == 1) {
      q += (delta /= j);
   }
   else {
      q -= (delta /= (m - j));
   }
}

// -----------------------------------------------------------------------------

AgentWS::AgentWS(int order)
   : Agent(order, 1 << (2 * order)),
     c1(1), c2(1)
{
   answer = 0;
}

int AgentWS::decide_stay()
{
   if (time <= order) // TODO <= ou < ?
      return rand->gen_01(0.5);
   else
      return rand->gen_01_theta(prob[state]);
}

int AgentWS::decide_answer()
{
   didStay = decide_stay();
   return didStay ? answer : (1 - answer);
}

void AgentWS::learn(int result)
{
   wasCorrect = as_bool(result == answer);

   if (time > order) {
      m[state]++;
      if (wasCorrect == didStay)
         j[state]++;
      update_prob(state);
   }
   update_state();
}

// xx xxx xxxx: must be called BEFORE updating j
// 14 May 2014: Why??
void AgentWS::update_prob(int state)
{
   const int& m = this->m[state];
   const int& j = this->j[state];
   int x = as_bool(wasCorrect == didStay);

   prob[state] += -c1 * (j + 1.)/((m+2.)*(m+3.)) + c2*x * 1./(m+3.);
   clamp_prob(state);
}

void AgentWS::update_state()
{
   if (order > 0) {
      state = ((state << 2) & mask) | (wasCorrect << 1) | didStay;
   }
}

void AgentWS::set_constants(double _1, double _2)
{
   c1 = _1;
   c2 = _2;
}

// -----------------------------------------------------------------------------

AgentWSIntegral::AgentWSIntegral(int order)
   : AgentWS(order),
     qs(vsize)
{}

int AgentWSIntegral::decide_stay()
{
   if (time <= order) // TODO <= ou < ?
      return rand->gen_01(0.5);
   else
      return rand->gen_01(qs[state]);
   
}

void AgentWSIntegral::update_prob(int state)
{
   AgentWS::update_prob(state);
   const double& p = prob[state];
   const int& m = this->m[state];
   // const int& j = this->j[state];

   if (p == 0)
      qs[state] = 0;
   else if (p == 1)
      qs[state] = 1;
   else
      qs[state] = 0.5 * (1 + erf(
         sqrt(m / (2 * p * (1-p))) * (p - 0.5)
      ));
}

void AgentWSIntegral2::update_prob(int state)
{
   AgentWS::update_prob(state);
   const double& p = prob[state];
   const int& m = this->m[state];
   // const int& j = this->j[state];

   if (p == 0)
      qs[state] = 0;
   else if (p == 1)
      qs[state] = 1;
   else
      qs[state] = 0.5 * (1 + erf(
         sqrt(m * 2) * (p - 0.5)
      ));
}

// -----------------------------------------------------------------------------

