import sys

if (len(sys.argv) != 3):
    print('Usage: python draw.py <filename>')
    print("""
      Type:
      1 Line
      2 Circle
      3 Polygon
    """)
    exit()

filename = sys.argv[1]
width = 25

file = open(filename + '.txt', "r")
new_file = open(filename + '_large.txt', "w")

for line in file:
	x1, y1, x2, y2, color = map(int, line.split(','))
	for i in range(width):
		x1_new = 1000 - (x1 + i)
		y1_new = 1000 - (y1 + i)
		x2_new = 1000 - (x2 + i)
		y2_new = 1000 - (y2 + i)

		new_line = str(x1_new) + ',' + str(y1_new) + ',' + str(x2_new) + ',' + str(y2_new) + ',' + color + '\n'
		new_file.writelines(new_line)

file.close()
new_file.close()