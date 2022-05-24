#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "util.hpp"

namespace Heuristic{
  void IncreaseIntersection(vi& tour1, vi& tour2, vvd & d1, vvd & d2);
  void makeFeasibleSolution (vi & tour0, vi & tour1, vvd & cost0, vvd & cost1, const int similarityParameter);
}

#endif
