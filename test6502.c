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
		printf("Unexpected value in A, expected %02X, was %02X", expected, state->a);
		exit(1);
	}
}

void assertX(State6502* state, byte expected) {
	if (state->x != expected) {
		printf("Unexpected value in X, expected %02X, was %02X", expected, state->x);
		exit(1);
	}
}

void assertY(State6502* state, byte expected) {
	if (state->y != expected) {
		printf("Unexpected value in X, expected %02X, was %02X", expected, state->y);
		exit(1);
	}
}

////////////////////////////////////////


//// LDA

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
	char program[] = { LDA_ZPX, 0x02, 0x00, 0xAA }; //LDA $02,X
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
	char program[] = { LDA_ABS, 0x01, 0x04 }; //LDA $0401
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
	char program[] = { LDA_ABSX, 0x01, 0x04 }; //LDA $0401,x
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
	char program[] = { LDA_ABSY, 0x01, 0x04 }; //LDA $0401,y
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
	char program[] = { LDA_INDY, 0x3E }; //LDA ($3E),y
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

//// ORA

void test_ORA_IMM() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_IMM, 0xA0 }; //ORA #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	test_cleanup(&state);
}

void test_ORA_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ZP, 0x03, 0x00, 0xA0 }; //ORA $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ZPX, 0x02, 0x00, 0xA0 }; //ORA $2,x
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ABS, 0x01, 0x04 }; //ORA $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ABSX, 0x01, 0x04 }; //ORA $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_ABSY, 0x01, 0x04}; //ORA $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x05;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_INDX, 0x3E }; //ORA ($3E, x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0043] = 0xA9;
	state.memory[0x0044] = 0x04;
	state.memory[0x04A9] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_ORA_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x05;
	state.a = 0x0A;

	//arrange
	char program[] = { ORA_INDY, 0x3E, 0x04, 0xAA }; //ORA ($3E),y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x3E] = 0xA0; //0x04A0
	state.memory[0x3F] = 0x04;
	state.memory[0x04A5] = 0xA0; //address 0x04A0 + 0x05 = 0x04A5

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

//// AND

void test_AND_IMM() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0xAA;

	//arrange
	char program[] = { AND_IMM, 0xA0 }; //AND #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	test_cleanup(&state);
}

void test_AND_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ZP, 0x03, 0x00, 0xA0 }; //AND $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ZPX, 0x02, 0x00, 0xA0 }; //AND $2,x
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ABS, 0x01, 0x04 }; //AND $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ABSX, 0x01, 0x04 }; //AND $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_ABSY, 0x01, 0x04 }; //AND $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_INDX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x05;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_INDX, 0x3E }; //AND ($3E, x)
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x0043] = 0xA9;
	state.memory[0x0044] = 0x04;
	state.memory[0x04A9] = 0xA0;

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

void test_AND_INDY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x05;
	state.a = 0xAA;

	//arrange
	char program[] = { AND_INDY, 0x3E, 0x04, 0xAA }; //AND ($3E),y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x3E] = 0xA0; //0x04A0
	state.memory[0x3F] = 0x04;
	state.memory[0x04A5] = 0xA0; //address 0x04A0 + 0x05 = 0x04A5

	//act
	test_step(&state);

	//assert	
	assertA(&state, 0xA0);

	//cleanup
	test_cleanup(&state);
}

//// LDX

void test_LDX_IMM() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_IMM, 0xAA }; //LDX #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	test_cleanup(&state);
}

void test_LDX_ZP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_ZP, 0x03, 0x00, 0xAA }; //LDX $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDX_ZPY() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;

	//arrange
	char program[] = { LDX_ZPY, 0x02, 0x00, 0xAA }; //LDX $02,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDX_ABS() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDX_ABS, 0x01, 0x04 }; //LDX $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDX_ABSY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x02;

	//arrange
	char program[] = { LDX_ABSY, 0x01, 0x04 }; //LDX $0401,y
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertX(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

/////////////////////

typedef void fp();
fp* tests_lda[] = { test_LDA_IMM, test_LDA_ZP, test_LDA_ZPX, test_LDA_ABS, test_LDA_ABSX, test_LDA_ABSY, test_LDA_INDX, test_LDA_INDY };
fp* tests_ora[] = { test_ORA_IMM, test_ORA_ZP, test_ORA_ZPX, test_ORA_ABS, test_ORA_ABSX, test_ORA_ABSY, test_ORA_INDX, test_ORA_INDY};
fp* tests_and[] = { test_AND_IMM, test_AND_ZP, test_AND_ZPX, test_AND_ABS, test_AND_ABSX, test_AND_ABSY, test_AND_INDX, test_AND_INDY };
fp* tests_ldx[] = { test_LDX_IMM, test_LDX_ZP, test_LDX_ZPY, test_LDX_ABS, test_LDX_ABSY };

void run_suite(fp* suite[]) {
	for (int i = 0; i < sizeof(suite) / sizeof(fp*); i++)
		suite[i]();
}

void run_tests() {
	run_suite(tests_lda);
	run_suite(tests_ora);
	run_suite(tests_and);
	run_suite(tests_ldx);
}