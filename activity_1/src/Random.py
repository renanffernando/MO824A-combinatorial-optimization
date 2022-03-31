import numpy as np

class Random:
    @staticmethod
    def randint (low, high):
        """
        Genrerate an random int in the interval [low, high]
        """
        return np.random.randint(low, high+1)

    @staticmethod
    def randArray (*size, low, high):
        assert low < high
        return (high - low) * np.random.rand(*size) + low
