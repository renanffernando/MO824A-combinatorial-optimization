#include "util.hpp"
#include "concordeSolver.hpp"
#include "lagrangian.hpp"
#include "solve_z.hpp"
#include "heuristic.hpp"

using namespace std;

struct InputData {
    int numberOfVertices, numberOfEdges, similarityParameter;
    vvd distance0;
    vvd distance1;
};

struct ProblemData {
    vd lambda0;
    vd lambda1;
    vvd cost0;
    vvd cost1;
    vi tour0;
    vi feasibleTour0;
    double lb0;
    vi tour1;
    vi feasibleTour1;
    double lb1;
    vi z;
    double zCost;
    double lowerBound;
    double upperBound;
    double previousLowerBound;
    double previousUpperBound;
    vi bestTour0;
    vi bestTour1;
    ld learnRate;
};

InputData readInputData();
void initializeProblemData (const InputData & input, ProblemData & problem);
bool criteria (const ProblemData problem);
void computeLowerBound (ProblemData & problem);
void computeUpperBound (const InputData & input, ProblemData & problem);
bool should_I_continue (const ProblemData problem);
void displayResult (const ProblemData & problem);

int main ()
{
    const auto input = readInputData();
    ProblemData problem;
    initializeProblemData(input, problem);
    do
    {
        problem.previousLowerBound = problem.lowerBound;
        problem.previousUpperBound = problem.upperBound;

        problem.cost0 = applyLambdaToCost(problem.lambda0, input.distance0);
        const auto solution_tsp_0 = solve_tsp(problem.cost0);
        problem.lb0 = get<double>(solution_tsp_0);
        problem.tour0 = get<vi>(solution_tsp_0);

        problem.cost1 = applyLambdaToCost(problem.lambda1, input.distance1);
        const auto solution_tsp_1 = solve_tsp(problem.cost1);
        problem.lb1 = get<double>(solution_tsp_1);
        problem.tour1 = get<vi>(solution_tsp_1);

        problem.feasibleTour0 = problem.tour0;
        problem.feasibleTour1 = problem.tour1;
        Heuristic::makeFeasibleSolution(problem.feasibleTour0, problem.feasibleTour1, problem.cost0, problem.cost1, input.similarityParameter);

        problem.z = solve_z(problem.lambda0, problem.lambda1, input.similarityParameter);
        problem.zCost = computeCost(problem.lambda0, problem.lambda1, problem.z);

        computeLowerBound(problem);
        computeUpperBound(input, problem);

        updateLambda(problem.tour0, problem.tour1, problem.z, problem.lambda0, problem.lambda1, problem.lowerBound, problem.upperBound, problem.learnRate);
        displayResult(problem);
    } while (should_I_continue(problem));
    return 0;
}

InputData readInputData ()
{
    InputData data;
    cin >> data.numberOfVertices >> data.numberOfEdges >> data.similarityParameter;
    assert(data.numberOfEdges == data.numberOfVertices * (data.numberOfVertices - 1) / 2);

    data.distance0 = vvd(
        data.numberOfVertices,
        vd(data.numberOfVertices, 0)
    );
    data.distance1 = vvd(
        data.numberOfVertices,
        vd(data.numberOfVertices, 0)
    );

    int lhsVertex, rhsVertex, distance0, distance1;
    for (int count = 0; count < data.numberOfEdges; ++count)
    {
        cin >> lhsVertex >> rhsVertex >> distance0 >> distance1;
        assert(lhsVertex < rhsVertex);
        data.distance0[lhsVertex][rhsVertex] = distance0;
        data.distance1[lhsVertex][rhsVertex] = distance1;
        data.distance0[rhsVertex][lhsVertex] = distance0;
        data.distance1[rhsVertex][lhsVertex] = distance1;
    }

    return data;
}

void initializeProblemData (const InputData & input, ProblemData & problem)
{
    problem.lambda0 = vd(input.numberOfVertices * input.numberOfVertices, 1);
    problem.lambda1 = vd(input.numberOfVertices * input.numberOfVertices, 1);
    problem.cost0 = input.distance0;
    problem.cost1 = input.distance1;
    problem.lowerBound = 0;
    problem.upperBound = DBL_MAX;
    problem.previousLowerBound = 0;
    problem.previousUpperBound = DBL_MAX;
    problem.learnRate = (sqrt(5) - 1) / 2;
}

bool should_I_continue (const ProblemData problem)
{   
    if(problem.learnRate < 1e-4) return false;
    constexpr double tolerance = 1e-2;
    double upperBoundDifference = abs(problem.upperBound - problem.previousUpperBound);
    double lowerBoundDifference = abs(problem.lowerBound - problem.previousLowerBound);
    if (upperBoundDifference > tolerance) return true;
    if (lowerBoundDifference > tolerance) return true;
    return false;
}

void computeLowerBound (ProblemData & problem)
{
    double acc = 0;
    acc += problem.lb0;
    acc += problem.lb1;
    acc += problem.zCost;
    if(problem.lowerBound > acc)
        problem.learnRate /= 2;
    problem.lowerBound = acc;
}

void computeUpperBound (const InputData & input, ProblemData & problem)
{
    double acc = 0;
    const auto & d0 = input.distance0;
    const auto & d1 = input.distance1;
    const auto & tour0 = problem.feasibleTour0;
    const auto & tour1 = problem.feasibleTour1;
    const auto & n = input.numberOfVertices;
    FOR(i, n)
    {
      acc += d0[tour0[i]][tour0[(i + 1) % n]];
      acc += d1[tour1[i]][tour1[(i + 1) % n]];
    }
    if(acc <= problem.upperBound){
        problem.upperBound = acc;
        problem.bestTour0 = problem.feasibleTour0;
        problem.bestTour1 = problem.feasibleTour1;
    }
}

void displayResult (const ProblemData & problem)
{
    cout << endl << endl;
    cout << "=================================" << endl;
    cout << "upper bound: " << problem.upperBound << endl;
    cout << "lower bound: " << problem.lowerBound << endl;
    cout << "tour 0: ";
    for(const auto & vertex: problem.feasibleTour0)
    {
        cout << vertex << " ";
    }
    cout << endl;
    cout << "tour 1: ";
    for(const auto & vertex: problem.feasibleTour1)
    {
        cout << vertex << " ";
    }
    cout << endl;
    cout << "=================================" << endl;
    cout << endl;
}
