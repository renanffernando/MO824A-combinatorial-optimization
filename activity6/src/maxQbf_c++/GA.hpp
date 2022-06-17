#ifndef GA_H
#define GA_H
#include "Solution.hpp"

class GA
{
  Solution *bestSol;
  vector<Solution *> population;
  Instance *instance;
  int generation, popSize, timeLimit, maxGenerations;
  chrono::steady_clock::time_point begin;
  mt19937 rng;

public:
  GA(Instance *instance, int popSize, int timeLimit, int maxGenerations);

  int getTime();
  Solution *run();
  Solution* buildInitial(Instance* instance, double alpha);
  void gotoBestNeighborhood();
  vector<Solution*> getParents();
  void nextGeneration();
  void buildInitialPopulation();
  void free(vector<Solution*>& v);
  void mutation(vector<Solution*>& parents);
  vector<Solution*> crossover(vector<Solution*> parents);
};

#endif
