#include "state.h"
#include "cpu.h"
#include "opcodes.h"
#include <stdio.h>

void* unimplemented_instruction(State6502* state) {
	printf("Error: unimplemented instruction\n");
	exit(1);
}

void set_NV_flags(State6502* state, byte value) {
	//TODO implement V flag
	if (value) {
		state->flags.z = 0;
	}
	else {
		state->flags.z = 1;
	}
}

void clear_flags(State6502* state) {
	memcpy(&state->flags, &state->a, 1);
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
	byte result = state->a | operand;
	set_NV_flags(state, result);
	state->a = result;
}

//load accumulator
void LDA(State6502* state, byte operand) {
	state->a = operand;
	set_NV_flags(state, state->a);
}

word pop_word(State6502 * state) {
	byte low = pop_byte(state);
	byte high = pop_byte(state);
	word result = (high << 8) | low;
	return result;
}

word get_word(State6502 * state, word address) {
	return state->memory[address] << 8 | state->memory[address + 1];
}

int emulate_6502_op(State6502 * state) {
	byte* opcode = &state->memory[state->pc++];
	switch (*opcode) {
	case BRK: state->running = 0; 
		state->flags.b = 1;
		break; //BRK
	case NOP: break; //NOP
	case ORA_IND_X: //ORA, indirect, x
		unimplemented_instruction(state);
		break;
	case ORA_ZP: //ORA, zero page
	{
		byte address = pop_byte(state);
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_IND_Y:
		//The value in Y is added to the address at the little-endian address stored at the two-byte pair of the specified address (LSB) and the specified address plus one (MSB). 
		//The value at the sum address is used to perform the computation. Indeed addressing mode actually repeats exactly the accumulator register's digits. 
		// Example
		//The value $03 in Y is added to the address $C235 at addresses $002A and $002B for a sum of $C238. The value $2F at $C238 is shifted right (yielding $17) and written back to $C238. 
		//word address = pop_word(state) + state->y;
		//word value = get_word(state, address);
		//ORA(state, state->memory[value]);
		unimplemented_instruction(state);
		break;
	case ORA_IMM:
		ORA(state, pop_byte(state));
		break;
	case ORA_ZP_X:
	{
		byte address = pop_byte(state) + state->x;
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_ABS:
	{
		word address = pop_word(state);
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_ABS_X:
	{
		word address = pop_word(state) + state->x;
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_ABS_Y:
	{
		word address = pop_word(state) + state->y;
		ORA(state, state->memory[address]);
		break;
	}
	case LDA_IMM:
	{
		LDA(state,pop_byte(state));
		break;
	}
	break;
	default:
		unimplemented_instruction(state); break;
	}
	return 0;
}
