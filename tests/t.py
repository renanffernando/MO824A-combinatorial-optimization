from math import ceil
import sys
import os

nT = 250

ns = [100, 150, 200, 250]
kMult = [0, 0.5, 1]

outdir = sys.argv[1]
obj = []
bounds = []
gaps = []
times = []
	
for n in ns:
	for kM in kMult:
		k = int(kM * n)
		name = "N%dK%d.ktsp" % (n, k)
		with open(outdir + "/" + name, "r") as instFile:
			while True:
				line = instFile.readline().split()
				if len(line) < 2:
					continue
				if line[0] == "Explored":
					times.append(float(line[7]))
				if (line[0] == "Optimal"  and line[1] == "solution") or (line[0] == "Time" and line[1] == "limit"):
					line = instFile.readline().replace(",", "")
					line = line.replace("%", "")
					line = line.split()
					assert(line[0] == "Best" and line[1] == "objective")
					assert(line[3] == "best" and line[4] == "bound")
					assert(line[6] == "gap")
					bestObj = float(line[2])
					bestBound = float(line[5])
					gap = float(line[7])
					obj.append(bestObj)
					bounds.append(bestBound)
					gaps.append(gap)
					break
print("Cost = ", obj)
print("BestBound = ", bounds)
print("Time = ", times)
print("Gap = ", gaps)