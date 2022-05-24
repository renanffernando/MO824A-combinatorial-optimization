import os
from typing import OrderedDict

all_files = os.listdir('./out')
all_files = [file_name for file_name in all_files if file_name[0] == 'k']
all_files = sorted(all_files)

tables = [OrderedDict(), OrderedDict()]

for file_name in all_files:
  with open("./out/" + file_name) as file:
    lines = list(map(lambda x: str(x).split(), file.readlines()))
    #print(lines)
    name = file_name[:-4]
    tables[0][name] = []
    tables[1][name] = []
    for line in lines:
      idx = 0 if line[0][0] == 'F' else 1
      tables[idx][name].append(line[9])
      tables[idx][name].append(line[15][:-2])

with open("firstImprovement.csv", "w") as file:
  file.write("Instance,Custo,Time(s),Custo,Time(s),Custo,Time(s),Custo,Time(s),Custo,Time(s),Custo,Time(s)\n")
  for key in tables[0]:
    file.write(key + ",")
    for i in range(len(tables[0][key])):
      file.write(tables[0][key][i] + (',' if i + 1 < len(tables[0][key]) else "\n"))
  
with open("bestImprovement.csv", "w") as file:
  file.write("Instance,Custo,Time(s),Custo,Time(s),Custo,Time(s),Custo,Time(s),Custo,Time(s),Custo,Time(s)\n")
  for key in tables[1]:
    file.write(key + ",")
    for i in range(len(tables[1][key])):
      file.write(tables[1][key][i] + (',' if i + 1 < len(tables[1][key]) else "\n"))