#include "test_framework.h"
#include "disassembler.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#ifdef _DEBUG
#include <signal.h>
#endif

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

void print_all(State6502 * state) {
	print_state(state);
	print_memory(state, 0);
	//print_memory(state, 0x20);
	//print_memory(state, 0x40);
	//print_memory(state, 0x80);
	printf("\n");
}

void test_step(State6502 * state) {
	print_all(state);
	disassemble_6502(state->memory, state->pc);
	printf("\n");
	emulate_6502_op(state);
	print_all(state);
}

void test_step_until_break(State6502 * state) {
	do {
		print_all(state);
		disassemble_6502(state->memory, state->pc);
		printf("\n");
		emulate_6502_op(state);
	} while (state->flags.b != 1);
	print_all(state);
}

void test_cleanup(State6502 * state) {
	free(state->memory);
}

State6502 create_blank_state() {
	State6502 state;
	clear_state(&state);
	const int memory_size = 65536;
	state.memory = malloc(memory_size);
	memset(state.memory, 0, sizeof(byte) * memory_size);
	return state;
}

void exit_or_break() {
#ifdef _DEBUG
	raise(SIGINT);
#endif
	exit(1);
}

void assert_register(State6502 * state, byte expected, byte actual, char* name) {
	if (actual != expected) {
		printf("Unexpected value in %s, expected %02X, was %02X", name, expected, actual);
		exit_or_break();
	}
}

void assertA(State6502 * state, byte expected) {
	assert_register(state, expected, state->a, "A");
}

void assertX(State6502 * state, byte expected) {
	assert_register(state, expected, state->x, "X");
}

void assertY(State6502 * state, byte expected) {
	assert_register(state, expected, state->y, "Y");
}

void assert_sp(State6502 * state, byte expected) {
	assert_register(state, expected, state->sp, "SP");
}

void assert_pc(State6502 * state, word expected) {
	if (state->pc != expected) {
		printf("Unexpected value in PC, expected %02X, was %02X", expected, state->pc);
		exit_or_break();
	}
}

//assert_memory(&state, 0xFF, 0x99)
void assert_memory(State6502 * state, word address, byte expected) {
	if (state->memory[address] != expected) {
		printf("Unexpected value in $%04X, expected %02X, was %02X", address, expected, state->memory[address]);
		exit_or_break();
	}
}

void assert_flag(byte flag_value, byte expected, char* flag_name) {
	if (flag_value != expected) {
		printf("Unexpected value in flag %s, expected %d, was %d", flag_name, expected, flag_value);
		exit_or_break();
	}
}

void assert_flag_n(State6502 * state, byte expected) {
	assert_flag(state->flags.n, expected, "N");
}

void assert_flag_z(State6502 * state, byte expected) {
	assert_flag(state->flags.z, expected, "Z");
}

void assert_flag_c(State6502 * state, byte expected) {
	assert_flag(state->flags.c, expected, "C");
}

void assert_flag_i(State6502 * state, byte expected) {
	assert_flag(state->flags.i, expected, "I");
}

void assert_flag_d(State6502 * state, byte expected) {
	assert_flag(state->flags.d, expected, "D");
}

void assert_flag_v(State6502 * state, byte expected) {
	assert_flag(state->flags.v, expected, "V");
}

void assert_flag_b(State6502 * state, byte expected) {
	assert_flag(state->flags.b, expected, "B");
}