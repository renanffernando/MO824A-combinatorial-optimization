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
  const int timeLimit = 60 * 1e6;
  vector<MethodTS> methodsTS = {Diversification}; //, Probabilistic};
  vector<MethodLS> methodsLS = {FirstImprovement};
  vi tenures = {1000};
  map<int, int> targets;
  targets.insert({20, 110});
  targets.insert({40, 308});
  targets.insert({60, 446});
  targets.insert({80, 750});
  targets.insert({100, 1200});
  targets.insert({200, 3900});
  targets.insert({400, 10600});

  for(int tenure : tenures)
    for(MethodLS methodls : methodsLS)
    for (MethodTS methodTS : methodsTS){
      assert(targets.count(instance->n));
      Solution* initialSol = buildInitial(instance, 0.05);
      TabuSearch ts(instance, methodls, methodTS, tenure, timeLimit, INT_MAX);
      Solution* sol = ts.run(initialSol);

      assert(sol->value == Solution::computeValue(sol));
      assert(sol->getWeight() <= instance->W);

      cout << setprecision(6) << fixed;
      cout << "Cost: " << sol->getCost() << " - Time: " << sol->elapsedTime/1e6;
      cout << endl;
      delete sol;
      delete initialSol;
    }

  delete instance;
}
