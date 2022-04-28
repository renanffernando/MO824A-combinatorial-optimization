#include "concordeSolver.hpp"

#include <iostream>
#include <vector>
#include <tuple>
#include <cassert>
#include <stdexcept>

using std::cin;

std::vector<std::vector<int>> read_data ()
{
    int numberOfVertices, numberOfEdges, similarityParameter;
    cin >> numberOfVertices >> numberOfEdges >> similarityParameter;
    assert(numberOfEdges == numberOfVertices * (numberOfVertices - 1) / 2);

    if (similarityParameter != 0)
    {
        throw std::runtime_error("the similarity paramenter 'k' must be 0 (zero)");
    }

    std::vector<std::vector<int>> distance(
        numberOfVertices,
        std::vector<int>(numberOfVertices - 1, 0)
    );

    int lhsVertex, rhsVertex, distance0, distance1;
    for (int count = 0; count < numberOfEdges; ++count)
    {
        cin >> lhsVertex >> rhsVertex >> distance0 >> distance1;
        assert(lhsVertex < rhsVertex);
        distance[lhsVertex][rhsVertex] = distance0;
    }

    return distance;
}

void print_solution(const std::vector<int>& tour)
{
    std::cout << "\nSolution: ";
    for (int i = 0; i <= static_cast<int>(tour.size()); ++i)
    {
        std::cout << " " << tour[i % tour.size()];
    }
    std::cout << "\n\n" << std::endl;
}

int main()
{
    std::vector<std::vector<int>> distance = read_data();
    const auto solution = solve_tsp(distance);
    print_solution(std::get<1>(solution));
    return 0;
}
