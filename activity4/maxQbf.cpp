#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using vll = vector<ll>;
using vvll = vector<vll>;
using vi = vector<int>;
using ii = pair<int, int>;
#define FOR(i, b) for(int i = 0; i < (b); i++)
#define SZ(a) ((int) a.size())
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
    
    ll cMax = costsSet.begin()->first;
    ll cMin = prev(costsSet.end())->first;
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

enum Method {FirstImprovement, BestImprovement};

class Neighborhood{
  public:
  Solution* anyImprovement(Solution* sol, chrono::steady_clock::time_point begin, Method method, int timeLimit){
    Solution* bestSol = new Solution(*sol);
    while(true){
      auto now = chrono::steady_clock::now();
      auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
      int elapsedTime = elapsed.count();
      if(elapsedTime > timeLimit)
        break;
      Solution* neighbor = improvingNeighbor(bestSol, method);
      if(neighbor->cost <= bestSol->cost)
        break;
      swap(bestSol, neighbor);
      delete neighbor;
    }
    return bestSol;
  }

  virtual Solution* improvingNeighbor(Solution* bestSol, Method method) = 0;
};

class FlipNeighborhood: public Neighborhood{
  Solution* improvingNeighbor(Solution* bestSol, Method method) override{
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
        if (method == FirstImprovement)
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
  Solution* improvingNeighbor(Solution* bestSol, Method method) override{
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
          if (method == FirstImprovement)
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

void localSearch(Solution*& bestSol, chrono::steady_clock::time_point begin, Method method, int timeLimit){
  vector<Neighborhood*> neighborhoods;
  neighborhoods.push_back(new FlipNeighborhood());
  neighborhoods.push_back(new SwapNeighborhood());

  int k = 0;
  while(k < SZ(neighborhoods)){
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
    int elapsedTime = elapsed.count();
    if(elapsedTime > timeLimit)
      break;

    Solution* localOptimal = neighborhoods[k]->anyImprovement(bestSol, begin, method, timeLimit);

    if (bestSol->cost < localOptimal->cost){
      swap(bestSol, localOptimal);
      delete localOptimal;
      k = k == 0 ? 1 : 0;
    }else
      k++;
  }
}

Solution* Grasp(Instance* instance, Method method, double alpha, int timeLimit, int maxIterations){
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

    Solution* sol = buildInitial(instance, alpha);
    localSearch(sol, begin, method, timeLimit);

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
  vector<double> alphas = {0.1, 0.2, 0.3};
  const int timeLimit = 60000;
  const int maxIterations = 500;
  vector<Method> methods = {FirstImprovement, BestImprovement};

  for(auto alpha : alphas)
    for(auto method : methods){
      Solution* sol = Grasp(instance, method, alpha, timeLimit, maxIterations);
      assert(sol->cost == Solution::computeCost(sol));
      assert(sol->weight <= instance->W);

      cout << setprecision(3) << fixed;
      if (method == BestImprovement)
        cout <<  "BestImprovement  - alpha: " << alpha << " - Cost: " << sol->cost << " - Weight: " << sol->weight  << " - Time: " << sol->elapsedTime/1000.0 << "s - numIterations: " << sol->iterations << endl;
      else if (method == FirstImprovement)
        cout <<  "FirstImprovement - alpha: " << alpha << " - Cost: " << sol->cost << " - Weight: " << sol->weight  << " - Time: " << sol->elapsedTime/1000.0 << "s - numIterations: " << sol->iterations << endl;
      delete sol;
    }

  delete instance;
}