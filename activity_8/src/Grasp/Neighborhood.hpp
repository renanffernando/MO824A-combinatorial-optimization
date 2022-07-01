#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H
#include "Solution.hpp"

enum MethodLS {FirstImprovement, BestImprovement};

class Neighborhood{
  public:
  static inline ll lsIterations = 0;

  Solution* anyImprovement(Solution* sol, chrono::steady_clock::time_point begin, MethodLS methodls, int timeLimit);

  virtual Solution* improvingNeighbor(Solution* bestSol, MethodLS methodls) = 0;
};

class FlipNeighborhood: public Neighborhood{
  Solution* improvingNeighbor(Solution* bestSol, MethodLS methodls) override;
};

class SwapNeighborhood: public Neighborhood{
  Solution* improvingNeighbor(Solution* bestSol, MethodLS methodls) override;
};

#endif
