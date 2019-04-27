import sys

output_name = sys.argv[1]
print(output_name)

f = open(output_name, 'wb')

def as_byte(value):
    return bytes([value])

def process_line(line):
    hexes = line[5:].strip()
    hex_values = [int(hex,16) for hex in hexes.split(' ')]
    for hex_value in hex_values:
        print(as_byte(hex_value))
        f.write(as_byte(hex_value))
        #print(chr(hex_value), end = '')

for line in sys.stdin:
    process_line(line)

f.close()
