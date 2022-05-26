#ifndef GRASP_H
#define GRASP_H
#include "Solution.hpp"

enum MethodTabuSearch {Classic, Bias, Pop};
enum MethodLS {FirstImprovement, BestImprovement};

struct Move{
  int in1, in2, out1, out2;
  Move (int in1, int in2, int out1, int out2): in1(in1), in2(in2), out1(out1), out2(out2){}

  static Move Reverse(const Move& move){
    return Move(move.out1, move.out2, move.in1, move.in2);
  }

  bool operator < (const Move& m2) const{
    return tuple<int, int, int, int>(in1, in2, out1, out2) < tuple<int, int, int, int>(m2.in1, m2.in2, m2.out1, m2.out2);
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
