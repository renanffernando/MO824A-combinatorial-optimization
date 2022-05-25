#ifndef GRASP_H
#define GRASP_H
#include "Solution.hpp"

enum MethodTabuSearch {Classic, Bias, Pop};
enum MethodLS {FirstImprovement, BestImprovement};

struct Move{
  int in, out;
  Move (int in, int out): in(in), out(out) {}

  static Move Reverse(const Move& move){
    return Move(move.out, move.in);
  }

  bool operator < (const Move& m2) const{
    return make_pair(in, out) < make_pair(m2.in, m2.out);
  }
};

class TabuSearch{
  Solution* sol, *bestSol;
  Instance* instance;
  MethodLS methodls; 
  int timeLimit, maxIterations;
  int lsIterations, maxTabuSize = 30;
  deque<Move> tabuList;
  set<Move> tabuSet;
  chrono::steady_clock::time_point begin;

  public:
  TabuSearch(Instance* instance, MethodLS methodls, int timeLimit, int maxIterations);
  
  int getTime();
  Solution* run(Solution* sol);
  void gotoBestNeighborhood();
  void localSearch();
};

#endif
