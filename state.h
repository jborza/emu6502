#pragma once
#include <cstdint>
#include "flags.h"

typedef struct State6502 {
	uint8_t A; //accumulator
	uint8_t X; //x index
	uint8_t Y; //y index
	uint8_t SP; //stack pointer, 256 byte stack between $0100 and $01FF
	uint16_t PC; //program counter, points to the next instruction to be executed
	uint8_t* Memory;
	Flags Flags; //CPU flags
} State6502;