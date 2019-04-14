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

void test_step(State6502* state) {
	print_all(state);
	disassemble_6502(state->memory, state->pc);
	emulate_6502_op(state);
	print_all(state);
}

void test_cleanup(State6502* state) {
	free(state->memory);
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
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	test_cleanup(&state);
}

void test_LDA_ZP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_ZP, 0x03, 0x00, 0xAA }; //LDA $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;

	//arrange
	char program[] = { LDA_ZPX, 0x02, 0x00, 0xAA }; //LDA $2,x
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ABS() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDA_ABS, 0x01, 0x04, 0xAA }; //LDA $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;

	//arrange
	char program[] = { LDA_ABSX, 0x01, 0x04, 0xAA }; //LDA $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;

	//arrange
	char program[] = { LDA_ABSY, 0x01, 0x04, 0xAA }; //LDA $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x05;

	//arrange
	char program[] = { LDA_INDX, 0x3E }; //LDA ($3E, x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0043] = 0xA9;
	state.memory[0x0044] = 0x04;
	state.memory[0x04A9] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDA_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x05;

	//arrange
	char program[] = { LDA_INDY, 0x3E, 0x04, 0xAA }; //LDA ($3E),y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x3E] = 0xA0; //0x04A0
	state.memory[0x3F] = 0x04; 
	state.memory[0x04A5] = 0xAA; //address 0x04A0 + 0x05 = 0x04A5

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

int main()
{
	//test_LDA_IMM();
	//test_LDA_ZP();
	//test_LDA_ZPX();
	//test_LDA_ABS();
	//test_LDA_ABSX();
	//test_LDA_ABSY();
	test_LDA_INDX();
	test_LDA_INDY();
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