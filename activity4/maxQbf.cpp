#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using vll = vector<ll>;
using vvll = vector<vll>;
using vi = vector<int>;
#define FOR(i, b) for(int i = 0; i < (b); i++)
#define SZ(a) ((int) a.size())

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
  int n, elapsedTime;
  vi used;
  ll cost, weight;
  Solution(Instance* instance): instance(instance), n(instance->n), elapsedTime(0), used(n, 0), cost(0), weight(0){}

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
    cost -= deltaAdd(i);
    assert(cost == computeCost(this));
  }

  static ll computeCost(Solution* sol){
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

Solution* Grasp(Instance* instance, double alpha, int timeLimit, int maxIterations){
  Solution* bestSol = NULL;
  auto begin = chrono::steady_clock::now();
  int curIteration = 0;

  while(true){
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
    int elapsedTime = elapsed.count();
    if(elapsedTime > timeLimit || curIteration > maxIterations){
      assert(bestSol != NULL);
      bestSol->elapsedTime = elapsedTime;
      break;
    }

    Solution* sol = buildInitial(instance, alpha);
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
  vector<double> alphas = {0.2};
  const int timeLimit = 60000;
  const int maxIterations = 10;

  for(auto alpha : alphas){
    Solution* sol = Grasp(instance, alpha, timeLimit, maxIterations);
    assert(sol->cost == Solution::computeCost(sol));
    assert(sol->weight <= instance->W);

    cout << "Cost: " << sol->cost << " - Weight: " << sol->weight << endl;
    delete sol;
  }

  delete instance;
}