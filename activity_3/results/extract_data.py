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

plt.figure(figsize=(20,20))
for file_name in all_files:
    with open(file_name) as file:
        lines = list(map(lambda x: str(x), file.readlines()))
        upper_bounds = find_all(lines, '^upper bound: (\d+)$')
        lower_bounds = find_all(lines, '^lower bound: (\d+.?\d*)$')
        learning_rates = find_all(lines, '^learning rate: (\d.?\d*)$')
        processing_times = find_all(lines, '^Processing time: (\d+) \[s\]$')

        # print('\n')
        # print(file_name)
        # print(f'lower bound: {lower_bounds[-1]}')
        # print(f'upper bound: {upper_bounds[-1]}')
        # print(f'learning rate: {learning_rates[-1]}')
        # print(f'processing time: {processing_times[-1]}')
        ub = float(upper_bounds[-1])
        lb = float(lower_bounds[-1])
        gap = (ub - lb) / lb
        print(f'{file_name[3:6]} & {file_name[7:-4]} & {upper_bounds[-1]} & {lower_bounds[-1]} & {{:1.3f}} & {processing_times[-1]}\\\\\\hline'.format(gap))

        lower_bounds = list(map(float, lower_bounds))
        upper_bounds = list(map(float, upper_bounds))
        xdata = [_ for _ in range(len(upper_bounds))]
        n2r = {
            100: 0,
            150: 1,
            200: 2,
            250: 3
        }
        n = int(file_name[3:6])
        k = int(file_name[7:-4])
        r = n2r[n]
        c = (int(2 * k / n) if k != 0 else 0)
        pos = 3*r + c
        # print(f"\n\n{n}\n{k}\n{r}\n{c}\n{pos}\n\n")
        plt.subplot(4, 3, pos+1)
        plt.plot(xdata, lower_bounds, color='tab:red', label = "Lower Bound")
        plt.plot(xdata, upper_bounds, color='tab:blue', label = "Upper Bound")
        plt.xlabel('Iterations')
        plt.ylabel('Value')
        plt.grid(color='black', linestyle='-.', linewidth=0.5)
        plt.legend()
        name = file_name.replace(".log", "").replace("./", "")
        plt.title(f"vertices = {n} - similaridade = {k}")
plt.savefig('all.png')
plt.close()

'''
print(upper_bounds)
print(lower_bounds)
print(learning_rates)
print(processing_times)
'''
