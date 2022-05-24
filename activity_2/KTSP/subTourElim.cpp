#include "subTourElim.hpp"

using namespace SubTourSeparation;

template <class T = int>
struct MinCut{
  constexpr static T INF = numeric_limits<T>::max() / 2;

  struct FlowEdge {
    int v, u;
    T cap, flow = 0;
    FlowEdge(int v, int u, T cap) : v(v), u(u), cap(cap) {}
  };

  struct Dinic {
    const static bool SCALING = true;
    vector<FlowEdge> edges;
    vector<vi> adj;
    vi level, ptr;
    vector<vi> comps;
    queue<int> q;
    int n, s, t, m;
    T lim = 0;

    Dinic(int n, int s, int t):
      adj(n), level(n), ptr(n), n(n), s(s), t(t), m(0) {}

    void AddEdge(int v, int u, T cap, bool isDirected = true) {
      edges.emplace_back(v, u, cap);
      edges.emplace_back(u, v, isDirected ? 0 : cap);
      adj[v].push_back(m);
      adj[u].push_back(m + 1);
      m += 2;
    }

    void Update(int t){
      this->t = t;
      for(auto& e : edges)
        e.flow = 0;
    }

    void minComponent(vi& comp){
      fill(all(level), -1);
      FOR(v, n){
        if(level[v] == -1){
          level[v] = 1;
          comps.push_back({v});
          dfsMC(v);
        }
      }
      comp = comps[0];
      for(vi& c : comps)
        if(SZ(c) < SZ(comp))
          comp = c;
    }

    void dfsMC(int v){
      for(int id : adj[v]){
        int u = edges[id].u;
        if(level[u] == -1){
          level[u] = 1;
          comps.back().push_back(u);
          dfsMC(u);
        }
      }
    }

    bool bfs() {
      fill(all(level), -1);
      level[s] = 0;
      q.push(s);

      while (!q.empty()) {
        int v = q.front(); q.pop();
        for (int id : adj[v]) {
          auto& e = edges[id];
          if (level[e.u] == -1 && (e.cap - e.flow) > EPS && (!SCALING || e.cap - e.flow >= lim)){
            level[e.u] = level[v] + 1;
            q.push(e.u);
          }
        }
      }
      return level[t] != -1;
    }

    T dfs(int v, T pushed) {
      if (v == t || !pushed)
        return pushed;
      for (int& cid = ptr[v]; cid < (int)adj[v].size(); cid++) {
        int id = adj[v][cid];
        auto& e = edges[id];
        if (level[v] + 1 != level[e.u])
          continue;
        if (T tr = dfs(e.u, min(pushed, e.cap - e.flow))){
          e.flow += tr;
          edges[id ^ 1].flow -= tr;
          return tr;
        }
      }
      return 0;
    }

    T Solve() {
      T f = 0, pushed;
      for (lim = SCALING ? 1 - EPS : EPS; 2 * lim > EPS; lim /= 2) {
        while ((f < 2 - n * EPS) && bfs()) {
          fill(all(ptr), 0);
          while ((f < 2 - n * EPS) && (pushed = dfs(s, INF)))
            f += pushed;
        }
      }
      return f;
    }
  };

  int n, s, t;
  vector<char> seen;
  Dinic dinic;

  MinCut(int n, int s, int t): n(n), s(s), t(t), seen(n, false), dinic(n, s, t) {}

  void AddEdge(int v, int u, T cap, bool isDirected = true) {
    dinic.AddEdge(v, u, cap, isDirected);
  }

  void Update(int t){
    this->t = t;
    dinic.Update(t);
  }

  void minComponent(vi& cut){
    dinic.minComponent(cut);
  }

  bool Solve(vi& cut) {
    ld flow = dinic.Solve();
    assert(flow <= 2 + EPS);

    if(flow >= 2 - n * EPS)
      return false;

    stack<int> q;
    q.push(t);
    fill(all(seen), false);
    seen[t] = true;

    // find vertex set of the side of the sink
    while(!q.empty()){
      int v = q.top(); q.pop();
      for(auto id : dinic.adj[v]){
        auto e = dinic.edges[id];
        if (!seen[e.u] && (e.cap - e.flow) > EPS){
          seen[e.u] = true;
          q.push(e.u);
        }
      }
    }
    FOR(v, n)
      if(seen[v])
        cut.push_back(v);
    return true;
  }
};

void SubTourSeparation::find(int n, double** sol, int& tourlenP, vi& tour){
  tourlenP = n;
  FOR(i, n)
    tour[i] = i;

  vvd mat(n, vd(n));
  ld acum = 0;
  FOR(i, n){
    copy(sol[i], sol[i] + n, mat[i].data());
    acum = accumulate(all(mat[i]), acum);
  }
  assert(fabs(acum - 2 * n) < EPS);

  MinCut<double> minCut(n, 0, 0);
  FOR(i, n)
    FOR(j, i)
      if(sol[i][j] > EPS)
        minCut.AddEdge(i, j, sol[i][j], false);

  vi cut;
  minCut.minComponent(cut);
  if(SZ(cut) < n){
    copy(all(cut), tour.begin());
    tourlenP = SZ(cut);
    return;
  }
  const bool useMinCut = false;
  if(!useMinCut)
    return;

  for(int t = 1; t < n; t++){
    cut.clear();
    minCut.Update(t);
    if(minCut.Solve(cut) && SZ(cut) < tourlenP){
      copy(all(cut), tour.begin());
      tourlenP = SZ(cut);
    }
  }
  assert(tourlenP >= 0 && tourlenP <= n);
}
