#pragma once
#include "types.h"

void* unimplemented_instruction(State6502* state);
int emulate_6502_op(State6502* state);

void clear_flags(State6502* state);
void clear_state(State6502* state);