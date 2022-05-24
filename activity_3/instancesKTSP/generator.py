from math import ceil
import sys
import os

nT = 250

ns = [100, 150, 200, 250]
kMult = [0, 0.5, 1]
x1 = []
y1 = []
x2 = []
y2 = []

for _ in range(nT):
  xi1, yi1, xi2, yi2 = [int(k) for k in input().split()]
  x1.append(xi1);
  y1.append(yi1);
  x2.append(xi2);
  y2.append(yi2);

def calCost(xa, ya, xb, yb):
  return int(ceil(((xa - xb)**2 + (ya - yb) ** 2) **(0.5)))


outdir = "."

for n in ns:
  m = n * (n - 1) // 2

  for kM in kMult:
    k = int(kM * n)
    name = "N%dK%d.ktsp" % (n, k)
    with open(outdir + "/" + name, "w") as instFile:
      instFile.write("%d %d %d\n" % (n, m, k))
      for i in range(n):
        for j in range(i + 1, n):
          instFile.write("%d %d %d %d\n" % (i, j, calCost(x1[i], y1[i], x1[j], y1[j]), calCost(x2[i], y2[i], x2[j], y2[j])))
