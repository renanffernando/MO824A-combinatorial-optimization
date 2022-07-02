import sys
import os
from glob import glob
import pandas as pd
import numpy as np
import os
import sys; sys.path.insert(0, '..')
from perfprof import *
import matplotlib.pyplot as plt

# libs local configurations
plt.rcParams['figure.figsize'] = [10,10]
pd.set_option('display.max_rows', 1000)
pd.set_option('display.max_columns', 1000)

MHs = ["GA", "Grasp", "Tabu"]

outBaseDir = "perfprof-graphics"
if(not os.path.exists(outBaseDir)):
  os.system("mkdir " + outBaseDir)

data1 = {}
data2 = {}

for MH in MHs:
  logDir = MH + "/hitCost"
  print(logDir)
  data1[MH] = []
  data2[MH] = []

  for logFile in sorted(glob(os.path.join(logDir, "*")), key=os.path.getsize):
    with open(logFile, "r") as inputFile:
      while True:
        try:
          l = inputFile.readline().split()
          cost = float(l[-1])
          data1[MH].append(cost)
          l = inputFile.readline().split()
          cost = float(l[-1])
          data2[MH].append(cost)
          break
        except:
          raise Exception("fail to read")
          break

datas = []
for MH in MHs:
  datas.append(data1[MH])
  datas.append(data2[MH])

data = np.transpose(datas)
namesMH = ["GA1", "GA2", "Grasp2", "Grasp2", "Tabu1", "Tabu2"]

print(data)
perfprof(data, linespecs=['red', 'blue', 'green', 'black', 'purple', 'yellow'], legendnames=namesMH,  title = "Time to target", nameFile = outBaseDir + "/ttt", fontsize = 20, tickfontsize = 20, legendfontsize = 14, titlefontsize = 20, legendpos = 'center left')
