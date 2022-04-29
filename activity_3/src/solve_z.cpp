#include "solve_z.hpp"

#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cmath>

std::vector<int> sumCosts(
    const std::vector<double> & cost0,
    const std::vector<double> & cost1
) {
    assert(cost0.size() == cost1.size());
    std::vector<int> sum (cost0.size());
    for (std::size_t i = 0; i < cost0.size(); ++i)
    {
        sum[i] = cost0[i] + cost1[i];
    }
    return sum;
}

double computeCost (
    const std::vector<double> & cost0,
    const std::vector<double> & cost1,
    const std::vector<int> & z
) {
    assert(cost0.size() == cost1.size());
    assert(cost0.size() == z.size());
    double acc = 0;
    for (std::size_t i = 0; i < z.size() ; ++i)
    {
        acc += (cost0[i] + cost1[i]) * z[i];
    }
    return acc / 2;
}

std::vector<int> solve_z(
    const std::vector<double> & cost0,
    const std::vector<double> & cost1,
    const int sigma
) {
    std::vector<std::pair<double,std::size_t>> value_index_vector;
    const auto cost = sumCosts(cost0, cost1);
    value_index_vector.reserve(cost.size());
    for (std::size_t i = 0; i < cost.size() ; ++i)
    {
        value_index_vector.emplace_back(std::make_pair(cost[i], i));
    }
    std::sort(value_index_vector.begin(), value_index_vector.end());
    std::vector<int> z(cost.size(), 0);
    int numberVertices = sqrt(cost.size());
    for (std::size_t i = 0; i < static_cast<std::size_t>(sigma) ; ++i)
    {
        const std::size_t z_to_set = std::get<1>(value_index_vector[i]);
        int u = z_to_set % numberVertices, v = z_to_set / numberVertices;
        z[v * numberVertices + u] = z[u * numberVertices + v] = 1;
    }
    return z;
}
