#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using vll = vector<ll>;
using vvll = vector<vll>;
using vi = vector<int>;
using ii = pair<int, int>;
#define FOR(i, b) for(int i = 0; i < (b); i++)
#define SZ(a) ((int) a.size())
#define all(a) a.begin(), a.end()
#define NDEBUG

struct Instance{
  int n;
  ll W;
  vvll cost;
  vll weights;
  Instance(int n, ll W, vvll cost, vll weights): n(n), W(W), cost(cost), weights(weights){}
};

Instance* readInstance(){
  int n;
  ll W;
  cin >> n >> W;
  vll weights(n);
  vvll cost(n, vll(n));
  FOR(i, n)
    cin >> weights[i];

  FOR(i, n)
    for(int j = i; j < n; j++){
      cin >> cost[i][j];
      //cost[j][i] = cost[i][j];
    }

  return new Instance(n, W, cost, weights);
}

struct Solution{
  Instance* instance;
  int n, elapsedTime, iterations;
  vi used;
  ll cost, weight;
  Solution(Instance* instance): instance(instance), n(instance->n), elapsedTime(0), iterations(0), used(n, 0), cost(0), weight(0){}

  void add(int i){
    assert(!used[i]);
    assert(canAdd(i));
    used[i] = 1;
    weight += instance->weights[i];
    cost += deltaAdd(i);
    assert(cost == computeCost(this));
  }

  ll deltaAdd(int i){
    ll delta = 0;
    FOR(j, n)
      if(i != j && used[j]){
        delta += instance->cost[i][j];
        delta += instance->cost[j][i];
      }
    delta += instance->cost[i][i];
    return delta;
  }

  void remove(int i){
    assert(used[i]);
    used[i] = 0;
    weight -= instance->weights[i];
    cost -= deltaAdd(i);
    assert(cost == computeCost(this));
  }

  void flip(int i){
    if(used[i])
      remove(i);
    else
      add(i);
  }

  static ll computeCost(Solution* sol){
    ll weight = 0;
    FOR(i, sol->n)
      if(sol->used[i])
        weight += sol->instance->weights[i];
    assert(weight == sol->weight);
    ll cost = 0;
    FOR(i, sol->n)
      FOR(j, sol->n)
        cost += sol->instance->cost[i][j] * sol->used[i] * sol->used[j];
    return cost;
  }

  bool canAdd(int i){
    assert(!used[i]);
    return weight + instance->weights[i] <= instance->W;
  }
};


enum MethodLS {FirstImprovement, BestImprovement};
enum MethodGrasp {Classic, Bias, Pop};

class Neighborhood{
  public:
  Solution* anyImprovement(Solution* sol, chrono::steady_clock::time_point begin, MethodLS methodls, int timeLimit){
    Solution* bestSol = new Solution(*sol);
    while(true){
      auto now = chrono::steady_clock::now();
      auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
      int elapsedTime = elapsed.count();
      if(elapsedTime > timeLimit)
        break;
      Solution* neighbor = improvingNeighbor(bestSol, methodls);
      if(neighbor->cost <= bestSol->cost)
        break;
      swap(bestSol, neighbor);
      delete neighbor;
    }
    return bestSol;
  }

  virtual Solution* improvingNeighbor(Solution* bestSol, MethodLS methodls) = 0;
};

class FlipNeighborhood: public Neighborhood{
  Solution* improvingNeighbor(Solution* bestSol, MethodLS methodls) override{
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
};

class SwapNeighborhood: public Neighborhood{
  Solution* improvingNeighbor(Solution* bestSol, MethodLS methodls) override{
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
};

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
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
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
  const int seed = 0;
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
  const int seed = 0;
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
  const int seed = 0;
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

Solution* Grasp(Instance* instance, MethodGrasp methodGrasp, MethodLS methodls, double alpha, int timeLimit, int maxIterations){
  Solution* bestSol = NULL;
  auto begin = chrono::steady_clock::now();
  int curIteration = 0;

  while(true){
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
    int elapsedTime = elapsed.count();
    if(elapsedTime > timeLimit || curIteration >= maxIterations){
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

int main(){
  cin.tie(0)->sync_with_stdio(0);

  Instance* instance = readInstance();
  vector<double> alphas = {0.15, 0.3};
  const int timeLimit = 60000 * 30;
  const int maxIterations = 500;
  vector<MethodLS> methods = {FirstImprovement, BestImprovement};
  vector<MethodGrasp> methodsGrasp = {Classic, Bias, Pop};

  for(auto alpha : alphas)
    for(auto methodls : methods)
      for(auto methodGrasp: methodsGrasp){
        Solution* sol = Grasp(instance, methodGrasp, methodls, alpha, timeLimit, maxIterations);
        assert(sol->cost == Solution::computeCost(sol));
        assert(sol->weight <= instance->W);

        cout << setprecision(3) << fixed;
        switch (methodls){
          case BestImprovement:
            cout << "Best  Improvement - ";
            break;
          case FirstImprovement:
            cout << "First Improvement - ";
            break;
          default:
            throw runtime_error("Invalid Local Search Method");
        }
        switch (methodGrasp){
          case Classic:
            cout << "Classic - ";
            break;
          case Bias:
            cout << "Bias    - ";
            break;
          case Pop:
            cout << "Pop     - ";
            break;
          default:
            throw runtime_error("Invalid Grasp Variant");
        }
        cout <<  "alpha: " << alpha << " - Cost: " << sol->cost << " - Weight: " << sol->weight  << " - Time: " << sol->elapsedTime/1000.0 << "s - numIterations: " << sol->iterations << endl;
        delete sol;
      }

  delete instance;
}