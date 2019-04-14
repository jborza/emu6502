// emu6502.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "state.h"
#include "cpu.h"
#include "disassembler.h"
#include "opcodes.h"
#include "test6502.h"

byte* read_game() {
	FILE* file = fopen("..\\bins\\tetris.bin", "r");
	byte* buffer = malloc(0x800);
	fread(buffer, 1, 0x800, file);
	fclose(file);
	return buffer;
}

int main()
{
	run_tests();
	printf("All tests succeeded.\n");
	return 0;
}

//void emulate() {
//	State6502 state;
//	memset(&state.memory, 0, sizeof(State6502));
//	print_state(&state);
//	clear_state(&state);
//	byte* memory = read_game();
//	//state.memory = read_game();
//	state.memory = malloc(4096);
//	memset(state.memory, 0, sizeof(byte) * 4096);
//	state.memory[0] = 0xEA;
//	state.memory[1] = 0x05; //ORA $a0
//	state.memory[2] = 0xA0;
//	state.memory[3] = 0xEA; //NOP
//	state.memory[4] = 0x09; //ORA #$ff
//	state.memory[5] = 0xff;
//	state.memory[6] = 0xA9; //LDA
//	state.memory[7] = 0x00; //0
//	state.memory[8] = 0x0D; //ORA $1234
//	state.memory[9] = 0x34;
//	state.memory[10] = 0x02;
//	state.memory[0xA0] = 0x13;
//	state.memory[0x0234] = 0xAA;
//
//	for (int i = 0; i < 10; i++) {
//		print_all(&state);
//		disassemble_6502(state.memory, state.pc);
//		emulate_6502_op(&state);
//	}
//}