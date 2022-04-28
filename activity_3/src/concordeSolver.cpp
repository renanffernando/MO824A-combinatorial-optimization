#include "concordeSolver.hpp"

extern "C" {
  #include <concorde.h>
}

#include <iostream>
#include <vector>
#include <numeric>

int solve_tsp_concorde (const std::vector<std::vector<int>>& distance, std::vector<int>& tour)
{
  int rval = 0;
  const int ncount = distance.size();
  if (static_cast<int>(tour.size()) != ncount) {
    tour.resize(ncount);
  }
  if (ncount < 5) {
    std::cerr << "Less than 5 nodes.";
    return 1;
  }

  int seed = rand();
  CCrandstate rstate;
  CCutil_sprand(seed, &rstate);
  const int ecount = (ncount * (ncount - 1)) / 2;
  std::vector<int> elist(ecount * 2);
  std::vector<int> elen(ecount);
  int edge = 0;
  int edgeWeight = 0;
  for (int i = 0; i < ncount; i++) {
    for (int j = i + 1; j < ncount; j++) {
      elist[edge++] = i;
      elist[edge++] = j;
      elen[edgeWeight++] = distance[i][j];
    }
  }
  char *name = CCtsp_problabel(" ");
  CCdatagroup dat;
  CCutil_init_datagroup (&dat);
  rval = CCutil_graph2dat_matrix (ncount, ecount, elist.data(), elen.data(), 1, &dat);

  double  optval, *timebound = nullptr;
  int success, foundtour, hit_timebound = 0;
  rval = CCtsp_solve_dat (ncount, &dat, nullptr, tour.data(), NULL, &optval, &success,
    &foundtour, name, timebound, &hit_timebound, 1, &rstate);

  return rval;
}
