import sys

if (len(sys.argv) != 2):
    print('Usage: python enlarge_circle.py <filename>')
    exit()

filename = sys.argv[1]
width = 10

file = open(filename + '.txt', "r")
new_file = open(filename + '_large.txt', "w")

for line in file:
	x1, y1, r = map(int, line.split(','))
	for i in range(width):
		x1_new = x1 + i
		y1_new = y1 + i
		r_new = r + i

		new_line = str(x1_new) + ',' + str(y1_new) + ',' + str(r_new) + '\n'
		new_file.writelines(new_line)

file.close()
new_file.close()
