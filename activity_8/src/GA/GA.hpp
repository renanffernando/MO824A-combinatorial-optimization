#ifndef GA_H
#define GA_H
#include "Solution.hpp"

enum GAType {Random, Classic, Diversity, Stochastic};

class GA {
  Solution *bestSol;
  vector<Solution *> population;
  Instance *instance;
  GAType gaType;
  ld mutationRate;
  int generation, popSize, timeLimit, target;
  chrono::steady_clock::time_point begin;
  mt19937 rng;

public:
  GA(Instance *instance, GAType gaType, ld mutationRate, int popSize, int timeLimit, int target);

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
