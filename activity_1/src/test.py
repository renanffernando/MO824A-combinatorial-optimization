from CompanyProblemSolver import CompanyProblemSolver
from ProblemSizeCounter import ProblemSizeCounter

if __name__ == "__main__":
    J = 100
    solver = CompanyProblemSolver(J)
    print(solver)

    counter = ProblemSizeCounter(
        solver.problem.J,
        solver.problem.F,
        solver.problem.L,
        solver.problem.M,
        solver.problem.P
    )
    print(counter)
