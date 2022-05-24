#ifndef STE_H
#define STE_H

#include <bits/stdc++.h>

using namespace std;
using vi = vector<int>;
using ld = double;
using vd = vector<ld>;
using vvd = vector<vd>;

#define all(a) a.begin(), a.end()
#define EPS 1e-4
#define FOR(i, b) for(int i = 0; i < (b); i++)
#define SZ(a) ((int) a.size())

namespace SubTourSeparation{
    void find(int n, double** sol, int& tourlenP, vi& tour);
}

#endif