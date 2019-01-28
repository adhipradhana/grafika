import numpy as np

def to_string(arr):
    s = ""
    for i, elem in enumerate(arr):
        if i != 0:
            s += ","
        s += str(elem)
    s += "\n"
    return s

with open('lines.txt','r') as f:
    lines = f.read()

lines = [line.split(',') for line in lines.split('\n')]
lines = np.array(lines).astype(float)
lines = lines * 10
lines[:, 0] = 150 - lines[:, 0]
lines[:, 2] = 150 - lines[:, 2]

lines = lines.astype(int)

print(lines)

with open('lines_transformed.txt', 'w') as f:
    for line in lines:
        f.write(to_string(line))