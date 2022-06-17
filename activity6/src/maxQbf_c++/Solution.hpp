#ifndef SOLUTION_H
#define SOLUTION_H
#include "Instance.hpp"

struct Value{
  ll cost, weight;

  Value (ll cost, ll weight): cost(cost), weight(weight) {}

  bool operator > (const Value& v2) const{
    return cost > v2.cost;
    //return make_pair(cost, -weight) > make_pair(v2.cost, -v2.weight);
  }

  bool operator <= (const Value& v2) const{
    return !(*this > v2);
  }

  bool operator == (const Value& v2) const{
    return make_pair(cost, weight) == make_pair(v2.cost, v2.weight);
  }

  Value operator + (const Value& v2) const{
    return Value(cost + v2.cost, weight + v2.weight);
  }

  Value operator += (const Value& v2){
    return *this = *this + v2;
  }

  Value operator - (const Value& v2) const{
    return Value(cost - v2.cost, weight - v2.weight);
  }

  Value operator -= (const Value& v2){
    return *this = *this - v2;
  }

  Value operator *= (const int& m){
    cost *= m;
    weight *= m;
    return *this;
  }
};

struct Solution{
  Value value;
  Instance* instance;
  int n, elapsedTime, generations;
  vi used;
  Solution(Instance* instance): value(0, 0), instance(instance), n(instance->n), elapsedTime(0), generations(0), used(n, 0){}

  void add(int i){
    assert(!used[i]);
    assert(canAdd(i));
    used[i] = 1;
    value += deltaAdd(i);
    assert(value == computeValue(this));
  }

  Value deltaAdd(int i){
    Value delta(0, instance->weights[i]);
    FOR(j, n)
      if(i != j && used[j]){
        delta.cost += instance->cost[i][j];
        delta.cost += instance->cost[j][i];
      }
    delta.cost += instance->cost[i][i];
    return delta;
  }

  void remove(int i){
    assert(used[i]);
    used[i] = 0;
    value -= deltaAdd(i);
    assert(value == computeValue(this));
  }

  void flip(int i){
    if(used[i])
      remove(i);
    else
      add(i);
  }

  ll getCost(){
    return value.cost;
  }

  ll getWeight(){
    return value.weight;
  }

  static Value computeValue(Solution* sol){
    ll weight = 0;
    FOR(i, sol->n)
      if(sol->used[i])
        weight += sol->instance->weights[i];
    assert(weight == sol->getWeight());
    ll cost = 0;
    FOR(i, sol->n)
      FOR(j, sol->n)
        cost += sol->instance->cost[i][j] * sol->used[i] * sol->used[j];
    return Value(cost, weight);
  }

  bool canAdd(int i){
    assert(!used[i]);
    return value.weight + instance->weights[i] <= instance->W;
  }

  struct greaterSol{
    bool operator () (const Solution* s1, const Solution* s2){
      return s1->value > s2->value;
    }
  };
};

#endif
