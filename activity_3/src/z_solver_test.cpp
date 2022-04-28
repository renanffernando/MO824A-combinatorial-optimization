#include "solve_z.hpp"

#include <vector>
#include <iostream>

template <typename T>
void print_solution(const std::vector<T> & vec)
{
    std::cout << "\nSolution: ";
    for (const auto & val: vec)
    {
        std::cout << " " << val;
    }
    std::cout << "\n" << std::endl;
}

int main ()
{
    const std::vector<double> cost = {
        0.91,
        0.88,
        0.34,
        0.09,
        0.23,
        0.79,
        0.24,
        0.65,
        0.09,
        0.55,
    };
    const int sigma = 4;
    const auto z = solve_z(cost, sigma);
    print_solution(z);
}
