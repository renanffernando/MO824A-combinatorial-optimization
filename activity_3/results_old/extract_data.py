import os
import re
import matplotlib.pyplot as plt

def find_all(l, regex):
    out = []
    for i in l:
        found = re.search(regex, i)
        if (found):
            out.append(found)
    return list(map(lambda s: s.group(1), out))

all_files = os.listdir('./')
all_files = list(filter(lambda file_name: file_name.endswith(".log"), all_files))
all_files = ["./" + file for file in all_files]
print(all_files)

for file_name in all_files:
    with open(file_name) as file:
        lines = list(map(lambda x: str(x), file.readlines()))
        print(lines[:10])
        upper_bounds = find_all(lines, '^upper bound: (\d+)$')
        lower_bounds = find_all(lines, '^lower bound: (\d+.?\d*)$')
        learning_rates = find_all(lines, '^learning rate: (\d.?\d*)$')
        processing_times = find_all(lines, '^Processing time: (\d+) \[s\]$')

        lower_bounds = list(map(float, lower_bounds))
        upper_bounds = list(map(float, upper_bounds))
        xdata = [_ for _ in range(len(upper_bounds))]
        plt.plot(xdata, lower_bounds, color='tab:red', label = "Lower Bound")
        plt.plot(xdata, upper_bounds, color='tab:blue', label = "Upper Bound")
        plt.xlabel('Iterations')
        plt.ylabel('Value')
        plt.grid(color='black', linestyle='-.', linewidth=0.5)
        plt.legend()
        name = file_name.replace(".log", "").replace("./", "")
        plt.title("Instance " + name)
        plt.savefig(name + ".png")
        plt.close()

'''
print(upper_bounds)
print(lower_bounds)
print(learning_rates)
print(processing_times)
'''
