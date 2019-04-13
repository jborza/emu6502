#pragma once
#include "flags.h"
#include "types.h"

typedef struct State6502 {
	byte a; //accumulator
	byte x; //x index
	byte y; //y index
	byte sp; //stack pointer, 256 byte stack between $0100 and $01FF
	word pc; //program counter, points to the next instruction to be executed
	byte* memory;
	Flags flags; //CPU flags
	int running;
} State6502;