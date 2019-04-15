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
	printf("\tA $%02X X $%02X Y $%02X SP $%02X PC $%04X\n", state->a, state->x, state->y, state->sp, state->pc);
}

void print_memory(State6502* state, word offset) {
	printf("$%04X: ", offset);
	for (byte i = 0; i < 32; i++) {
		printf("%02X", state->memory[offset + i]);
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

//assert_memory(&state, 0xFF, 0x99)
void assert_memory(State6502* state, word address, byte expected) {
	if (state->memory[address] != expected) {
		printf("Unexpected value in $%04X, expected %02X, was %02X", address, expected, state->memory[address]);
		exit(1);
	}
}

void assert_flag_n(State6502* state, byte expected) {
	if (state->flags.n != expected) {
		printf("Unexpected value in flag N, expected %d, was %d", expected, state->flags.n);
		exit(1);
	}
}

void assert_flag_z(State6502* state, byte expected) {
	if (state->flags.z != expected) {
		printf("Unexpected value in flag N, expected %d, was %d", expected, state->flags.z);
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

void test_LDA_ZPX_wraparound() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xFF;

	//arrange
	char program[] = { LDA_ZPX, 0x04, 0x00, 0xAA }; //LDA $04,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	//should wrap $FF+$04 around to $03
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
	state.y = 0x01;

	//arrange
	char program[] = { LDX_ZPY, 0x02, 0x00, 0xAA }; //LDX $02,Y
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

//// LDY

void test_LDY_IMM() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDY_IMM, 0xAA }; //LDY #$AA
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	test_cleanup(&state);
}

void test_LDY_ZP() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDY_ZP, 0x03, 0x00, 0xAA }; //LDY $3
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDY_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x01;

	//arrange
	char program[] = { LDY_ZPX, 0x02, 0x00, 0xAA }; //LDY $02,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDY_ABS() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { LDY_ABS, 0x01, 0x04 }; //LDY $0401
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x401] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

void test_LDY_ABSX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x02;

	//arrange
	char program[] = { LDY_ABSX, 0x01, 0x04 }; //LDY $0401,x
	memcpy(state.memory, program, sizeof(program));
	state.memory[0x403] = 0xAA;

	//act
	test_step(&state);

	//assert	
	assertY(&state, 0xAA);

	//cleanup
	test_cleanup(&state);
}

//// STX

void test_STX_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x99;

	//arrange
	char program[] = { STX_ZP, 0xFF}; //STX $FF
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xFF, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STX_ZPY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xA0;
	state.x = 0x99;

	//arrange
	char program[] = { STX_ZPY, 0x02}; //STX $02,Y
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xA2, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STX_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0x99;

	//arrange
	char program[] = { STX_ABS, 0x01, 0x04 }; //STX $0401
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x401, 0x99);

	//cleanup
	test_cleanup(&state);
}

//// STY

void test_STY_ZP() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x99;

	//arrange
	char program[] = { STY_ZP, 0xFF }; //STY $FF
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xFF, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STY_ZPX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xA0;
	state.y = 0x99;

	//arrange
	char program[] = { STY_ZPX, 0x02 }; //STY $02,X
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0xA2, 0x99);

	//cleanup
	test_cleanup(&state);
}

void test_STY_ABS() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0x99;

	//arrange
	char program[] = { STY_ABS, 0x01, 0x04 }; //STY $0401
	memcpy(state.memory, program, sizeof(program));

	//act
	test_step(&state);

	//assert	
	assert_memory(&state, 0x401, 0x99);

	//cleanup
	test_cleanup(&state);
}

// INX, INY, DEX, DEY

void test_DEX_wraparound() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { DEX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xFF);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_DEX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xE1;

	//arrange
	char program[] = { DEX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xE0);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_DEY_wraparound() {
	//initialize
	State6502 state = create_blank_state();

	//arrange
	char program[] = { DEY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xFF);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_DEY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xE1;

	//arrange
	char program[] = { DEY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xE0);
	assert_flag_n(&state, 1);
	assert_flag_z(&state, 0);

	test_cleanup(&state);
}

void test_INX_wraparound() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xFF;

	//arrange
	char program[] = { INX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0x00);
	assert_flag_z(&state, 1);
	assert_flag_n(&state, 0);

	test_cleanup(&state);
}

void test_INX() {
	//initialize
	State6502 state = create_blank_state();
	state.x = 0xE1;

	//arrange
	char program[] = { INX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xE2);

	test_cleanup(&state);
}

void test_INY_wraparound() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xFF;

	//arrange
	char program[] = { INY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0x00);
	assert_flag_z(&state, 1);

	test_cleanup(&state);
}

void test_INY() {
	//initialize
	State6502 state = create_blank_state();
	state.y = 0xE1;

	//arrange
	char program[] = { INY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xE2);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

// TXA, TAX, TYA, TAY

void test_TXA() {
	State6502 state = create_blank_state();
	state.x = 0xEE;

	char program[] = { TXA };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertA(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}

void test_TAX() {
	State6502 state = create_blank_state();
	state.a = 0xEE;

	char program[] = { TAX };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertX(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}


void test_TYA() {
	State6502 state = create_blank_state();
	state.y = 0xEE;

	char program[] = { TYA };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertA(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}


void test_TAY() {
	State6502 state = create_blank_state();
	state.a = 0xEE;

	char program[] = { TAY };
	memcpy(state.memory, program, sizeof(program));

	test_step(&state);

	assertY(&state, 0xEE);
	assert_flag_z(&state, 0);
	assert_flag_n(&state, 1);

	test_cleanup(&state);
}




/////////////////////

typedef void fp();
fp* tests_lda[] = { test_LDA_IMM, test_LDA_ZP, test_LDA_ZPX, test_LDA_ZPX_wraparound, test_LDA_ABS, test_LDA_ABSX, test_LDA_ABSY, test_LDA_INDX, test_LDA_INDY };
fp* tests_ora[] = { test_ORA_IMM, test_ORA_ZP, test_ORA_ZPX, test_ORA_ABS, test_ORA_ABSX, test_ORA_ABSY, test_ORA_INDX, test_ORA_INDY};
fp* tests_and[] = { test_AND_IMM, test_AND_ZP, test_AND_ZPX, test_AND_ABS, test_AND_ABSX, test_AND_ABSY, test_AND_INDX, test_AND_INDY };
fp* tests_ldx[] = { test_LDX_IMM, test_LDX_ZP, test_LDX_ZPY, test_LDX_ABS, test_LDX_ABSY };
fp* tests_ldy[] = { test_LDY_IMM, test_LDY_ZP, test_LDY_ZPX, test_LDY_ABS, test_LDY_ABSX };
fp* tests_stx[] = { test_STX_ZP, test_STX_ZPY, test_STX_ABS};
fp* tests_sty[] = { test_STY_ZP, test_STY_ZPX, test_STY_ABS };
fp* tests_inx_iny_dex_dey[] = { test_DEX, test_DEX_wraparound, test_DEY, test_DEY_wraparound, test_INX, test_INX_wraparound, test_INY, test_INY_wraparound};
fp* tests_txa_etc[] = { test_TXA, test_TAX, test_TYA, test_TAY };

#define RUN(suite) run_suite(suite, sizeof(suite)/sizeof(fp*))

void run_suite(fp** suite, int size) {
	for (int i = 0; i < size; i++)
		suite[i]();
}

void run_tests() {
	RUN(tests_lda);
	RUN(tests_ora);
	RUN(tests_and);
	RUN(tests_ldx);
	RUN(tests_lda);
	RUN(tests_ldy);
	RUN(tests_stx);
	RUN(tests_sty);
	RUN(tests_inx_iny_dex_dey);
	RUN(tests_txa_etc);
}