#include "heuristic.hpp"

vii intersectSet(set<ii>& edges1, set<ii>& edges2){
  vii equals;
  set_intersection(all(edges1), all(edges2), back_inserter(equals));
  return equals;
}

struct Graph{
  int n;
  vvi adj, comps;
  vector<char> seen;
  vector<int> ans;
  vi level;

  Graph(int n): n(n), adj(n), seen(n), level(n){}

  void AddEdge(int u, int v){
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  bool dfs(int v, int p){
    seen[v] = true;
    for(int u : adj[v]){
      if(!seen[u]){
        if(dfs(u, v)){
          ans.push_back(u);
          return true;
        }
      }else if(u != p){
        ans.push_back(u);
        return true;
      }
    }
    return false;
  }

  vi FindCycle(){
    ans.clear();
    fill(all(seen), false);
    FOR(u, n)
      if(!seen[u] && dfs(u, -1)){
        // the cycle begin in u
        if(count(all(ans), u) == 1)
          ans.push_back(u);
        // the cycle does not begin in u
        while(ans.back() != ans[0])
          ans.pop_back();
        return ans;
      }
    return ans;
  }

  bool hasCycle(){
    FindCycle();
    return !ans.empty();
  }

  void RemoveEdge(int u, int v){
    assert(u != v);
    FOR(i, 2){
      auto iter1 = find(all(adj[v]), u);
      assert(iter1 != adj[v].end());
      adj[v].erase(iter1);
      swap(u, v);
    }
  }

  void dfsComp(int v){
    for(int u : adj[v]){
      if(level[u] == -1){
        level[u] = 1;
        comps.back().push_back(u);
        dfsComp(u);
      }
    }
  }

  void FindComps(){
    fill(all(level), -1);
    FOR(v, n){
      if(level[v] == -1){
        level[v] = 1;
        comps.push_back({v});
        dfsComp(v);
      }
    }
  }

  void FindExtremes(int& u, int& v){
    FindComps();
    vi ret;

    assert(SZ(comps) == 1);
    for(int v : comps[0]){
      assert(SZ(adj[v]) >= 1 && SZ(adj[v]) <= 2);
      if(SZ(adj[v]) == 1)
        ret.push_back(v);
    }
    u = ret[0], v = ret[1];
  }
};

void Heuristic::IncreaseIntersection(vi& tour1, vi& tour2, vvd& d1, vvd& d2){
	assert(SZ(tour1) == SZ(tour2));
  set<ii> edges1, edges2;
	int n = SZ(tour1);

  Graph G1(n), G2(n);
  FOR(i, n){
    G1.AddEdge(tour1[i], tour1[(i + 1) % n]);
    G2.AddEdge(tour2[i], tour2[(i + 1) % n]);
  }

	FOR(i, n){
		edges1.insert(minmax(tour1[i], tour1[(i + 1) % n]));
		edges2.insert(minmax(tour2[i], tour2[(i + 1) % n]));
	}

  struct Edge{
    double cost;
    int u, v, id;
    bool operator < (const Edge& e2) const{
      return cost < e2.cost;
    }
  };

  auto inters = intersectSet(edges1, edges2);
  if(SZ(inters) + 3 >= n){
    ld cost1 = 0, cost2 = 0;
    FOR(i, n){
      cost1 += d1[tour1[i]][tour1[(i + 1) % n]] + d2[tour1[i]][tour1[(i + 1) % n]];
      cost2 += d1[tour2[i]][tour2[(i + 1) % n]] + d2[tour2[i]][tour2[(i + 1) % n]];
    }
    if(cost1 < cost2)
      tour2 = tour1;
    else
      tour1 = tour2;
    return;
  }

  int oldSizeInters = SZ(inters);
  multiset<Edge> mp;
  for(auto e : edges1)
    if(!binary_search(all(inters), e))
      mp.insert({d1[e.first][e.second], e.first, e.second, 1});
  
  for(auto e : edges2)
    if(!binary_search(all(inters), e))
      mp.insert({d2[e.first][e.second], e.first, e.second, 2});
  
  struct PairEdges{
    int nl, nr;
    double cost;
  };
  PairEdges best = {-1, -1, INT_MAX};
  int vl, vr;
  bool swapped = false;
  
  for(Edge chosen : mp){
    swapped = false;
    if(chosen.id == 2){
      swapped = true;
      swap(tour1, tour2);
      swap(edges1, edges2);
      swap(d1, d2);
      swap(G1, G2);
    }
    vl = chosen.u;
    vr = chosen.v;
    G2.AddEdge(vl, vr);
    ii edgeLR = minmax(vl, vr);
    assert(!count(all(inters), edgeLR));
    inters.insert(lower_bound(all(inters), edgeLR), edgeLR);
    
    vi neighborL, neighborR;
    for(int v : G2.adj[vl]){
      ii edgeVL = minmax(v, vl);
      if(!count(all(inters), edgeVL))
        neighborL.push_back(v);
    }

    for(int v : G2.adj[vr]){
      ii edgeVR = minmax(v, vr);
      if(!count(all(inters), edgeVR))
        neighborR.push_back(v);
    }
    
    assert(SZ(neighborL) <= 2 && SZ(neighborR) <= 2);

    best = {-1, -1, INT_MAX};
    for(int nl : neighborL)
      for(int nr : neighborR){
        double cost = d2[nl][vl] + d2[nr][vr];
        if(best.cost > cost){
          G2.RemoveEdge(nl, vl);
          G2.RemoveEdge(nr, vr);
          if(!G2.hasCycle())
            best = {nl, nr, cost};
          G2.AddEdge(nl, vl);
          G2.AddEdge(nr, vr);
        }
      }

    if(best.nl != -1)
      break;
    G2.RemoveEdge(vl, vr);
    inters.erase(lower_bound(all(inters), edgeLR));
    if(swapped){
      swap(tour1, tour2);
      swap(edges1, edges2);
      swap(d1, d2);
      swap(G1, G2);
    }
  }
  assert(best.nl != -1);

  G2.RemoveEdge(best.nl, vl);
  G2.RemoveEdge(best.nr, vr);
  assert(!G2.hasCycle());

  int U, V;
  G2.FindExtremes(U, V);
  G2.AddEdge(U, V);

  tour2 = G2.FindCycle();
  assert(SZ(tour2) == n + 1);
  tour2.pop_back();
  edges2.clear();
  FOR(i, n)
		edges2.insert(minmax(tour2[i], tour2[(i + 1) % n]));

  if(swapped){
    swap(tour1, tour2);
    swap(edges1, edges2);
    swap(d1, d2);
    swap(G1, G2);
  }

  inters = intersectSet(edges1, edges2);
  assert(SZ(inters) > oldSizeInters);
}
