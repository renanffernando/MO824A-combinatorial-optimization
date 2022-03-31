import inspect


class ProblemSizeCounter:

    def __init__ (self, J, F, L, M, P):
        self._initNumberOfVariables(J, F, L, M, P)
        self._initNumberOfConstraints(J, F, L, M, P)

    def _initNumberOfVariables(self, J, F, L, M, P):
        self.numberOfVariablesX = P * L * F
        self.numberOfVariablesY = P * F * J
        self.totalNumberOfVariables = self.numberOfVariablesX + self.numberOfVariablesY

    def _initNumberOfConstraints(self, J, F, L, M, P):
        self.numberOfDemandConstraints = J * P
        self.numberOfMachineCapacityConstraints = L * F
        self.numberOfVariablesCompatibilityConstraints = P * F
        self.numberOfResourcesConstraints = M * F
        self.totalNumberOfConstraints = self.numberOfDemandConstraints \
            + self.numberOfMachineCapacityConstraints \
            + self.numberOfVariablesCompatibilityConstraints \
            + self.numberOfResourcesConstraints


    def __str__ (self):
        attributesToPrint = [
            "numberOfVariablesX",
            "numberOfVariablesY",
            "totalNumberOfVariables",
            "numberOfDemandConstraints",
            "numberOfMachineCapacityConstraints",
            "numberOfVariablesCompatibilityConstraints",
            "numberOfResourcesConstraints",
            "totalNumberOfConstraints"
        ]
        string = "ProblemSizeCounter[\n"
        for attribute in attributesToPrint:
            value = getattr(self, attribute)
            string += f"\t{attribute} = {value}\n"
        string += "]"
        return string
