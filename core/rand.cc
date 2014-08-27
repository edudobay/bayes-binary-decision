#include <sys/time.h>
#include <unistd.h>

#include "rand.h"

/****************************************************************************/

Random::Random()
   : x(123456789), y(364236069), z(521288629), w(88675123)
{}

void Random::seed(void)
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   x = getpid() * tv.tv_sec * tv.tv_usec;
}

void Random::seed(uint32_t val)
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   x = getpid() * tv.tv_sec * val * tv.tv_usec;
}

void Random::set_x(uint32_t newseed) {
   x = newseed;
}

uint32_t Random::get_x(void) {
   return x;
}

uint32_t Random::gen_int(void)
{
   uint32_t t;

   t = x ^ (x << 11);
   x = y;
   y = z;
   z = w;
   return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

int Random::gen_range(int min, int max)
{
   uint32_t r = this->gen_int();
   return min + (int)((r / (double)UINT32_MAX) * (max - min + 1));
}

double Random::gen_unit(void)
{
   uint32_t r = this->gen_int();
   return (double)r / UINT32_MAX;
}

int Random::gen_01(double prob1)
{
   if (prob1 == 0)
      return 0;
   if (prob1 == 1)
      return 1;

   double r = gen_unit();
   if (r < prob1)
      return 1;
   else
      return 0;
}

int Random::gen_01_theta(double prob1)
{
   if (prob1 == 0.5)
      return (gen_unit() < 0.5) ? 0 : 1;
   return (prob1 < 0.5) ? 0 : 1;
}

// -----------------------------------------------------------------------------

UsesRandom::UsesRandom() {}

void UsesRandom::setRand(Random *rand) {
   this->rand.reset(rand);
}

void UsesRandom::setRand(shared_ptr<Random> rand) {
   this->rand = rand;
}

