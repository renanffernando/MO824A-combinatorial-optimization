from copy import deepcopy
from datetime import datetime
import pandas as pd
import random
from numpy.random import choice
import sys
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

def reactive_alpha(n, A, W, maxW, alpha, a, a_uses, a_costs, prob_alpha, cost_alpha):
	sol = [0 for i in range(n)]
	candidates = [i for i in range(n)]
	it = 0

	# print(a, cost_alpha, a_costs, a_uses)
	if a != -1:
		a_costs[a] += cost_alpha
		a_uses[a] += 1

	# print("prob_alpha = ",  prob_alpha)

	# print("anterior", count_alpha)
	# print(a, cost_alpha)
	# print(a_costs)
	# print('cost', count_alpha)
	# print(count_alpha[1])
	# print(count_alpha[a])
	# alpha_used = random.choices(alpha, weights = prob_alpha, k=1)[0]
	alpha_used = choice(alpha, 1, p = prob_alpha)[0]
	a = alpha.index(alpha_used)

	while len(candidates) != 0:

		addCosts = [add_cost(A, W, maxW, sol, x) for x in candidates]
		validCosts = [cost for cost in addCosts if cost != -INF]

		# if len(candidates) % 5 == 0:
			# alpha_used = random.choices(alpha, weights = prob_a, k=1)[0]

		if len(validCosts) == 0:
			return sol, a_uses, a_costs, a

		cMin = min(validCosts)

		cMax = max(validCosts)

		lowerBound = cMax - alpha_used * (cMax - cMin)

		rcl = [candidates[idx] for idx in range(len(candidates)) if
				 addCosts[idx] != -INF and addCosts[idx] >= lowerBound]

		if len(rcl) == 0:
			return sol, a_uses, a_costs, a

		candidateToAdd = random.choice(rcl)

		sol[candidateToAdd] = 1
		candidates.remove(candidateToAdd)

		it += 1

	# print("count_alpha =", count_alpha)

	return sol, a_uses, a_costs, a


def flipOneMovement(A, W, maxW, sol, lsMethod):
	n = len(sol)
	bestSol = deepcopy(sol)
	bestCost = sol_cost(A, sol)
	hadImprovement = True

	while hadImprovement:
		hadImprovement = False
		bestNeighbor = None
		bestNeighborCost = bestCost
		weightBestSol = sol_weight(W, bestSol)
	
		for i in range(n):
			if not bestSol[i] and (weightBestSol + W[i] > maxW):
				continue
			curSol = deepcopy(bestSol)
			curSol[i] = (curSol[i] + 1) % 2
			curW = sol_weight(W, curSol)

			if curW > maxW:
				continue

			curCost = sol_cost(A, curSol)
			if curCost > bestNeighborCost:
				bestNeighbor = curSol
				bestNeighborCost = curCost
				if lsMethod == "first-improv":
					break

		if bestNeighbor != None:
			hadImprovement = True
			bestSol = bestNeighbor
			bestCost = bestNeighborCost

	return bestSol, bestCost

def swapMovement(A, W, maxW, sol, lsMethod):
	n = len(sol)
	bestSol = deepcopy(sol)
	bestCost = sol_cost(A, sol)
	hadImprovement = True

	while hadImprovement:
		hadImprovement = False
		bestNeighbor = None
		bestNeighborCost = bestCost
		weightBestSol = sol_weight(W, bestSol)
  
		for i in range(n):
			if not bestSol[i]:
				continue
			for j in range(n):
				if bestSol[j] or (weightBestSol - W[i] + W[j] > maxW):
					continue
				curSol = deepcopy(bestSol)
				curSol[i] = (curSol[i] + 1) % 2
				curSol[j] = (curSol[j] + 1) % 2
				curW = sol_weight(W, curSol)

				if curW > maxW:
					continue

				curCost = sol_cost(A, curSol)
				if curCost > bestNeighborCost:
					bestNeighbor = curSol
					bestNeighborCost = curCost
					if lsMethod == "first-improv":
						break

		if bestNeighbor != None:
			hadImprovement = True
			bestSol = bestNeighbor
			bestCost = bestNeighborCost

	return bestSol, bestCost


def local_search(A, W, maxW, sol, lsMethod):
	neighborhoods = [flipOneMovement, swapMovement]
	k = 0
	bestSol = deepcopy(sol)
	bestCost = sol_cost(A, bestSol)
	while k < len(neighborhoods):
		localOptimalSol, localOptimalCost = neighborhoods[k](A, W, maxW, bestSol, lsMethod)
		if localOptimalCost > bestCost:
			bestCost = localOptimalCost
			bestSol = localOptimalSol
			k = 1 if k == 0 else 0
		else:
			k += 1
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

#função que atualiza as probabilidades de cada alpha
def prob_update(a_uses, a_costs, bestCost):
	#let average_alpha[i] be the average value of all solutions found using α = α_i , for i = 1, . . . , m.
	average_alpha = [(a_costs[i]/a_uses[i]) for i in range(len(a_uses))]
	print("average_alpha=", average_alpha)

	print("a_uses =", a_uses)
	print("a_costs=", a_costs)
	print("bestCosts=", bestCost)

	#Q_i = average_alpha[i]/Z*, Z* sendo a melhor solução encontrada até o momento
	Qi = [average_alpha[i]/bestCost for i in range(len(a_uses))]
	print("Qi=", Qi)

	#p_i = Q_i / ∑Q_j, p_i = Q_i de cada α dividido pela soma de todos os Q_i
	prob_alpha = [Qi[a]/sum(Qi) for a in range(len(a_uses))]
	print("prob_alpha=", prob_alpha)

	return prob_alpha

def grasp(n, A, W, maxW, maxIt, alpha, lsMethod, maxTimeSecs):
	bestSolution = []
	cost = 0
	bestCost = 0
	it = 0
	startTime = datetime.now()

	k = 25 # intervalo de interações em que se atualiza as probabilidades de cada alpha
	prob_alpha = [1/len(alpha)] * len(alpha) #lista de probabilidades de cada alpha
	a_uses = [0] * len(alpha) #número de vezes que cada alpha foi usado
	a_costs = [0] * len(alpha) #custo cumulativo obtido por cada uso de alpha
	a = -1 #valor de alpha escolhido a cada interação (entre 0 a len(alpha))

	while (datetime.now() - startTime).total_seconds() < maxTimeSecs and it < maxIt:

		if (it % k) == (k-1):
			prob_alpha = prob_update(a_uses, a_costs, bestCost)

		curSol, a_uses, a_costs, a = reactive_alpha(n, A, W, maxW, alpha, a, a_uses, a_costs, prob_alpha, cost)
		curSol, cost = local_search(A, W, maxW, curSol, lsMethod)

		if cost > bestCost:
			bestCost = cost
			bestSolution = curSol

		it += 1

	return bestSolution, bestCost


if __name__ == "__main__":

	maxIt = 500
	alpha_react = [0.1,0.2,0.8, 0.9]

	instances = ["kqbf080"]
	# instances = ["kqbf020", "kqbf040", "kqbf060", "kqbf080", "kqbf100", "kqbf200", "kqbf400"]
	lsMethods = ["fist-improv", "best-improv"]

	solutions = {'instance': [], 'alpha': [], 'lsMethod': [], 'solCost': [], 'time': []}

	for instanceName in instances:
		n, A, W, maxW = read_instance(instanceName)
		for lsMethod in lsMethods:
			start = datetime.now()
			sol, cost = grasp(n, A, W, maxW, maxIt, alpha_react, lsMethod, 30 * 60)
			totalTime = (datetime.now() - start).total_seconds()

			solutions['instance'].append(instanceName)
			solutions['alpha'].append(alpha_react)
			solutions['lsMethod'].append(lsMethod)
			solutions['time'].append(str(totalTime).replace('.', ','))
			solutions['solCost'].append(str(cost).replace('.', ','))

			print("Max weight = ", maxW)
			print("Sol cost = ", sol_cost(A, sol))
			print("Sol weight = ", sol_weight(W, sol))
			print("Sol = ", sol)

	df_solutions = pd.DataFrame(solutions)
	df_solutions.to_csv('kqbf_reactive_solutions.csv', sep=";")
