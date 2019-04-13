#include "state.h"
#include "cpu.h"
#include "opcodes.h"
#include <stdio.h>

void* unimplemented_instruction(State6502* state) {
	printf("Error: unimplemented instruction\n");
	exit(1);
}

void set_NV_flags(State6502* state, word value) {
	//if(value)
}

void clear_flags(State6502* state) {
	memcpy(&state->flags, &state->a,1);
}

void clear_state(State6502* state) {
	state->a = 0;
	state->x = 0;
	state->y = 0;
	state->pc = 0;
	state->sp = 0;
	//state -> flags = (Flags)0;
	clear_flags(state);
	state->running = 1;
}

byte pop_byte(State6502* state) {
	return state->memory[state->pc++];
}

void print_state(State6502* state) {
	printf("\tC=%d,Z=%d,I=%d,D=%d,B=%d,V=%d,N=%d\n", state->flags.c, state->flags.z, state->flags.i, state->flags.d, state->flags.b, state->flags.v, state->flags.n);
	printf("\tA $%02x X $%02x Y $%02x SP $%02x PC $%04x\n", state->a, state->x, state->y, state->sp, state->pc);
}

void print_memory(State6502* state, word offset) {
	printf("$%04x: ", offset);
	for (byte i = 0; i < 32; i++) {
		printf("%02x", state->memory[offset + i]);
		if (i % 8 == 7)
			printf("|");
		else
			printf(" ");
	}
	printf("\n");
}

//bitwise or with accumulator
void ORA(State6502 * state, byte operand) {
	word result = state->a | operand;
	set_NV_flags(state, result);
	state->a = result & 0xff;
}

int emulate_6502_op(State6502 * state) {
	byte* opcode = &state->memory[state->pc++];
	switch (*opcode) {
	case 0x00: break; //NOP
	case ORA_IND_X: //ORA, indirect, x
		break;
	case ORA_ZP: //ORA, zero page
		byte zp = pop_byte(state);
		ORA(state, state->memory[zp]);
		break;
	case 0x09: //ORA, immediate
		

	default:
		unimplemented_instruction(state); break;
	}
	return 0;
}
