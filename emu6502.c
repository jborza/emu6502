// emu6502.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include "state.h"
#include "cpu.h"
#include "disassembler.h"
#include <memory.h>
#include "opcodes.h"

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
	//print_memory(state, 0x20);
	//print_memory(state, 0x40);
	//print_memory(state, 0x80);
	printf("\n");

}

State6502 create_blank_state() {
	State6502 state;
	clear_state(&state);
	state.memory = malloc(4096);
	memset(state.memory, 0, sizeof(byte) * 4096);
	return state;
}

void assertA(State6502* state, byte expected) {
	if (state->a != expected) {
		printf("Unexpected value in A, was %02x, expected %02x", expected, state->a);
		exit(1);
	}
}

void test_LDA_IMM() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_IMM, 0xAA }; //LDA #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	print_all(&state);
	disassemble_6502(state.memory, state.pc);
	emulate_6502_op(&state);
	print_all(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	free(state.memory);
}

void test_LDA_ZP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_ZP, 0x03, 0x00, 0xAA }; //LDA $3
	memcpy(state.memory, program, sizeof(program));

	//act
	print_all(&state);
	disassemble_6502(&state.memory, state.pc);
	emulate_6502_op(&state);
	print_all(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	free(state.memory);
}

void test_LDA_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;

	//arrange
	char program[] = { LDA_ZPX, 0x02, 0x00, 0xAA }; //LDA $2,x
	memcpy(state.memory, program, sizeof(program));

	//act
	print_all(&state);
	disassemble_6502(&state.memory, state.pc);
	emulate_6502_op(&state);
	print_all(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	free(state.memory);
}

int main()
{
	test_LDA_IMM();
	//test_LDA_ZP();
	//test_LDA_ZPX();
}

int emulate() {
	State6502 state;
	memset(&state.memory, 0, sizeof(State6502));
	print_state(&state);
	clear_state(&state);
	byte* memory = read_game();
	//state.memory = read_game();
	state.memory = malloc(4096);
	memset(state.memory, 0, sizeof(byte) * 4096);
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
	state.memory[10] = 0x02;
	state.memory[0xA0] = 0x13;
	state.memory[0x0234] = 0xAA;

	for (int i = 0; i < 10; i++) {
		print_all(&state);
		disassemble_6502(state.memory, state.pc);
		emulate_6502_op(&state);
	}
}