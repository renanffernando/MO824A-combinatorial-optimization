from copy import deepcopy
from datetime import datetime
import pandas as pd
import random
from itertools import combinations
from math import ceil

INF = 99999999999


def sol_cost(A, sol):
    n = len(sol)
    cost = 0

    for i in range(n):
        for j in range(n):
            cost += A[i, j] * sol[i] * sol[j]

    return cost


def sol_weight(W, sol):
    n = len(sol)
    weight = 0

    for i in range(n):
        weight += W[i] * sol[i]

    return weight


def add_cost(A, W, maxW, sol, xi):
    usedW = sol_weight(W, sol)
    if usedW + W[xi] > maxW:
        return -INF

    # print("qweqweqweqeqwe")
    cost = 0
    n = len(sol)

    for xj in range(n):
        cost += sol[xj] * A[xi, xj]

    return cost


def greedy_random_construction(n, A, W, maxW, alpha):
    sol = [0 for i in range(n)]
    candidates = [i for i in range(n)]
    it = 0

    while len(candidates) != 0:
        addCosts = [add_cost(A, W, maxW, sol, x) for x in candidates]
        validCosts = [cost for cost in addCosts if cost != -INF]

        if len(validCosts) == 0:
            return sol

        cMin = min(validCosts)
        cMax = max(validCosts)

        upperBound = cMin + alpha * (cMax - cMin)

        rcl = [candidates[idx] for idx in range(len(candidates)) if
               addCosts[idx] != -INF and addCosts[idx] >= upperBound]

        if len(rcl) == 0:
            return sol

        candidateToAdd = random.choice(rcl)

        sol[candidateToAdd] = 1
        candidates.remove(candidateToAdd)

        it += 1

    return sol


def local_search(A, W, maxW, sol, lsMethod):
    n = len(sol)
    bestSol = deepcopy(sol)
    bestCost = sol_cost(A, sol)
    hadImprovement = True

    while hadImprovement:
        hadImprovement = False
        for i in range(n):
            curSol = deepcopy(sol)
            curSol[i] = (curSol[i] + 1) % 2
            curW = sol_weight(W, curSol)

            if curW > maxW:
                continue

            curCost = sol_cost(A, curSol)
            if curCost > bestCost:
                hadImprovement = True
                bestSol = curSol
                bestCost = curCost
                if lsMethod == "first-improv":
                    break

    return bestSol, bestCost


def read_instance(file_name):
    sys.stdin = open("instances/" + file_name, "r")

    n = int(input())
    maxW = int(input())
    W = list(map(int, input().split()))
    A = {(i, j): 0 for i in range(n) for j in range(n)}

    for i in range(n):
        ai = list(map(int, input().split()))
        for j in range(len(ai)):
            A[i, j] = ai[j]

    return n, A, W, maxW


def grasp(n, A, W, maxW, maxIt, alpha, lsMethod, maxTimeSecs):
    bestSolution = []
    bestCost = 0
    it = 0
    startTime = datetime.now()

    while (datetime.now() - startTime).total_seconds() < maxTimeSecs and it < maxIt:
        curSol = greedy_random_construction(n, A, W, maxW, alpha)
        curSol, cost = local_search(A, W, maxW, curSol, lsMethod)

        if cost > bestCost:
            bestCost = cost
            bestSolution = curSol
        it += 1

    return bestSolution, bestCost


if __name__ == "__main__":

    maxIt = 500
    alphas = [0.4, 0.8]
    instances = ["kqbf020", "kqbf040", "kqbf060", "kqbf080"]
    lsMethods = ["fist-improv", "best-improv"]
    # instances = ["kqbf020", "kqbf040", "kqbf060", "kqbf080", "kqbf100", "kqbf200", "kqbf400"]

    solutions = {'instance': [], 'alpha': [], 'lsMethod': [], 'solCost': [], 'time': []}

    for instanceName in instances:
        n, A, W, maxW = read_instance(instanceName)
        for alpha in alphas:
            for lsMethod in lsMethods:
                start = datetime.now()
                sol, cost = grasp(n, A, W, maxW, maxIt, alpha, lsMethod, 30 * 60)
                totalTime = (datetime.now() - start).total_seconds()

                solutions['instance'].append(instanceName)
                solutions['alpha'].append(alpha)
                solutions['lsMethod'].append(lsMethod)
                solutions['time'].append(str(totalTime).replace('.', ','))
                solutions['solCost'].append(str(cost).replace('.', ','))

                # print("Max weight = ", maxW)
                # print("Sol cost = ", sol_cost(A, sol))
                # print("Sol weight = ", sol_weight(W, sol))
                # print("Sol = ", sol)

    df_solutions = pd.DataFrame(solutions)
    df_solutions.to_csv('kqbf_solutions.csv', sep=";")