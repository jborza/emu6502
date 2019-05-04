#pragma once
//#include <cstdint>
#include "types.h"

typedef struct Flags {
	//flags high to low: NV-BDIZC
	uint8_t c : 1;  //0 - set if last operation caused an overflow from bit 7 of the result or an underflow from bit 0
	uint8_t z : 1; //1 - set if the result of the last operation was zero
	uint8_t i : 1; //2 - set with SEI and cleared with CLI
	uint8_t d : 1; //3 - set with SEC and cleared with CLD
	uint8_t b : 1; //4 - set when BRK executed
	uint8_t pad : 1; //5 - not used
	uint8_t v : 1; //6 - if the result has yielded an invalid two's complement result
	uint8_t n : 1; //7 - set if the result of the last operation set bit 7 to one
} Flags;