#include "Util.h"

#include <cmath>
#include <string>
#include <sstream>

void Stats::reset (void) { n = 0; mean = 0.0; M2 = 0.0; _sum = 0.0; }

void Stats::inc() { add(1.0); }

void Stats::add(long double x) {
  _sum += x;
  ++n;
  auto delta = x - mean;
  mean += (delta / n);
  M2 = M2 + delta * (x - mean);
}

double Stats::variance (void) const {
  if (n > 2) 
    return M2 / (n-1);
  else
    return 0.0;
} 

double Stats::std_err (void) const {
  return sqrt(variance()/n);
}

/** If we know the population size we can get the error
 * with finite population control (fpc).
 */
double Stats::std_err(unsigned int N) const {
  return sqrt(variance()/n)*fpc(N,n);
}

double Stats::sum (void) const { return _sum; }

const char * Stats::to_string() const {
  std::stringstream ss;
  ss << "Stats(";
  ss << "n(" << n << "), ";
  ss << "sum(" << sum() << "), ";
  ss << "mean(" << mean << "), ";
  ss << "var(" << variance( ) << "), ";
  ss << "std_err(" << std_err() << ")";
  ss << ")";
  return ss.str().c_str();
  
}


