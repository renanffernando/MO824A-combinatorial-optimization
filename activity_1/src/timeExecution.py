import timeit
from CompanyProblemSolver import CompanyProblemSolver

class TimedSolution:
    def __init__(self, J):
        problem = CompanyProblemSolver(J)
        self.J = J
        self.numberOfVariables = problem.numberOfVariables
        self.numberOfConstraints = problem.numberOfConstraints
        self.cost = problem.cost
        self.executionTime = TimedSolution.getExecutionTime(J)

    @staticmethod
    def getExecutionTime(J):
        repetitions = 1
        totalExecutionTime = timeit.timeit(
            stmt = f"CompanyProblemSolver({J})",
            setup = "from CompanyProblemSolver import CompanyProblemSolver",
            number = repetitions
        )
        return totalExecutionTime / repetitions

    def __str__(self):
        asDict = {
            "problem size (J)": self.J,
            "number of variables": self.numberOfVariables,
            "number of constraints": self.numberOfConstraints,
            "cost": self.cost,
            "execution time [s]": self.executionTime
        }
        return asDict.__repr__()


if __name__ == "__main__":
    problemSizes = range(100, 400+1, 100)

    solutionOutput = []
    for J in problemSizes:
        print("\n\n")
        print("==========================================================")
        print(f"========================== J = {J} =======================")
        print("==========================================================")

        solution = TimedSolution(J)

        print("\n\n")
        print("Execution Times - partial result")
        print(solution)
        print("\n\n")

    print("==========================================================")
    print(f"========================== Output =======================")
    print("==========================================================")

    print("\n\n")
    print("Execution Times")
    print(solution)
    print("\n\n")
