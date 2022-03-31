import functools
import operator
import numpy as np
import gurobipy as gp
from gurobipy import GRB
from CompanyProblem import CompanyProblem

def _numberOfElements(array):
    return functools.reduce(operator.mul, array.shape, 1)

class CompanyProblemSolver:

    def __init__ (self, J):
        self._initProblem(J)
        self._initModel()
        self._initXVariables()
        self._initYVariables()
        self._addDemandConstraint()
        self._addMachineCapacityConstraint()
        self._addVariablesCompatibilityConstraint()
        self._addResourcesConstraint()
        self._addGoal()
        self._count()
        self._solve()

    def _initProblem(self, J):
        self.problem = CompanyProblem(J)

    def _initModel(self):
        self.model = gp.Model("Lhamas Lamejantes Lunares")
        self.constraints = []

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
        self.demandConstraint = self.model.addConstr(self.problem.D.transpose()== np.sum(self.y, axis=1))
        self.constraints.append(self.demandConstraint)

    def _addMachineCapacityConstraint(self):
        # C in Matrix(L, F)
        # x in Matrix(P, L, F)
        # sum(x, axis=0) in Matrix(L, F)
        self.machineCapacityConstraint = self.model.addConstr(self.problem.C >= np.sum(self.x, axis=0))
        self.constraints.append(self.machineCapacityConstraint)

    def _addVariablesCompatibilityConstraint(self):
        # x in Matrix(P, L, F)
        # sum(x, axis=1) in Matrix(P, F)
        # y in Matrix(P, F, J)
        # sum(y, axis=2) in Matrix(P, F)
        self.variablesCompatibilityConstraint = self.model.addConstr(np.sum(self.x, axis=1) == np.sum(self.y, axis=2))
        self.constraints.append(self.variablesCompatibilityConstraint)

    def _addResourcesConstraint(self):
        # r in Matrix(M, P, L)
        # x in Matrix(P, L, F)
        # R in Matrix(M, F)
        R = np.zeros(self.problem.M, self.problem.F)
        for f in range(len(self.problem.F)):
            for m in range(len(self.problem.M)):
                R[m, f] = np.sum(self.problem.r[m,:,:] * self.x[:,:,f])
        self.resourcesConstraint = self.model.addConstr(self.problem.R >= R)
        self.constraints.append(self.resourcesConstraint)

    def _addGoal(self):
        self.model.setObjective(
            np.sum(self.problem.p * self.x) + np.sum(self.problem.t * self.y),
            GRB.MINIMIZE
        )

    def _count(self):
        self.numberOfVariables = _numberOfElements(self.x) + _numberOfElements(self.y)
        self.numberOfConstraints = functools.reduce(
            operator.add,
            map(_numberOfElements, self.constraints),
            0
        )

    def _solve(self):
        self.model.optimize()
        self.cost = self.model.ObjVal

