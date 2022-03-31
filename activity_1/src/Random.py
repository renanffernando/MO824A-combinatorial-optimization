import numpy as np

class Random:

    randomSeed = 17558175
    randomGenerator = np.random.RandomState(randomSeed)

    @staticmethod
    def randint (low, high):
        """
        Genrerate an random int in the interval [low, high]
        """
        return Random.randomGenerator.randint(low, high+1)

    @staticmethod
    def randArray (*size, low, high):
        assert low < high
        return (high - low) * Random.randomGenerator.rand(*size) + low
