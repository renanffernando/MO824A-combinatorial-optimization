#include "concordeSolver.hpp"

extern "C" {
  #include <concorde.h>
}

#include <iostream>
#include <vector>
#include <numeric>
#include <tuple>
#include <stdexcept>

std::vector<int> solve_tsp (const std::vector<std::vector<int>>& distance)
{
    const int ncount = distance.size();
    if (ncount < 5) {
        throw std::runtime_error("Less than 5 nodes.");
    }
    std::vector<int> tour(distance.size());
    int rval;

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
    rval = CCutil_graph2dat_matrix(
        ncount,
        ecount,
        elist.data(),
        elen.data(),
        1,
        &dat
    );

    double  optval, *timebound = nullptr;
    int success, foundtour, hit_timebound = 0;
    rval = CCtsp_solve_dat (
        ncount,
        &dat,
        nullptr,
        tour.data(),
        nullptr,
        &optval,
        &success,
        &foundtour,
        name,
        timebound,
        &hit_timebound,
        1,
        &rstate
    );

    if (rval != 0)
    {
        throw std::runtime_error("error solving TSP with concorde");
    }

    return tour;
}

std::tuple<double, std::vector<int>> solve_tsp (const std::vector<std::vector<double>>& distance)
{
    constexpr int multiplier = 1e+4;
    std::vector<std::vector<int>> integerDistances(
        distance.size(),
        std::vector<int>(distance[0].size(), 0)
    );
    for (std::size_t i = 0; i < distance.size(); ++i)
    {
        for (std::size_t j = 0; j < distance.size(); ++j)
        {
            integerDistances[i][j] = multiplier * distance[i][j];
        }
    }
    std::vector<int> tour = solve_tsp(integerDistances);
    ld cost = 0;
    FOR(i, SZ(tour))
        cost += distance[tour[i]][tour[(i + 1) % SZ(tour)]];
    return tuple<double, vi>(cost, tour);
}
