#include "TS.hpp"

Solution* buildInitial(Instance* instance, double alpha){
  const int seed = 0;
  mt19937 rng(seed);
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

int main(){
  cin.tie(0)->sync_with_stdio(0);

  Instance* instance = Instance::readInstance();
  const int timeLimit = 60000 * 30;
  const int maxIterations = 20000;
  vector<MethodLS> methods = {FirstImprovement, BestImprovement};

  for(MethodLS methodls : methods){
    Solution* initialSol = buildInitial(instance, 0.05);
    TabuSearch ts(instance, methodls, timeLimit, maxIterations);
    Solution* sol = ts.run(initialSol);

    assert(sol->cost == Solution::computeCost(sol));
    assert(sol->weight <= instance->W);

    cout << setprecision(3) << fixed;
    switch (methodls){
      case BestImprovement:
        cout << "Best  Improvement";
        break;
      case FirstImprovement:
        cout << "First Improvement";
        break;
      default:
        throw runtime_error("Invalid Local Search Method");
    }

    cout << " - CostInitial: " << initialSol->cost <<  " - Cost: " << sol->cost << " - Weight: " << sol->weight  << " - Time: " << sol->elapsedTime/1000.0 << "s - numIterationsTS: " << sol->iterations << endl;
    delete sol;
    delete initialSol;
  }

  delete instance;
}
