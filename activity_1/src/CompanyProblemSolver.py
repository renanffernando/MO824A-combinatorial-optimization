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
        self.x = self.model.addMVar(
            shape = (self.problem.P, self.problem.L, self.problem.F),
            lb = 0,
            ub = float('inf'),
            vtype = GRB.CONTINUOUS,
            obj = 0.0,
            name = "x"
        )

    def _initYVariables(self):
        self.y = self.model.addMVar(
            shape = (self.problem.P, self.problem.F, self.problem.J),
            lb = 0,
            ub = float('inf'),
            vtype = GRB.CONTINUOUS,
            obj = 0.0,
            name = "y"
        )

    def _addDemandConstraint(self):
        # D in Matrix(J, P)
        # D.transpose in Matrix(P, J)
        # y in Matrix(P, F, J)
        # np.sum(self.y, axis=1) in Matrix(P, J)
        self.model.addConstr(self.problem.D.transpose()== np.sum(self.y, axis=1))

