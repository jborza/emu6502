import csv

with open('opcodes.h','w') as outfile:
    with open('6502_ops.csv','r') as file:
        reader = csv.DictReader(file)
        for op in reader:
            print(op)
            addr_mode = op['addressing mode']
            suffix = '_'+ addr_mode if addr_mode != 'IMP' else ''
            name = op['mnemonic'] + suffix

            outfile.write('#define %s %s' % (name, op['opcode']))
            outfile.write('\n')
