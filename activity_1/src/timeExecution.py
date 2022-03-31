import timeit
from CompanyProblemSolver import CompanyProblemSolver

class TimedSolution:
    def __init__(self, J):
        problem = CompanyProblemSolver(J)
        self.numberOfVariables = problem.numberOfVariables
        self.numberOfConstraints = problem.numberOfConstraints
        self.cost = problem.cost
        self.executionTime = TimedSolution.getExecutionTime(J)

    @staticmethod
    def getExecutionTime(J):
        repetitions = 3
        totalExecutionTime = timeit.timeit(
            stmt = f"CompanyProblemSolver({J})",
            setup = "from CompanyProblemSolver import CompanyProblemSolver",
            number = repetitions
        )
        return totalExecutionTime / repetitions

if __name__ == "__main__":
    problemSizes = range(100, 1000+1, 100)

    solutionOutput = []
    for J in problemSizes:
        solution = TimedSolution(J)
        solutionOutput.append({
            "problem size (J)"
            "number of variables": solution.numberOfVariables,
            "number of constraints": solution.numberOfConstraints,
            "cost": solution.cost,
            "execution time [s]": solution.executionTime
        })

    print("\n\n")
    print("Execution Times (J x time [s])")
    print(solutionOutput)
    print("\n\n")

    print("\n\n")
    print("Execution times as a reproducible dict")
    print(solutionOutput.__repr__())
    print("\n\n")
