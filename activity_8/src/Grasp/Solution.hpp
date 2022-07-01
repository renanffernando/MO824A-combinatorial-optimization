#ifndef SOLUTION_H
#define SOLUTION_H
#include "Instance.hpp"

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

#endif
