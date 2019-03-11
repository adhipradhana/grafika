import sys

if (len(sys.argv) != 3):
    print('Usage: python draw.py <filename> <type>')
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

if (sys.argv[2] == "1"):
  for line in file:
    x1, y1, x2, y2, color = line.split(',')
    for i in range(width):
      x1_new = 1000 - (round(float(x1) + i))
      y1_new = 1000 - (round(float(y1) + i))
      x2_new = 1000 - (round(float(x2) + i))
      y2_new = 1000 - (round(float(y2) + i))

      new_line = str(x1_new) + ',' + str(y1_new) + ',' + str(x2_new) + ',' + str(y2_new) + ',' + color.split("\n")[0] + '\n'
      new_file.writelines(new_line)
elif (sys.argv[2] == "2"):
  for circle in file:
    x1, y1, r, color = circle.split(',')
    for i in range(width):
      x1_new = 1000 - (round(float(x1) + i))
      y1_new = 1000 - (round(float(y1) + i))

      new_line = str(x1_new) + ',' + str(y1_new) + ',' + str(r) + ',' + color.split("\n")[0] + '\n'
      new_file.writelines(new_line)
elif (sys.argv[2] == "3"):
  i = 0
  for line in file:
    if (',' in line):
      x1, y1, color = line.split(',')
      x1_new = 1000 - (round(float(x1) + i))
      y1_new = 1000 - (round(float(y1) + i))

      new_line = str(x1_new) + ',' + str(y1_new) + ',' + color.split("\n")[0] + '\n'
      new_file.writelines(new_line)
      i+=1
    else:
      new_file.writelines(line)
      i=0
file.close()
new_file.close()
