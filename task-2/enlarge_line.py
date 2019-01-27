import sys

if (len(sys.argv) != 2):
    print('Usage: python enlarge_line.py <filename>')
    exit()

filename = sys.argv[1]
width = 10

file = open(filename + '.txt', "r")
new_file = open(filename + '_large.txt', "w")

for line in file:
	x1, y1, x2, y2 = map(int, line.split(','))
	for i in range(width):
		x1_new = x1 + i
		y1_new = y1 + i
		x2_new = x2 + i
		y2_new = y2 + i

		new_line = str(x1_new) + ',' + str(y1_new) + ',' + str(x2_new) + ',' + str(y2_new) + '\n'
		new_file.writelines(new_line)

file.close()
new_file.close()
