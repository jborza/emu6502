#pragma once
#include "types.h"
#include "state.h"
void run_tests();
void print_memory(State6502* state, word offset);
void print_state(State6502* state);