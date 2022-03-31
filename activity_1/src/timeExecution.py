import timeit

def getExecutionTime(J):
    repetitions = 3
    totalExecutionTime = timeit.timeit(
        stmt = f"CompanyProblemSolver({J})",
        setup = "from CompanyProblemSolver import CompanyProblemSolver",
        number = repetitions
    )
    return totalExecutionTime / repetitions

problemSizes = range(100, 1000+1, 100)

executionTimes = {}
for J in problemSizes:
    executionTimes[J] = getExecutionTime(J)

print("\n\n")
print("Execution Times (J x time [s])")
print(executionTimes)
print("\n\n")

print("\n\n")
print("Execution times as a reproducible dict")
print(executionTimes.__repr__())
print("\n\n")
