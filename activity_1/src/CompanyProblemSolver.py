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
        self._addDemandConstraint()
        self._addMachineCapacityConstraint()
        self._addVariablesCompatibilityConstraint()

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

    def _addMachineCapacityConstraint(self):
        # C in Matrix(L, F)
        # x in Matrix(P, L, F)
        # sum(x, axis=0) in Matrix(L, F)
        self.model.addConstr(self.problem.C >= np.sum(self.x, axis=0))

    def _addVariablesCompatibilityConstraint(self):
        # x in Matrix(P, L, F)
        # sum(x, axis=1) in Matrix(P, F)
        # y in Matrix(P, F, J)
        # sum(y, axis=2) in Matrix(P, F)
        self.model.addConstr(np.sum(self.x, axis=1) == np.sum(self.y, axis=2))

