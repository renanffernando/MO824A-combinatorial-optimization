import sys
import os
from glob import glob


MHs = ["GA", "Grasp", "Tabu"]

outBaseDir = "tttplot-graphics"
if(not os.path.exists(outBaseDir)):
  os.system("mkdir " + outBaseDir)

for MH in MHs:
  logDir = MH + "/tttplot"
  print(logDir)
  outDir = outBaseDir + "/" + MH
  if(not os.path.exists(outDir)):
    os.system("mkdir " + outDir)
  print(outDir)

  for logFile in sorted(glob(os.path.join(logDir, "*")), key=os.path.getsize):
    datas = []
    with open(logFile, "r") as inputFile:
      while True:
        try:
          l = inputFile.readline().split()
          time = float(l[-1])
          if time >= 60:
            continue
          datas.append(time)
        except:
          break

    with open("dados.dat", "w") as outputFile:
      for d in datas:
        outputFile.write(str(d) + "\n")

    l = logFile.split(".")
    fileName = l[-2]
    l = fileName.split("/")
    fileName = l[-1]
    print(fileName)
    title = "Instance " + fileName
    os.system("perl tttplots.pl -f dados " + title)
    os.system("ps2pdf -dEPSCrop dados-exp.ps dados-exp.pdf")
    os.system("ps2pdf -dEPSCrop dados-qq.ps dados-qq.pdf")
    os.system("gs -sDEVICE=jpeg -dTextAlphaBits=4 -r300 -o dados-qq.jpg dados-qq.pdf")
    os.system("gs -sDEVICE=jpeg -dTextAlphaBits=4 -r300 -o dados-exp.jpg dados-exp.pdf")
    os.system("mv dados-exp.jpg " + outDir + "/" + fileName + "-exp.jpg")
    os.system("mv dados-qq.jpg " + outDir + "/" + fileName + "-qq.jpg")
    os.system("rm dados*")
