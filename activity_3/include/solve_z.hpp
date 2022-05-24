#ifndef ZOLVE_Z_HPP_
#define ZOLVE_Z_HPP_

#include <vector>

std::vector<int> solve_z(
    const std::vector<double> & cost0,
    const std::vector<double> & cost1,
    const int sigma
);

double computeCost (
    const std::vector<double> & cost0,
    const std::vector<double> & cost1,
    const std::vector<int> & z
);

#endif
