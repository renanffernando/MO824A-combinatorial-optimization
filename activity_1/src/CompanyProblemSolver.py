import functools
import operator
import numpy as np
import gurobipy as gp
from gurobipy import GRB
from CompanyProblem import CompanyProblem

def _numberOfElements(array):
    array = np.array(array)
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
        self.demandConstraint = [
            self.model.addConstr(self.problem.D[j,p] == self.y[p,:,j].sum())
            for j in range(self.problem.J)
            for p in range(self.problem.P)
        ]
        self.constraints.append(self.demandConstraint)

    def _addMachineCapacityConstraint(self):
        # C in Matrix(L, F)
        # x in Matrix(P, L, F)
        # sum(x, axis=0) in Matrix(L, F)
        self.machineCapacityConstraint = [
            self.model.addConstr(self.problem.C[l,f] >= self.x[:,l,f].sum())
            for l in range(self.problem.L)
            for f in range(self.problem.F)
        ]
        self.constraints.append(self.machineCapacityConstraint)

    def _addVariablesCompatibilityConstraint(self):
        # x in Matrix(P, L, F)
        # sum(x, axis=1) in Matrix(P, F)
        # y in Matrix(P, F, J)
        # sum(y, axis=2) in Matrix(P, F)
        self.variablesCompatibilityConstraint = [
            self.model.addConstr(self.x[p,:,f].sum() == self.y[p,f,:].sum())
            for p in range(self.problem.P)
            for f in range(self.problem.F)
        ]
        self.constraints.append(self.variablesCompatibilityConstraint)

    def _addResourcesConstraint(self):
        # r in Matrix(M, P, L)
        # x in Matrix(P, L, F)
        # R in Matrix(M, F)
        R = np.array([
            gp.LinExpr()
            for m in range(self.problem.M)
            for f in range(self.problem.F)]
            ).reshape(self.problem.M, self.problem.F)
        for f in range(self.problem.F):
            for m in range(self.problem.M):
                for p in range(self.problem.P):
                    for l in range(self.problem.L):
                        R[m, f].addTerms(self.problem.r[m,p,l], self.x[p,l,f])
        self.resourcesConstraint = [
            self.model.addConstr(R[m,f] <= self.problem.R[m,f])
            for m in range(self.problem.M)
            for f in range(self.problem.F)
        ]
        self.constraints.append(self.resourcesConstraint)

    def _addGoal(self):
        expr = gp.LinExpr()
        for f in range(self.problem.F):
            for p in range(self.problem.P):
                for l in range(self.problem.L):\
                    expr.addTerms(self.problem.p[p,l,f], self.x[p,l,f])
        for p in range(self.problem.P):
            for f in range(self.problem.F):
                for j in range(self.problem.J):\
                    expr.addTerms(self.problem.t[p,f,j], self.y[p,f,j])
        self.model.setObjective(expr, GRB.MINIMIZE)

    def _count(self):
        self.numberOfVariables = _numberOfElements(self.x.tolist()) + _numberOfElements(self.y.tolist())
        self.numberOfConstraints = functools.reduce(
            operator.add,
            map(_numberOfElements, self.constraints),
            0
        )

    def _solve(self):
        self.model.optimize()
        self.cost = self.model.ObjVal

    def __str__(self):
        return f"""
CompanyProblemSolver[
    J = {self.problem.J}
    F = {self.problem.F}
    L = {self.problem.L}
    M = {self.problem.M}
    P = {self.problem.P}
    numberOfVariables = {self.numberOfVariables}
    numberOfConstraints = {self.numberOfConstraints}
    cost = {self.cost}
]
"""

