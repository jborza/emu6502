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
	byte buffer[NESTEST_SIZE];
	int read = fread(&buffer, sizeof(byte), NESTEST_SIZE, file);
	fclose(file);
	return buffer;
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
	do{
		disassemble_6502(state.memory, state.pc);
		emulate_6502_op(&state);
		printf("    A:%02X X:%02X Y:%02X P:%02X SP:%02X\n", state.a, state.x, state.y, flags_as_byte(&state), state.sp);
	} while (state.flags.b != 1);
}

int main()
{
	run_nestest();
	//run_tests();
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