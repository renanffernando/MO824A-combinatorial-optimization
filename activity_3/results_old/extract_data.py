import os
import re

def find_all(l, regex):
    out = []
    for i in l:
        found = re.search(regex, i)
        if (found):
            out.append(found)
    return list(map(lambda s: s.group(1), out))



all_files = os.listdir('./')
all_files = list(filter(lambda file_name: file_name.endswith(".log"), all_files))

for file_name in ['./N100K50.log']:
    with open(file_name) as file:
        lines = list(map(lambda x: str(x), file.readlines()))
        print(lines[:10])
        upper_bounds = find_all(lines, '^upper bound: (\d+)$')
        lower_bounds = find_all(lines, '^lower bound: (\d+.?\d*)$')
        learning_rates = find_all(lines, '^learning rate: (\d.?\d*)$')
        processing_times = find_all(lines, '^Processing time: (\d+) \[s\]$')

print(upper_bounds)
print(lower_bounds)
print(learning_rates)
print(processing_times)
