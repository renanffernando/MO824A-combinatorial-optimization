#include "solve_z.hpp"

#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>

std::vector<int> solve_z(
    const std::vector<double> & cost,
    const int sigma
) {
    std::vector<std::pair<double,std::size_t>> value_index_vector;
    value_index_vector.reserve(cost.size());
    for (std::size_t i = 0; i < cost.size() ; ++i)
    {
        value_index_vector.emplace_back(std::make_pair(cost[i], i));
    }
    std::sort(value_index_vector.begin(), value_index_vector.end());
    std::vector<int> z(cost.size(), 0);
    for (std::size_t i = 0; i < static_cast<std::size_t>(sigma) ; ++i)
    {
        const std::size_t z_to_set = std::get<1>(value_index_vector[i]);
        z[z_to_set] = 1;
    }
    return z;
}
