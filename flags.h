#pragma once
#include <cstdint>

typedef struct Flags {
	uint8_t c : 1;
	uint8_t z : 1;
	uint8_t i : 1;
	uint8_t d : 1;
	uint8_t b : 1;
	uint8_t pad : 1;
	uint8_t v : 1;
	uint8_t n : 1;
} Flags;