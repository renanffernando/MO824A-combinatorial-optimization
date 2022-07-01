#include "Neighborhood.hpp"

Solution* Neighborhood::anyImprovement(Solution* sol, chrono::steady_clock::time_point begin, MethodLS methodls, int timeLimit){
  Solution* bestSol = new Solution(*sol);
  while(true){
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
    int elapsedTime = elapsed.count();
    if(elapsedTime > timeLimit)
      break;
    lsIterations++;
    Solution* neighbor = improvingNeighbor(bestSol, methodls);
    if(neighbor->cost <= bestSol->cost)
      break;
    swap(bestSol, neighbor);
    delete neighbor;
  }
  return bestSol;
}

Solution* FlipNeighborhood::improvingNeighbor(Solution* bestSol, MethodLS methodls){
  int bestMove = -1;
  ll bestDelta = 0;
  FOR(i, bestSol->n){
    if(!bestSol->used[i] && !bestSol->canAdd(i))
      continue;

    ll delta = bestSol->deltaAdd(i);
    if(bestSol->used[i])
      delta *= -1;
    if (delta > bestDelta){
      bestDelta = delta;
      bestMove = i;
      if (methodls == FirstImprovement)
        break;
    }
  }
  Solution* bestNeighbor = new Solution(*bestSol);
  if (bestMove != -1){
    assert(bestDelta > 0);
    ll newCost = bestNeighbor->cost + bestDelta;
    bestNeighbor->flip(bestMove);
    assert(newCost == bestNeighbor->cost);
  }
  return bestNeighbor;
}

Solution* SwapNeighborhood::improvingNeighbor(Solution* bestSol, MethodLS methodls){
  ii bestMove = {-1, -1};
  ll bestDelta = 0;

  FOR(i, bestSol->n){
    if(bestSol->used[i])
      continue;

    FOR(j, bestSol->n){
      if(!bestSol->used[j])
        continue;

      if(bestSol->weight + bestSol->instance->weights[i] - bestSol->instance->weights[j] > bestSol->instance->W)
        continue;

      ll delta = bestSol->deltaAdd(i) - bestSol->deltaAdd(j) - bestSol->instance->cost[i][j] - bestSol->instance->cost[j][i];
      if (delta > bestDelta){
        bestDelta = delta;
        bestMove = {i, j};
        if (methodls == FirstImprovement)
          break;
      }
    }
  }
  Solution* bestNeighbor = new Solution(*bestSol);
  if (bestMove.first != -1){
    assert(bestDelta > 0);
    ll newCost = bestNeighbor->cost + bestDelta;
    bestNeighbor->remove(bestMove.second);
    bestNeighbor->add(bestMove.first);
    assert(newCost == bestNeighbor->cost);
  }
  return bestNeighbor;
}
