import struct
import sys

if (len(sys.argv) != 2):
    print('Command: py bitmap_to_point.py <bmp file name without extension>')
    exit()

filename = sys.argv[1]

with open(filename + '.bmp', 'rb') as f:
    data = bytearray(f.read())

text_file = open(filename + '.txt', 'w')

width, = struct.unpack_from('<L', data, 18)
height, = struct.unpack_from('<L', data, 22)
offset, = struct.unpack_from('<L', data, 10)

row_size = (width * 3 + 3) // 4 * 4

for y in range(height):
    for x in range(width):
        pos = offset + x * 3 + y * row_size
        if (data[pos + 0] != 255 or data[pos + 1] != 255 or data[pos + 2] != 255):
            text_file.write(str(x) + ',' + str(height - y - 1) + '\n')

text_file.close()