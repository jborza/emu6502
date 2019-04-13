// emu6502.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include "state.h"
#include "cpu.h"
#include "disassembler.h"

byte* read_game() {
	FILE* file = fopen("..\\bins\\tetris.bin", "r");
	byte* buffer = malloc(0x800);
	fread(buffer, 1, 0x800, file);
	fclose(file);
	return buffer;
}

void print_all(State6502* state) {
	print_state(state);
	print_memory(state, 0);
	print_memory(state, 0x20);
	print_memory(state, 0x40);
	print_memory(state, 0x80);
}

int main()
{
	printf("Hello World!\n");
	State6502 state;
	print_state(&state);
	clear_state(&state);
	byte* memory = read_game();
	//state.memory = read_game();
	state.memory = malloc(4096);
	state.memory[0] = 0xEA;
	state.memory[1] = 0x05; //ORA $a0
	state.memory[2] = 0xA0;
	state.memory[3] = 0xEA; //NOP
	state.memory[4] = 0x09; //ORA #$ff
	state.memory[5] = 0xff; 
	state.memory[6] = 0xA9; //LDA
	state.memory[7] = 0x00; //0
	state.memory[8] = 0x0D; //ORA $1234
	state.memory[9] = 0x34;
	state.memory[10] = 0x12;
	state.memory[0xA0] = 0x13;
	state.memory[0x1234] = 0xAA;

	for (int i = 0; i < 10; i++) {
		print_all(&state);
		disassemble_6502(state.memory, state.pc);
		emulate_6502_op(&state);
	}
}