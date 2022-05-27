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

    multimap<Value, int, greater<Value>> costsSet;
    for(int i : valids)
      costsSet.insert({sol->deltaAdd(i), i});

    Value cMax = costsSet.begin()->first;
    Value cMin = prev(costsSet.end())->first;
    ll lowerBound = cMax.cost - ceil(alpha * (cMax.cost - cMin.cost));

    vi cands;
    for(auto pr : costsSet){
      if(pr.first.cost < lowerBound)
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
  const int maxIterations = 10000;
  vector<MethodTS> methodsTS = {Classic, Diversification, Probabilistic};
  vector<MethodLS> methodsLS = {FirstImprovement, BestImprovement};
  vi tenures = {20, 100};

  for(int tenure : tenures)
    for(MethodLS methodls : methodsLS)
    for (MethodTS methodTS : methodsTS){
      Solution* initialSol = buildInitial(instance, 0.05);
      TabuSearch ts(instance, methodls, methodTS, tenure, timeLimit, maxIterations);
      Solution* sol = ts.run(initialSol);

      assert(sol->value == Solution::computeValue(sol));
      assert(sol->getWeight() <= instance->W);

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

      switch (methodTS){
        case Classic:
          cout << " - Classic        ";
          break;
        case Probabilistic:
          cout << " - Probabilistic  ";
          break;
        case Diversification:
          cout << " - Diversification";
          break;
        default:
          throw runtime_error("Invalid Tabu Search Method");
      }

      cout << " - Tenure: " << tenure << " - CostInitial: " << initialSol->getCost();
      cout << " - Cost: " << sol->getCost() << " - Weight: " << sol->getWeight();
      cout << " - Time: " << sol->elapsedTime/1000.0 << "s - numIterationsTS: " << sol->iterations;
      cout << endl;
      delete sol;
      delete initialSol;
    }

  delete instance;
}
