import itertools
import numpy as np
import gurobipy as gp
from gurobipy import GRB
from CompanyProblem import CompanyProblem

class CompanyProblemSolver:

    def __init__ (self, J):
        self._initProblem(J)
        self._initModel()
        self._initXVariables()
        self._initYVariables()

    def _initProblem(self, J):
        self.problem = CompanyProblem(J)

    def _initModel(self):
        self.model = gp.Model("Lhamas Lamejantes Lunares")

    def _initXVariables(self):
        allVariablesIndices = itertools.product(
            range(self.problem.P),
            range(self.problem.L),
            range(self.problem.F)
        )
        indicesNames = ["p", "l", "f"]
        allVariables = map(
            lambda variableIndices: self._makeVariable("x", indicesNames, variableIndices),
            allVariablesIndices
        )
        self.x = np.array(allVariables).reshape(
            self.problem.P,
            self.problem.L,
            self.problem.F
        )

    def _initYVariables(self):
        allVariablesIndices = itertools.product(
            range(self.problem.P),
            range(self.problem.F),
            range(self.problem.J)
        )
        indicesNames = ["p", "f", "j"]
        allVariables = map(
            lambda variableIndices: self._makeVariable("y", indicesNames, variableIndices),
            allVariablesIndices
        )
        self.x = np.array(allVariables).reshape(
            self.problem.P,
            self.problem.F,
            self.problem.J
        )

    def _makeVariable(self, variableBaseName, indicesNames, indices):
        assert len(indicesNames) == len(indices)
        indicesIdentifier = "_".join(
            f"{iName}{i}" for (iName, i) in zip(indicesNames, indices)
        )
        variableName = f"{variableBaseName}_{indicesIdentifier}"
        return self.model.addVar(
            lb = 0.0,
            ub = float('inf'),
            vtype = GRB.CONTINUOUS,
            obj = 0.0,
            name = variableName
        )

