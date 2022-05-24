#ifndef INSTANCE_H
#define INSTANCE_H

#include "util.hpp"

struct Instance{
  int n;
  ll W;
  vvll cost;
  vll weights;
  Instance(int n, ll W, vvll cost, vll weights): n(n), W(W), cost(cost), weights(weights){}

  static Instance* readInstance(){
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
};

#endif
