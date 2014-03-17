#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <random>
#include <vector>
#include <cmath>

using namespace std;

double timediff (const timespec& start, const timespec& end)
{
   return (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
}

void simulate1(const vector<int>& v)
{
   ofstream f("b1.dat");
   double q = 0.5;
   unsigned m = 0, j = 0;
   double delta = 0.25;

   while (m < v.size()) {
      j += v[m]; // Here m is understood as (µ - 1), the offset needed to get
                 // the µ-th element of the vector
      delta *= (m + 1) * 0.5; // Here m is understood as the µ from the
                              // previous iteration
      if (v[m]) { // x = 1
         delta /= j;
         q += delta;
      }
      else { // x = 0
         delta /= (m + 1 - j);
         q -= delta;
      }
      ++m;
      f << q << "\n";
   }

   cout << q << "\n";
}

void simulate0(const vector<int>& v)
{
   ofstream f("b0.dat");
   unsigned m = 0, j = 0;
   double prob;
   while (m < v.size()) {
      j += v[m];
      ++m;
      prob = 0.5 * (1 + erf((j - m/2.)/sqrt(m/2.)));
      f << prob << "\n";
   }
   cout << prob << "\n";
}

int main(int argc, char *argv[])
{
   if (argc < 2) return 1;
   int n = atoi(argv[1]);

   default_random_engine generator;
   uniform_int_distribution<int> distribution(0,1);

   vector<int> v;
   v.reserve(n);
   int j = 0;
   for (int i = n; i > 0; --i) {
      j = distribution(generator);
      v.push_back(j);
   }

   timespec t0, t1, t2;
   clock_gettime(CLOCK_REALTIME, &t0);

   simulate0(v);
   clock_gettime(CLOCK_REALTIME, &t1);

   simulate1(v);
   clock_gettime(CLOCK_REALTIME, &t2);

   cout <<
      "0: " << timediff(t0, t1) << "\n"
      "1: " << timediff(t1, t2) << "\n";

   return 0;
}
