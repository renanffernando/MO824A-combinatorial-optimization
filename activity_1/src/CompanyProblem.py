from Constants import Constants
from Random import randint, randArray

class CompanyProblem:

    def __init__ (self, J):
        """
        Generate a random instance of the problem based on an input number of clients J.

        Parameters
        ----------
        J : int (positive)
            Number of clients

        Returns
        -------
        int
            Description of anonymous integer return value.
        """
        J = int(J)
        self._checkInput(J)
        self._setSizes(J)
        self._setParameters()

    def _checkInput(self, J):
        if (J <= 0):
            raise Exception(f"Input value J must be positive. Value provided: '{J}'")

    def _setSizes(self, J):
        self.J = J
        self.F = randint(J, 2*J)
        self.L = randint(Constants.L.low, Constants.L.high)
        self.M = randint(Constants.M.low, Constants.M.high)
        self.P = randint(Constants.P.low, Constants.P.high)

    def _setParameters(self):
        self.D = randArray(self.J, self.P,         low=Constants.D.low, high=Constants.D.high)
        self.r = randArray(self.M, self.P, self.L, low=Constants.r.low, high=Constants.r.high)
        self.R = randArray(self.M, self.F,         low=Constants.R.low, high=Constants.R.high)
        self.C = randArray(self.L, self.F,         low=Constants.C.low, high=Constants.C.high)
        self.p = randArray(self.P, self.L, self.F, low=Constants.p.low, high=Constants.p.high)
        self.t = randArray(self.P, self.F, self.J, low=Constants.t.low, high=Constants.t.high)
