import csv

with open('disassembler.c','w') as outfile:

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
            outfile.write('sprintf(op, ')
            if(addr_mode == 'IMP'):
                outfile.write('"%s"' % mnemonic)
            else: #all other addressing modes use name + suffix
                outfile.write('"%s ' % mnemonic)    
                if(addr_mode == 'IMM'):
                    outfile.write('#$%02X')
                elif(addr_mode == 'ACC'):
                    outfile.write("A")
                elif(addr_mode.startswith('ZP')):
                    outfile.write("$%02X")
                    if(addr_mode == 'ZPX'):
                        outfile.write(",X")
                    if(addr_mode == 'ZPY'):
                        outfile.write(",Y")
                elif(addr_mode.startswith('ABS')):
                    outfile.write("$%02X%02X")
                    if(addr_mode == 'ABSX'):
                        outfile.write(",X")
                    if(addr_mode == 'ABSY'):
                        outfile.write(",Y")
                elif(addr_mode == 'IND'):
                    outfile.write("($%02X%02X)")
                elif(addr_mode == 'INDX'):
                    outfile.write("($%02X,X)")
                elif(addr_mode == 'INDY'):
                    outfile.write("($%02X),Y")
                elif(addr_mode == 'REL'):
                    outfile.write("$%02X")
                #end format string
                outfile.write("\"")
                
                #append bytes, reversed as of little endian
                if(args > 2):
                    outfile.write(", code[2]")
                if(args > 1):
                    outfile.write(", code[1]")
                
            outfile.write(');')
            if(args > 1):
                outfile.write("bytes = %s;" % args)
            #sprintf(op, "%s"' % ())
            #outfile.write('#define %s %s' % (name, op['opcode']))
            outfile.write('break;')
            outfile.write('\n')
