/*
 * rand.h
 *
 *  Created on: 22/07/2012
 *      Author: Eduardo
 */

#ifndef __RAND_H_
#define __RAND_H_

#include <cstdint>
#include <memory>

using std::uint32_t;
using std::shared_ptr;

class Random {
public:
   Random();
   void seed();
   void seed(uint32_t);
   void set_x(uint32_t);
   uint32_t get_x(void);
   uint32_t gen_int(void);
   int gen_range(int min, int max);
   double gen_unit(void);
   int gen_01(double prob1);
   int gen_01_theta(double prob1);

private:
   uint32_t x, y, z, w;
};

class UsesRandom {
public:
   void setRand(Random *rand);
   void setRand(shared_ptr<Random> rand);
protected:
   UsesRandom();

   shared_ptr<Random> rand;
};

#endif /* __RAND_H_ */
