#include "GRASP.hpp"

void localSearch(Solution*& bestSol, int mask, chrono::steady_clock::time_point begin, MethodLS methodls, int timeLimit){
  vector<Neighborhood*> neighborhoods;
  if(mask & 1)
    neighborhoods.push_back(new FlipNeighborhood());
  mask >>= 1;
  if(mask & 1)
    neighborhoods.push_back(new SwapNeighborhood());

  int k = 0;
  while(k < SZ(neighborhoods)){
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(now - begin);
    int elapsedTime = elapsed.count();
    if(elapsedTime > timeLimit)
      break;

    Solution* localOptimal = neighborhoods[k]->anyImprovement(bestSol, begin, methodls, timeLimit);

    if (bestSol->cost < localOptimal->cost){
      swap(bestSol, localOptimal);
      delete localOptimal;
      k = k == 0 ? 1 : 0;
    }else
      k++;
  }
}

Solution* buildInitialBias(Instance* instance, double alpha){
  const int seed = random_device().operator()();
  static mt19937 rng(seed);
  Solution* sol = new Solution(instance);
  int n = sol->n;

  while(true){
    vi valids;
    FOR(i, n)
      if(!sol->used[i] && sol->canAdd(i))
        valids.push_back(i);

    if(SZ(valids) == 0)
      break;

    multimap<ll, int, greater<ll>> costsSet;
    for(int i : valids)
      costsSet.insert({sol->deltaAdd(i), i});

    const bool onlyPositive = false;
    ll cMax = costsSet.begin()->first;
    ll cMin = prev(costsSet.end())->first;
    if(onlyPositive){
      if(cMax < 0)
        break;
      cMin = max<ll>(0, cMax);
    }

    ll lowerBound = cMax - ceil(alpha * (cMax - cMin));

    vi cands;
    for(auto pr : costsSet){
      if(pr.first < lowerBound)
        break;
      cands.push_back(pr.second);
    }

    assert(!cands.empty());
    auto bias = [](int e){return exp(-e);};

    vector<double> pi(SZ(cands));
    FOR(i, SZ(cands))
      pi[i] = bias(i);

    double acumTotal = accumulate(all(pi), 0.0);

    std::uniform_real_distribution<double> distribution(0, 1);

    double prob = distribution(rng);
    int chosen = SZ(cands) - 1;
    double acum = 0;
    FOR(i, SZ(cands)){
      acum += pi[i] / acumTotal;
      if(acum > prob){
        chosen = i;
        break;
      }
    }
    sol->add(cands[chosen]);
  }

  return sol;
}

Solution* buildInitialPop(Instance* instance, double alpha, int mask, chrono::steady_clock::time_point begin, MethodLS methodls, int timeLimit){
  const int seed = random_device().operator()();;
  static mt19937 rng(seed);
  Solution* sol = new Solution(instance);
  int n = sol->n;
  vector<double> breaksToLocalSearch = {0.4, 0.7};

  while(true){
    vi valids;
    FOR(i, n)
      if(!sol->used[i] && sol->canAdd(i))
        valids.push_back(i);

    if(SZ(valids) == 0)
      break;

    multimap<ll, int, greater<ll>> costsSet;
    for(int i : valids)
      costsSet.insert({sol->deltaAdd(i), i});

    const bool onlyPositive = false;
    ll cMax = costsSet.begin()->first;
    ll cMin = prev(costsSet.end())->first;
    if(onlyPositive){
      if(cMax < 0)
        break;
      cMin = max<ll>(0, cMax);
    }
    ll lowerBound = cMax - ceil(alpha * (cMax - cMin));

    vi cands;
    for(auto pr : costsSet){
      if(pr.first < lowerBound)
        break;
      cands.push_back(pr.second);
    }

    assert(!cands.empty());
    std::uniform_int_distribution<int> distribution(0, SZ(cands) - 1);

    int chosen = distribution(rng);
    sol->add(cands[chosen]);
    if(!breaksToLocalSearch.empty() && sol->weight >= breaksToLocalSearch[0] * sol->instance->W){
      breaksToLocalSearch.erase(breaksToLocalSearch.begin());
      localSearch(sol, mask, begin, methodls, timeLimit);
    }
  }

  return sol;
}

Solution* buildInitial(Instance* instance, double alpha){
  const int seed = random_device().operator()();
  static mt19937 rng(seed);
  Solution* sol = new Solution(instance);
  int n = sol->n;

  while(true){
    vi valids;
    FOR(i, n)
      if(!sol->used[i] && sol->canAdd(i))
        valids.push_back(i);

    if(SZ(valids) == 0)
      break;

    multimap<ll, int, greater<ll>> costsSet;
    for(int i : valids)
      costsSet.insert({sol->deltaAdd(i), i});

    const bool onlyPositive = false;
    ll cMax = costsSet.begin()->first;
    ll cMin = prev(costsSet.end())->first;
    if(onlyPositive){
      if(cMax < 0)
        break;
      cMin = max<ll>(0, cMax);
    }
    ll lowerBound = cMax - ceil(alpha * (cMax - cMin));

    vi cands;
    for(auto pr : costsSet){
      if(pr.first < lowerBound)
        break;
      cands.push_back(pr.second);
    }

    assert(!cands.empty());
    std::uniform_int_distribution<int> distribution(0, SZ(cands) - 1);

    int chosen = distribution(rng);
    sol->add(cands[chosen]);
  }

  return sol;
}

Solution* GRASP::Grasp(Instance* instance, MethodGrasp methodGrasp, MethodLS methodls, double alpha, int timeLimit, int target){
  Solution* bestSol = NULL;
  auto begin = chrono::steady_clock::now();
  int curIteration = 0;
  Neighborhood::lsIterations = 0;

  while(true){
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(now - begin);
    int elapsedTime = elapsed.count();
    if(elapsedTime > timeLimit || (bestSol != NULL && bestSol->cost >= target)){
      assert(bestSol != NULL);
      bestSol->elapsedTime = elapsedTime;
      bestSol->iterations = curIteration;
      break;
    }

    Solution* sol = NULL;

    switch (methodGrasp){
      case Classic:
        sol = buildInitial(instance, alpha);
        break;
      case Bias:
        sol = buildInitialBias(instance, alpha);
        break;
      case Pop:
        sol = buildInitialPop(instance, alpha, 2, begin, methodls, timeLimit);
    }
    localSearch(sol, 3, begin, methodls, timeLimit);

    if (bestSol == NULL || bestSol->cost < sol->cost){
      swap(bestSol, sol);
      if(sol != NULL)
        delete sol;
    }
    curIteration++;
  }

  return bestSol;
}
