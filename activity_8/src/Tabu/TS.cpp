#include "TS.hpp"

TabuSearch::TabuSearch(Instance* instance, MethodLS methodls,  MethodTS methodTS, int maxTabuSize, int timeLimit, int target):
  instance(instance), methodls(methodls), methodTS(methodTS), maxTabuSize(maxTabuSize), timeLimit(timeLimit), target(target), rng(random_device().operator()()),
  iterationsInSol(instance->n), fixed(instance->n){}

void TabuSearch::gotoBestNeighborhood(){
  Move bestMove(-1, -1, -1, -1);
  Value bestDelta(INT_MIN, 0);

  FOR(i, sol->n){
    if(fixed[i])
      continue;
    Move curMove = sol->used[i] ? Move(-1, -1, i, -1) : Move(i, -1, -1, -1);

    if(!sol->used[i] && !sol->canAdd(i))
      continue;

    Value delta = sol->deltaAdd(i);
    if(sol->used[i])
      delta *= -1;

    if (tabuSet.count(curMove) && delta + sol->value <= bestSol->value)
      continue;

    if (delta > bestDelta){
      bestDelta = delta;
      bestMove = curMove;
      if (bestDelta > Value(0, 0) && methodls == FirstImprovement)
        break;
    }
  }

  if(methodls != FirstImprovement || bestDelta <= Value(0, 0)){
    vi vin, vout;
    FOR(i, sol->n){
      if(fixed[i])
        continue;
      (sol->used[i] ? vin : vout).push_back(i);
    }
    
    for(int i : vout){
      for(int j : vin){
        ll deltaWeight = sol->instance->weights[i] - sol->instance->weights[j];

        if(sol->getWeight() + deltaWeight > sol->instance->W)
          continue;

        ll deltaCost = sol->deltaAdd(i).cost - sol->deltaAdd(j).cost - sol->instance->cost[i][j] - sol->instance->cost[j][i];
        
        Value delta(deltaCost, deltaWeight);
        Move curMove(i, -1, j, -1);
        if (tabuSet.count(curMove) && delta + sol->value <= bestSol->value)
          continue;

        if (delta > bestDelta){
          bestDelta = delta;
          bestMove = {i, -1, j, -1};
          if (bestDelta > Value(0, 0) && methodls == FirstImprovement)
            break;
        }
      }
    }
  }

  if(methodTS == Probabilistic && (methodls != FirstImprovement || bestDelta <= Value(0, 0))){
    vi vin, vout;
    FOR(i, sol->n){
      if(fixed[i])
        continue;
      (sol->used[i] ? vin : vout).push_back(i);
    }
    
    vi vin1 = vin, vin2 = vin;
    vi vout1 = vout, vout2 = vout;
    shuffle(all(vin1), rng);
    shuffle(all(vin2), rng);
    shuffle(all(vout1), rng);
    shuffle(all(vout2), rng);

    int MAXSZ = ceil(pow(instance->n, 0.5)/2.0);
    vin1.resize(min(SZ(vin1), MAXSZ));
    vin2.resize(min(SZ(vin2), MAXSZ));
    vout1.resize(min(SZ(vout1), MAXSZ));
    vout2.resize(min(SZ(vout2), MAXSZ));

    for(int i : vout1){
      for(int h : vout2){
        if(i == h)
          continue;
        for(int j : vin1){
          for(int l : vin2){
            if(j == l)
              continue;

            ll deltaWeight = sol->instance->weights[i] + sol->instance->weights[h] - sol->instance->weights[j] - sol->instance->weights[l];
            if(sol->getWeight() + deltaWeight > sol->instance->W)
              continue;

            ll deltaCost = sol->deltaAdd(i).cost + sol->deltaAdd(h).cost - sol->deltaAdd(j).cost - sol->deltaAdd(l).cost;
            deltaCost -= sol->instance->cost[i][j] + sol->instance->cost[j][i] + sol->instance->cost[h][j] + sol->instance->cost[j][h];
            deltaCost -= sol->instance->cost[i][l] + sol->instance->cost[l][i] + sol->instance->cost[h][l] + sol->instance->cost[l][h];
            deltaCost += sol->instance->cost[i][h] + sol->instance->cost[h][i];
            deltaCost += sol->instance->cost[j][l] + sol->instance->cost[l][j];
            Move curMove(i, h, j, l);
            Value delta(deltaCost, deltaWeight);

            if (tabuSet.count(curMove) && delta + sol->value <= bestSol->value)
              continue;

            if (delta > bestDelta){
              bestDelta = delta;
              bestMove = {i, h, j, l};
              if (bestDelta > Value(0, 0) && methodls == FirstImprovement)
                break;
            }
          }
        }
      }
    }
  }

  assert(bestMove.in1 != -1 || bestMove.out1 != -1);
  Value newValue = sol->value + bestDelta;

  if(bestMove.out1 != -1)
    sol->remove(bestMove.out1);
  if(bestMove.out2 != -1)
    sol->remove(bestMove.out2);
  if(bestMove.in1 != -1)
    sol->add(bestMove.in1);
  if(bestMove.in2 != -1)
    sol->add(bestMove.in2);
  assert(newValue == sol->value);

  assert(!tabuSet.count(bestMove) || sol->value > bestSol->value);
  
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

  if(sol->value > bestSol->value){
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
  auto elapsed = chrono::duration_cast<chrono::microseconds>(now - begin);
  return elapsed.count();
}

void TabuSearch::localSearch(){
  lsIterations = 0;
  const int C1 = 500;
  const int C2 = 30;
  int iterationsToReset = C1;
  int iterationsToUnfix = C2;

  while(true){
    assert(iterationsToReset >= 2 * iterationsToUnfix);
    FOR(i, sol->n)
      if(sol->used[i])
        iterationsInSol[i]++;
      else
        iterationsInSol[i] = 0;

    int elapsedTime = getTime();
    if(elapsedTime > timeLimit || bestSol->getCost() >= target)
      break;

    if(methodTS == Diversification){
      if(iterationsToReset == 0){
        delete sol;
        sol = new Solution(*bestSol);

        vector<ii> cands;
        FOR(i, instance->n)
          if(sol->used[i])
            cands.push_back({iterationsInSol[i], i});

        sort(all(cands));
        cands.resize(floor(0.8 * SZ(cands)));
        for(auto c : cands)
          fixed[c.second] = true;
        iterationsToReset = C1;
        iterationsToUnfix = C2;
      }

      if(iterationsToUnfix == 0)
        FOR(i, sol->n)
          fixed[i] = 0;
      iterationsToReset--;
      iterationsToUnfix--;
    }

    gotoBestNeighborhood();
    lsIterations++;
  }
}
