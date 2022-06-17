#include "GA.hpp"

GA::GA(Instance *instance, GAType gaType, ld mutationRate, int popSize, int timeLimit, int maxGenerations) :
 instance(instance), gaType(gaType), mutationRate(mutationRate), popSize(popSize), timeLimit(timeLimit), maxGenerations(maxGenerations), rng(0) {}

const bool print = false;

Solution* GA::buildInitial(Instance* instance, double alpha){
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

void GA::buildInitialPopulation() {
  while(SZ(population) < popSize)
    population.push_back(buildInitial(instance, gaType == Random ? 1.0 : 0.2));
  sort(all(population), Solution::greaterSol());
}

vector<Solution*> GA::getParents(){
  if(gaType == Stochastic)
    shuffle(all(population), rng);
  else
    sort(all(population), Solution::greaterSol());
  
  double propToSurvive = 0.2;
  vector<Solution*> parents;

  FOR(i, propToSurvive * SZ(population))
    parents.push_back(new Solution(*population[i]));
  return parents;
}

vi subsetSum(vi v, int WMin, int WMax, bool& find){
  int n = SZ(v), W = WMax;
  vector<vi> dp (n + 1, vi(W + 1));
  dp[0][0] = 1;
  FOR(i, n){
    FOR(w, W + 1){
      dp[i + 1][w] = dp[i][w];
      if(v[i] <= w)
        dp[i + 1][w] |= dp[i][w - v[i]];
    }
  }
  vi idx;
  for(int h = W; h >= WMin; h--){
    if(dp[n][h]){
      find = true;
      int sum = 0, w = h;
      for(int i = n; i > 0; i--){
        assert(dp[i][w]);
        if(v[i - 1] <= w && dp[i - 1][w - v[i - 1]]){
          sum += v[i - 1];
          idx.push_back(i - 1);
          w -= v[i - 1];
        }
      }
      assert(w == 0);      
      break;
    }
  }
  return idx;
}

void GA::mutation(vector<Solution*>& parents){
  for(auto sol : parents){
    int sz = 0;
    for(int e : sol->used)
      sz += e;
    int qt = ceil(mutationRate * sz);

    FOR(q, qt){
      uniform_int_distribution<> genB(0, 1);
      if(genB(rng)){
        vi valids;
        FOR(i, sol->n){
          if(sol->used[i] || sol->canAdd(i))
            valids.push_back(i);
        }
        assert(!valids.empty());
        uniform_int_distribution<> gen(0, SZ(valids) - 1);
        sol->flip(valids[gen(rng)]);
      }else{
        int maxTry = 10;
        FOR(h, maxTry){
          vi useds;
          FOR(i, sol->n)
            if(sol->used[i])
              useds.push_back(i);
          uniform_int_distribution<> gen(0, SZ(useds) - 1);
          int out = useds[gen(rng)];
          vi canIn;
          FOR(i, sol->n)
            if(!sol->used[i] && sol->getWeight() - instance->weights[out] + instance->weights[i] <= instance->W)
              canIn.push_back(i);
          if(canIn.empty())
            continue;

          uniform_int_distribution<> gen2(0, SZ(canIn) - 1);
          int in = canIn[gen2(rng)];
          sol->remove(out);
          sol->add(in);
          assert(sol->getWeight() <= instance->W);
          break;
        }
      }
    }
  }
}

vector<Solution*> GA::crossover(vector<Solution*> parents){
  vector<Solution*> offspring;

  while(SZ(offspring) < popSize){
    uniform_int_distribution<> gen(0, SZ(parents) - 1);
    int p1 = gen(rng), p2 = p1;
    while(p1 == p2)
      p2 = gen(rng);
    
    Solution* sol1 = parents[p1], *sol2 = parents[p2];

    vi diff12, diff21;
    FOR(i, sol1->n)
      if(sol1->used[i] ^ sol2->used[i])
        (sol1->used[i] ? diff12 : diff21).push_back(i);
    if(diff12.empty() && diff21.empty())
      continue;
    if(diff12.empty()){
      swap(diff12, diff21);
      swap(sol1, sol2);
    }
    
    bool find = false;
    vi idx;
    int maxTry = 10, r1 = 0;

    FOR(t, maxTry){
      shuffle(all(diff12), rng);
      shuffle(all(diff21), rng);
      int W1 = 0;
      {
        uniform_int_distribution<> gen2(1, SZ(diff12));
        r1 = gen2(rng);
        FOR(i, r1)
          W1 += instance->weights[diff12[i]];
      }
      int res1 = instance->W - sol1->getWeight(), res2 = instance->W - sol2->getWeight();
      int Wmin = W1 - res2, WMax = W1 + res1;
      
      vi weights;
      for(int id : diff21)
        weights.push_back(instance->weights[id]);
      
      idx = subsetSum(weights, Wmin, WMax, find);
      if(find){
        for(int& id : idx)
          id = diff21[id];
        break;
      }
    }
    if(!find)
      continue;
    Solution* child1 = new Solution(*sol1), *child2 = new Solution(*sol2);
    FOR(i, r1)
      child1->remove(diff12[i]);
    for(int id : idx)
      child1->add(id);
    assert(child1->getWeight() <= instance->W);

    for(int id : idx)
      child2->remove(id);
    FOR(i, r1)
      child2->add(diff12[i]);

    assert(child2->getWeight() <= instance->W);
    {
      bool has = false;
      if(gaType == Diversity)
        for(auto e : offspring)
          if(e->value == child1->value && e->used == child1->used){
            has = true;
            break;
          }
      if(!has)
        offspring.push_back(child1);
    }
    {
      bool has = false;
      if(gaType == Diversity)
        for(auto e : offspring)
          if(e->value == child2->value && e->used == child2->used){
            has = true;
            break;
          }
      if(!has)
        offspring.push_back(child2);
    }
  }
  return offspring;
}

void GA::nextGeneration(){
  bestSol = *min_element(all(population), Solution::greaterSol());

  auto parents = getParents();
  mutation(parents);
  parents.push_back(new Solution(*bestSol));
  
  auto offsprint = crossover(parents);
  if((*min_element(all(offsprint), Solution::greaterSol()))->getCost() < bestSol->getCost())
    offsprint.push_back(new Solution(*bestSol));
  
  free(population);
  free(parents);
  sort(all(offsprint), Solution::greaterSol());

  if(print && generation % 100 == 0)
    cout << "Generation: " << generation << " - BestCost: " << offsprint[0]->getCost() << endl;
  
  population = offsprint;
}

void GA::free(vector<Solution*>& v){
  for(auto sol : v)
    delete sol;
}

Solution *GA::run() {
  begin = chrono::steady_clock::now();
  buildInitialPopulation();
  int bestCostInitial = (*min_element(all(population), Solution::greaterSol()))->getCost();

  generation = 0;
  if(print)
    cout << "Generation: " << generation << " - BestCost: " << bestCostInitial << endl;

  FOR(h, maxGenerations){
    generation++;
    nextGeneration();
    if(getTime() >= timeLimit)
      break;
  }

  bestSol = new Solution(*population[0]);
  free(population);

  bestSol->elapsedTime = getTime();
  bestSol->generations = generation;
  bestSol->bestInitial = bestCostInitial;

  return bestSol;
}

int GA::getTime() {
  auto now = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - begin);
  return elapsed.count();
}
