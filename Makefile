CC=gcc
emu6502:
	$(CC) -o emu6502 *.c 
test:
	$(CC) -o test test6502.c cpu.c disassembler.c memory.c test_framework.c test_main.c