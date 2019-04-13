#pragma once
#include "flags.h"
#include "types.h"

typedef struct State6502 {
	byte a; //accumulator
	byte X; //x index
	byte Y; //y index
	byte sp; //stack pointer, 256 byte stack between $0100 and $01FF
	word pc; //program counter, points to the next instruction to be executed
	byte* memory;
	Flags flags; //CPU flags
} State6502;