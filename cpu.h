#pragma once
#include "types.h"

#define STACK_HOME 0x100

void* unimplemented_instruction(State6502* state);
int emulate_6502_op(State6502* state);

void clear_flags(State6502* state);
void clear_state(State6502* state);
byte flags_as_byte(State6502* state);