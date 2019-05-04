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
#include <errno.h>
#include <direct.h>

#define NESTEST_SIZE 0x4000
#define NESTEST_DST 0xC000
#define MEMORY_SIZE 0xFFFF

byte* read_nestest() {
	FILE* file = fopen("nestest\\nestest.bin", "rb");
	if (!file) {
		int err = errno;
		exit(1);
	}
	static byte buffer[NESTEST_SIZE];
	int read = fread(&buffer, sizeof(byte), NESTEST_SIZE, file);
	fclose(file);
	return buffer;
}

byte debug_flags_as_byte(State6502* state) {
	byte flags_value = 0;
	memcpy(&flags_value, &state->flags, sizeof(Flags));
	return flags_value;
}

void run_nestest() {
	State6502 state;
	clear_state(&state);
	state.memory = malloc(MEMORY_SIZE);
	memset(state.memory, 0, MEMORY_SIZE);
	byte* bin = read_nestest();
	//const word TARGET = 0xC000;
	memcpy(state.memory + NESTEST_DST, bin, NESTEST_SIZE);
	memcpy(state.memory + 0x8000, bin, NESTEST_SIZE);
	state.pc = NESTEST_DST;
	//a little cheat to simulate probably a JSR and SEI at the beginning 
	state.sp = 0xfd;
	state.flags.i = 1;
	do{
		char* dasm = disassemble_6502_to_string(state.memory, state.pc);
		printf("%-50s  A:%02X X:%02X Y:%02X P:%02X SP:%02X\n", dasm, state.a, state.x, state.y, debug_flags_as_byte(&state), state.sp);
		emulate_6502_op(&state);
	} while (state.flags.b != 1);
}

int main()
{
	run_nestest();
	//run_tests();
	return 0;
}