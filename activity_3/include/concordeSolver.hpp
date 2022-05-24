#ifndef CONCORDE_SOLVER_HPP_
#define CONCORDE_SOLVER_HPP_

#include <tuple>
#include <vector>
#include "util.hpp"

std::vector<int> solve_tsp (const std::vector<std::vector<int>>& distance);
std::tuple<double, std::vector<int>> solve_tsp (const std::vector<std::vector<double>>& distance);

#endif
