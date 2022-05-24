#include "util.hpp"
#include "concordeSolver.hpp"
#include "lagrangian.hpp"
#include "solve_z.hpp"
#include "heuristic.hpp"

using namespace std;

constexpr int maximumRunninTimeSeconds = 10*60;
constexpr double minimumLearningRate = 1e-4;
constexpr double minimumBoundsDifferenceTolerance = 1e-2;

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
    double bestLowerBound;
    double previousLowerBound;
    double previousUpperBound;
    vi bestTour0;
    vi bestTour1;
    ld learnRate;
    chrono::_V2::system_clock::time_point startTime;
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
    const auto elapsedTimeSeconds = chrono::duration_cast<chrono::seconds>(
        chrono::high_resolution_clock::now() - problem.startTime
    ).count();
    cout << endl << endl << "Processing time: " << elapsedTimeSeconds << " [s]" << endl << endl;
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
    problem.bestLowerBound = 0;
    problem.previousLowerBound = 0;
    problem.previousUpperBound = DBL_MAX;
    problem.learnRate = 2;
    problem.startTime = chrono::high_resolution_clock::now();
}

bool should_I_continue (const ProblemData problem)
{
    // time
    const auto elapsedTime = chrono::duration_cast<chrono::seconds>(
        chrono::high_resolution_clock::now() - problem.startTime
    ).count();
    if (elapsedTime > maximumRunninTimeSeconds)
    {
        cout << endl << endl << ">>>>> " << "maximum running time" << " criteria met: " << maximumRunninTimeSeconds << " [s]" << " <<<<<" << endl;
        return false;
    }
    // learn rate
    if(problem.learnRate < minimumLearningRate)
    {
        cout << endl << endl << ">>>>> " << "minimum learning rate" << " criteria met: " << minimumLearningRate << " <<<<<" << endl;
        return false;
    }
    // solution precision
    double upperBoundDifference = abs(problem.upperBound - problem.previousUpperBound);
    double lowerBoundDifference = abs(problem.lowerBound - problem.previousLowerBound);
    if ((upperBoundDifference < minimumBoundsDifferenceTolerance) &&
        (lowerBoundDifference < minimumBoundsDifferenceTolerance))
    {

        cout << endl << endl << ">>>>> " << "minimum upper bound and lower bound difference" << " criteria met: " << minimumBoundsDifferenceTolerance << " <<<<<" << endl;
        return false;
    }
    return true;
}

void computeLowerBound (ProblemData & problem)
{
    double lowerBoundFound = 0;
    lowerBoundFound += problem.lb0;
    lowerBoundFound += problem.lb1;
    lowerBoundFound += problem.zCost;
    if(problem.bestLowerBound < lowerBoundFound)
    {
        problem.bestLowerBound = lowerBoundFound;
    }
    if(problem.lowerBound > lowerBoundFound)
        problem.learnRate *= (sqrt(5) - 1) / 2;
    problem.lowerBound = lowerBoundFound;
}

void computeUpperBound (const InputData & input, ProblemData & problem)
{
    const auto & d0 = input.distance0;
    const auto & d1 = input.distance1;
    const auto & tour0 = problem.feasibleTour0;
    const auto & tour1 = problem.feasibleTour1;
    const auto & n = input.numberOfVertices;
    double upperBoundFound = 0;
    FOR(i, n)
    {
      upperBoundFound += d0[tour0[i]][tour0[(i + 1) % n]];
      upperBoundFound += d1[tour1[i]][tour1[(i + 1) % n]];
    }
    if(upperBoundFound < problem.upperBound){
        problem.upperBound = upperBoundFound;
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
    cout << "learning rate: " << problem.learnRate << endl;
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
