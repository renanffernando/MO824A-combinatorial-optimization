#include "heuristic.hpp"

vi completeTour(vvd& cost, vi isEnd, vvi tours, vi oldTour){
  struct Edge{
    int u, v;
    ld cost;
    bool type;
    bool operator < (const Edge& e) const{
      return type != e.type? type < e.type:
      (cost != e.cost ? cost < e.cost : make_pair(u, v) < make_pair(e.u, e.v));
    }
  };
  
  int n = SZ(oldTour);
  vvi type(n, vi(n, 1));
  FOR(i, n){
    int u = oldTour[i], v = oldTour[(i + 1) % n];
    if(u > v) swap(u, v);
    type[u][v] = 0;
  }

  vector<Edge> edges;
  FOR(i, n)
    for(int j = i + 1; j < n; j++)
      edges.push_back({i, j, cost[i][j], (bool)type[i][j]});

  sort(all(edges));
  
  for(auto e : edges){
    if(!isEnd[e.u] || !isEnd[e.v] || tours[e.u] == tours[e.v])
      continue;
    int sz0 = SZ(tours[e.u]), sz1 = SZ(tours[e.v]);
    if(tours[e.u][0] == e.u)
      reverse(all(tours[e.u]));
    if(tours[e.v].back() == e.v)
      reverse(all(tours[e.v]));
    
    int bg = tours[e.u][0], end = tours[e.v].back();
    tours[bg]  = tours[e.u];
    tours[end]  = tours[e.v];
    isEnd[e.u] = isEnd[e.v] = false;

    tours[bg].insert(tours[bg].end(), all(tours[end]));
    tours[end] = tours[bg];
    if(SZ(tours[end]) != sz0 + sz1)
      throw exception();
    isEnd[bg] = isEnd[end] = true;
  }
  vi temp;
  FOR(i, n)
    if(isEnd[i])
      temp.push_back(i);
  assert(SZ(temp) == 2 && tours[temp[0]] == tours[temp[1]]);
  assert(SZ(oldTour) == SZ(tours[temp[0]]));
  return tours[temp[0]];
}

vii similarEdges(
  const vi & tour0,
  const vi & tour1
) {
  assert(SZ(tour0) == SZ(tour1));
  int n = SZ(tour0);
  set<ii> edges0, edges1;

	FOR(i, n){
		edges0.insert(minmax(tour0[i], tour0[(i + 1) % n]));
		edges1.insert(minmax(tour1[i], tour1[(i + 1) % n]));
	}

  vii equals;
  set_intersection(all(edges0), all(edges1), back_inserter(equals));
  return equals;
}

void Heuristic::makeFeasibleSolution (
  vi & tour0,
  vi & tour1,
  vvd & cost0,
  vvd & cost1,
  const int similarityParameter
) {

  struct Edge{
    int u, v;
    ld cost;
    bool type;
    bool operator < (const Edge& e) const{
      return type != e.type ? type < e.type:
        (cost != e.cost ? cost < e.cost : make_pair(u, v) < make_pair(e.u, e.v));
    }
  };
  
  int n = SZ(tour0);
  auto equals = similarEdges(tour0, tour1);
  
  vvi type(n, vi(n, 1));

  for(auto e : equals)
    type[e.first][e.second] = 0;

  vector<Edge> edges;
  FOR(i, n)
    for(int j = i + 1; j < n; j++)
      edges.push_back({i, j, cost0[i][j] + cost1[i][j], (bool) type[i][j]});

  sort(all(edges));

  vi isEnd(n, 1);
  vvi tours(n);
  int similar = 0;
  FOR(i, n)
    tours[i] = {i};

  for(auto e : edges){
    if(similar == similarityParameter)
      break;
    if(!isEnd[e.u] || !isEnd[e.v] || tours[e.u] == tours[e.v])
      continue;
    int sz0 = SZ(tours[e.u]), sz1 = SZ(tours[e.v]);
    if(tours[e.u][0] == e.u)
      reverse(all(tours[e.u]));
    if(tours[e.v].back() == e.v)
      reverse(all(tours[e.v]));
    
    int bg = tours[e.u][0], end = tours[e.v].back();
    tours[bg]  = tours[e.u];
    tours[end]  = tours[e.v];
    isEnd[e.u] = isEnd[e.v] = false;

    tours[bg].insert(tours[bg].end(), all(tours[end]));
    tours[end] = tours[bg];
    if(SZ(tours[end]) != sz0 + sz1)
      throw exception();
    isEnd[bg] = isEnd[end] = true;
    similar++;
  }

  tour0 = completeTour(cost0, isEnd, tours, tour0);
  tour1 = completeTour(cost1, isEnd, tours, tour1);
  assert(SZ(similarEdges(tour0, tour1)) >= similarityParameter);
}
