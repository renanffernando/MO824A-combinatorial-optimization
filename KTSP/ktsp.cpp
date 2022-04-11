/* Copyright 2021, Gurobi Optimization, LLC */

/* Solve a traveling salesman problem on a randomly generated set of
   points using lazy constraints.   The base MIP model only includes
   'degree-2' constraints, requiring each node to have exactly
   two incident edges.  Solutions to this model may contain subtours -
   tours that don't visit every node.  The lazy constraint callback
   adds new constraints to cut them off. */

#include "/opt/gurobi951/linux64/include/gurobi_c++.h"
#include "subTourElim.hpp"

void findsubtour(int n, double** sol, int& tourlenP, vi& tour);

// Subtour elimination callback.  Whenever a feasible solution is found,
// find the smallest subtour, and add a subtour elimination constraint
// if the tour doesn't visit every node.
class subtourelim: public GRBCallback{
  public:
    vector<vector<GRBVar>>& vars1, vars2;
    int n, cnt = 0;
    subtourelim(vector<vector<GRBVar>>& vars1, vector<vector<GRBVar>>& vars2, int n): vars1(vars1), vars2(vars2), n(n) {}
  protected:
    void callback() {
      try {
        if (where == GRB_CB_MIPSOL || (where == GRB_CB_MIPNODE && getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL)) {
          // Found an integer feasible solution - does it visit every node?
          cnt++;
          double **x1 = new double*[n], **x2 = new double*[n];
          vi tour1(n), tour2(n);
          int len1, len2;
          if(where == GRB_CB_MIPSOL){
            FOR(i, n){
              x1[i] = getSolution(vars1[i].data(), n);
              x2[i] = getSolution(vars2[i].data(), n);
            }
          }else{
            FOR(i, n){
              x1[i] = getNodeRel(vars1[i].data(), n);
              x2[i] = getNodeRel(vars2[i].data(), n);
            }
          }

          if(where == GRB_CB_MIPSOL){
            findsubtour(n, x1, len1, tour1);
            findsubtour(n, x2, len2, tour2);
          }
          else{
            SubTourSeparation::find(n, x1, len1, tour1);
            SubTourSeparation::find(n, x2, len2, tour2);
          }

           if (len1 < n) {
            // Add subtour elimination constraint
            GRBLinExpr expr = 0;
            FOR(i, len1)
              FOR(j, i)
                expr += vars1[tour1[i]][tour1[j]];
            addLazy(expr, GRB_LESS_EQUAL, len1 - 1);
          }

          if (len2 < n) {
            // Add subtour elimination constraint
            GRBLinExpr expr = 0;
            FOR(i, len2)
              FOR(j, i)
                expr += vars2[tour2[i]][tour2[j]];
            addLazy(expr, GRB_LESS_EQUAL, len2 - 1);
          }

          FOR(i, n){
            delete[] x1[i];
            delete[] x2[i];
          }
          delete[] x1;
          delete[] x2;
        }
      } catch (GRBException e) {
        cout << "Error number: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
      } catch (...) {
        cout << "Error during callback" << endl;
      }
    }
};

// Given an integer-feasible solution 'sol', find the smallest
// sub-tour.  Result is returned in 'tour', and length is
// returned in 'tourlenP'.
void findsubtour(int n, double** sol, int& tourlenP, vi& tour){
  bool* seen = new bool[n];
  int bestind, bestlen;
  int i, node, len, start;

  FOR(i, n){
    FOR(j, n)
      assert(fabs(sol[i][j] - sol[j][i]) < EPS);
    assert(fabs(2.0 - accumulate(sol[i], sol[i] + n, 0.0)) < EPS);
  }
  fill(seen, seen + n, 0);

  start = 0;
  bestlen = n+1;
  bestind = -1;
  node = 0;
  while (start < n) {
    for (node = 0; node < n; node++)
      if (!seen[node])
        break;
    if (node == n)
      break;
    for (len = 0; len < n; len++) {
      tour[start+len] = node;
      seen[node] = true;
      vector<double> ss(sol[node], sol[node] + n);
      for (i = 0; i < n; i++) {
        if (sol[node][i] > 0.5 && !seen[i]) {
          node = i;
          break;
        }
      }
      if (i == n) {
        len++;
        if (len < bestlen) {
          bestlen = len;
          bestind = start;
        }
        start += len;
        break;
      }
    }
  }

  for (i = 0; i < bestlen; i++)
    tour[i] = tour[bestind+i];
  tourlenP = bestlen;

  delete[] seen;
}

int main(){
  int n, m, k;
  cin >> n >> m >> k;

  vvd d1(n, vd(n, DBL_MAX)), d2(n, vd(n, DBL_MAX));

  FOR(i, m){
    int u, v;
    double dist1, dist2;
    cin >> u >> v >> dist1 >> dist2;
    d1[u][v] = d1[v][u] = dist1;
    d2[u][v] = d2[v][u] = dist2;
  }
  FOR(i, n)
    d1[i][i] = d2[i][i] = 0;

  FOR(i, n)
    FOR(j, n)
      assert(d1[i][j] < DBL_MAX && d2[i][j] < DBL_MAX);
  
  try{
    GRBEnv env(true);
    //env.set(GRB_IntParam_Threads, 1);
    env.set(GRB_DoubleParam_TimeLimit, 1800);
    env.set(GRB_IntParam_Presolve, 0);
    env.set(GRB_IntParam_Method, 1);
    env.set(GRB_IntParam_NumericFocus, 1);
    env.set(GRB_IntParam_Cuts, 3);
    env.start();

    GRBModel model(env);
    model.set(GRB_IntParam_LazyConstraints, 1);

    vector<vector<GRBVar>> vars1(n, vector<GRBVar>(n)), vars2(n, vector<GRBVar>(n)), same(n, vector<GRBVar>(n));

    FOR(i, n)
      FOR(j, i + 1){
        vars1[i][j] = vars1[j][i] = model.addVar(0.0, 1.0, d1[i][j], GRB_BINARY, "x1_" + to_string(i) + "_" + to_string(j));
        vars2[i][j] = vars2[j][i] = model.addVar(0.0, 1.0, d2[i][j], GRB_BINARY, "x2_" + to_string(i) + "_" + to_string(j));
        same[i][j] = same[j][i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "same_" + to_string(i) + "_" + to_string(j));
      }

    // Degree-2 constraints
    FOR(i, n){
      GRBLinExpr expr1 = 0, expr2 = 0;
      FOR(j, n){
        expr1 += vars1[i][j];
        expr2 += vars2[i][j];
      }
      model.addConstr(expr1, GRB_EQUAL, 2, "deg1_" + to_string(i));
      model.addConstr(expr2, GRB_EQUAL, 2, "deg2_" + to_string(i));
    }

    // Forbid edge from node back to itself
    FOR(i, n){
      vars1[i][i].set(GRB_DoubleAttr_UB, 0);
      vars2[i][i].set(GRB_DoubleAttr_UB, 0);
      same[i][i].set(GRB_DoubleAttr_UB, 0);
    }

    FOR(i, n)
      FOR(j, i + 1){
        model.addConstr(same[i][j] <= vars1[i][j], "s1_" + to_string(i) + "_" + to_string(j));
        model.addConstr(same[i][j] <= vars2[i][j], "s2_" + to_string(i) + "_" + to_string(j));
      }

    {
      // Constraint of least k edges equals
      GRBLinExpr expr = 0.0;
      FOR(i, n)
        FOR(j, i)
          expr += same[i][j];
      model.addConstr(expr, GRB_GREATER_EQUAL, k, "k-same");
    }

    // Set callback function
    subtourelim cb(vars1, vars2, n);
    model.setCallback(&cb);
    {
      // initial solution
      FOR(i, n)
        FOR(j, i + 1){
          vars1[i][j].set(GRB_DoubleAttr_Start, 0.0);
          vars2[i][j].set(GRB_DoubleAttr_Start, 0.0);
          same[i][j].set(GRB_DoubleAttr_Start, 0.0);
        }
      int cur = 0, len = 0;
      vi seen(n, false);
      seen[cur] = true;
        
      while(len != n){
        map<double, int> nearest;
        FOR(nxt, n)
          if(!seen[nxt])
            nearest[max(d1[cur][nxt], d2[cur][nxt])] = nxt;
        int nxt = nearest.empty() ? 0 : nearest.begin()->second;
        vars1[cur][nxt].set(GRB_DoubleAttr_Start, 1.0);
        vars2[cur][nxt].set(GRB_DoubleAttr_Start, 1.0);
        same[cur][nxt].set(GRB_DoubleAttr_Start, 1.0);
        cur = nxt;
        seen[cur] = true;
        len++;
      }
    }

    // Optimize model
    model.optimize();

    // Extract solution
    if (model.get(GRB_IntAttr_SolCount) > 0) {
      double **sol1 = new double*[n], **sol2 = new double*[n];
      FOR(i, n){
        sol1[i] = model.get(GRB_DoubleAttr_X, vars1[i].data(), n);
        sol2[i] = model.get(GRB_DoubleAttr_X, vars2[i].data(), n);
      }
      vector<int> tour1(n), tour2(n);
      int len1, len2;
      findsubtour(n, sol1, len1, tour1);
      findsubtour(n, sol2, len2, tour2);
      assert(len1 == n && len2 == n);

      set<pair<int, int>> edges1, edges2;
      FOR(i, n){
        edges1.insert(minmax(tour1[i], tour1[(i + 1) % n]));
        edges2.insert(minmax(tour2[i], tour2[(i + 1) % n]));
      }
      
      vector<pair<int, int>> equals;
      set_intersection(all(edges1), all(edges2), back_inserter(equals));
      assert(SZ(equals) >= k);

      cout << "Tour1: ";
      FOR(i, len1)
        cout << tour1[i] << " ";
      cout << endl;

      cout << "Tour2: ";
      FOR(i, len2)
        cout << tour2[i] << " ";
      cout << endl;

      FOR(i, n){
        delete[] sol1[i];
        delete[] sol2[i];
      }
      delete[] sol1;
      delete[] sol2;
    }

  } catch (GRBException e) {
    cout << "Error number: " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during optimization" << endl;
  }

  return 0;
}
