#pragma once
#include "types.h"

void* unimplemented_instruction(State6502* state);
int emulate_6502_op(State6502* state);
void print_memory(State6502* state, word offset);
void print_state(State6502* state);
void clear_flags(State6502* state);
void clear_state(State6502* state);