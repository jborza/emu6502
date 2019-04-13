#include "state.h"
#include "cpu.h"

void* unimplemented_instruction(State6502* state) {
	printf("Error: unimplemented instruction\n");
	exit(1);
}

int emulate_6502_op(State6502* state) {
	byte* opcode = &state->memory[state->pc];
	switch (*opcode) {
		//...
	default:unimplemented_instruction(state); break;
	}
	state->pc += 1;
	return 0;
}
