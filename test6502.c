#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "test6502.h"
#include "opcodes.h"
#include "state.h"
#include "disassembler.h"
#include "cpu.h"


void print_state(State6502* state) {
	printf("\tC=%d,Z=%d,I=%d,D=%d,B=%d,V=%d,N=%d\n", state->flags.c, state->flags.z, state->flags.i, state->flags.d, state->flags.b, state->flags.v, state->flags.n);
	printf("\tA $%02x X $%02x Y $%02x SP $%02x PC $%04x\n", state->a, state->x, state->y, state->sp, state->pc);
}

void print_memory(State6502* state, word offset) {
	printf("$%04x: ", offset);
	for (byte i = 0; i < 32; i++) {
		printf("%02x", state->memory[offset + i]);
		if (i % 8 == 7)
			printf("|");
		else
			printf(" ");
	}
	printf("\n");
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

////////////////////////////////////////


//

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

/////////////////////
typedef void fp();
fp* tests[4] = { test_LDA_IMM, test_LDA_ZP, test_LDA_ZPX, test_LDA_ABS };

void run_tests() {
	for(int i = 0; i < sizeof(tests)/sizeof(fp*); i++)
		tests[i]();
	test_LDA_IMM();
	test_LDA_ZP();
	test_LDA_ZPX();
	test_LDA_ABS();
	test_LDA_ABSX();
	test_LDA_ABSY();
	test_LDA_INDX();
	test_LDA_INDY();
}