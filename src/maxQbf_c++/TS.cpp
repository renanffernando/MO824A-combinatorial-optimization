#include "TS.hpp"

TabuSearch::TabuSearch(Instance* instance, MethodLS methodls, int timeLimit, int maxIterations):
  instance(instance), methodls(methodls), timeLimit(timeLimit), maxIterations(maxIterations){}

void TabuSearch::gotoBestNeighborhood(){
  Move bestMove(-1, -1);
  ll bestDelta = INT_MIN;

  FOR(i, sol->n){
    Move curMove = sol->used[i] ? Move(-1, i) : Move(i, -1);

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

  if(methodls != FirstImprovement || bestDelta <= 0)
    FOR(i, sol->n){
      if(sol->used[i])
        continue;

      FOR(j, sol->n){
        if(!sol->used[j])
          continue;

        if(sol->weight + sol->instance->weights[i] - sol->instance->weights[j] > sol->instance->W)
          continue;

        ll delta = sol->deltaAdd(i) - sol->deltaAdd(j) - sol->instance->cost[i][j] - sol->instance->cost[j][i];
        Move curMove(i, j);
        if (tabuSet.count(curMove) && delta + sol->cost <= bestSol->cost)
          continue;

        if (delta > bestDelta){
          bestDelta = delta;
          bestMove = {i, j};
          if (bestDelta > 0 && methodls == FirstImprovement)
            break;
        }
      }
    }

  assert(bestMove.in != -1 || bestMove.out != -1);
  ll newCost = sol->cost + bestDelta;

  if(bestMove.out != -1)
    sol->remove(bestMove.out);
  if(bestMove.in != -1)
    sol->add(bestMove.in);
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
