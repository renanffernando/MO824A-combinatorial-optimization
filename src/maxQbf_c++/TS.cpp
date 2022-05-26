#include "TS.hpp"

TabuSearch::TabuSearch(Instance* instance, MethodLS methodls, int timeLimit, int maxIterations):
  instance(instance), methodls(methodls), timeLimit(timeLimit), maxIterations(maxIterations){}

void TabuSearch::gotoBestNeighborhood(){
  Move bestMove(-1, -1, -1, -1);
  ll bestDelta = INT_MIN;

  FOR(i, sol->n){
    Move curMove = sol->used[i] ? Move(-1, -1, i, -1) : Move(i, -1, -1, -1);

    if(!sol->used[i] && !sol->canAdd(i))
      continue;

    ll delta = sol->deltaAdd(i);
    if(sol->used[i])
      delta *= -1;

    if (tabuSet.count(curMove) && delta + sol->cost <= bestSol->cost)
      continue;

    if (delta > bestDelta){
      bestDelta = delta;
      bestMove = curMove;
      if (bestDelta > 0 && methodls == FirstImprovement)
        break;
    }
  }

  if(methodls != FirstImprovement || bestDelta <= 0){
    vi vin, vout;
    FOR(i, sol->n)
      (sol->used[i] ? vin : vout).push_back(i);
    
    for(int i : vout){
      for(int j : vin){

        if(sol->weight + sol->instance->weights[i] - sol->instance->weights[j] > sol->instance->W)
          continue;

        ll delta = sol->deltaAdd(i) - sol->deltaAdd(j) - sol->instance->cost[i][j] - sol->instance->cost[j][i];
        Move curMove(i, -1, j, -1);
        if (tabuSet.count(curMove) && delta + sol->cost <= bestSol->cost)
          continue;

        if (delta > bestDelta){
          bestDelta = delta;
          bestMove = {i, -1, j, -1};
          if (bestDelta > 0 && methodls == FirstImprovement)
            break;
        }
      }
    }
  }

  assert(bestMove.in1 != -1 || bestMove.out1 != -1);
  ll newCost = sol->cost + bestDelta;

  if(bestMove.out1 != -1)
    sol->remove(bestMove.out1);
  if(bestMove.out2 != -1)
    sol->remove(bestMove.out2);
  if(bestMove.in1 != -1)
    sol->add(bestMove.in1);
  if(bestMove.in2 != -1)
    sol->add(bestMove.in2);
  assert(newCost == sol->cost);

  assert(!tabuSet.count(bestMove) || sol->cost > bestSol->cost);
  
  Move revBestMove = Move::Reverse(bestMove);
  if(!tabuSet.count(revBestMove)){
    tabuList.push_back(revBestMove);
    tabuSet.insert(revBestMove);

    if(SZ(tabuList) > maxTabuSize){
      tabuSet.erase(tabuList.front());
      tabuList.pop_front();
    }
  }
  assert(SZ(tabuList) == SZ(tabuSet));

  if(sol->cost > bestSol->cost){
    delete bestSol;
    bestSol = new Solution(*sol);
  }
}

Solution* TabuSearch::run(Solution* sol){
  begin = chrono::steady_clock::now();
  this->sol = new Solution(*sol);
  this->bestSol = new Solution(*sol);

  localSearch();

  delete this->sol;
  bestSol->elapsedTime = getTime();
  bestSol->iterations = lsIterations;

  return bestSol;
}

int TabuSearch::getTime(){
  auto now = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
  return elapsed.count();
}

void TabuSearch::localSearch(){
  lsIterations = 0;

  while(true){
    int elapsedTime = getTime();
    if(elapsedTime > timeLimit || lsIterations >= maxIterations)
      break;

    gotoBestNeighborhood();
    lsIterations++;
  }
}
