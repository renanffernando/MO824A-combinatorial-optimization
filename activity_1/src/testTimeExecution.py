from datetime import datetime
from CompanyProblemSolver import CompanyProblemSolver

class TimedSolution:
    def __init__(self, J):
        time_start = datetime.now()
        solver = CompanyProblemSolver(J, displayProgress=True)
        time_end = datetime.now()
        self.duration = float((time_end - time_start).total_seconds())
        self.J = J
        self.numberOfVariables = solver.numberOfVariables
        self.numberOfConstraints = solver.numberOfConstraints
        self.cost = solver.cost

    def __str__(self):
        asDict = {
            "problem size (J)": self.J,
            "number of variables": self.numberOfVariables,
            "number of constraints": self.numberOfConstraints,
            "cost": self.cost,
            "execution time [s]": self.duration
        }
        return asDict.__repr__()

if __name__ == "__main__":
    problemSizes = range(100, 200+1, 100)
    for J in problemSizes:
        solution = TimedSolution(J)
        print(solution)
