import csv

with open('cpu.c','w') as outfile:

    with open('6502_ops.csv','r') as file:
        reader = csv.DictReader(file)
        for op in reader:
            print(op)
            args = int(op['bytes'])

            addr_mode = op['addressing mode']
            suffix = '_'+ addr_mode if addr_mode != 'IMP' else ''
            mnemonic = op['mnemonic']
            name = mnemonic + suffix

            outfile.write('case %s: ' % name)
            outfile.write('unimplemented_instruction(state); break;\n')