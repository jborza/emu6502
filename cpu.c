#include "state.h"
#include "cpu.h"
#include "opcodes.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

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
	clear_flags(state);
	state->running = 1;
}

byte pop_byte(State6502* state) {
	return state->memory[state->pc++];
}

//bitwise or with accumulator
void ORA(State6502 * state, byte operand) {
	byte result = state->a | operand;
	set_NV_flags(state, result);
	state->a = result;
}

//load accumulator
void LDA(State6502 * state, byte operand) {
	state->a = operand;
	set_NV_flags(state, state->a);
}

word pop_word(State6502 * state) {
	byte low = pop_byte(state);
	byte high = pop_byte(state);
	word result = (high << 8) | low;
	return result;
}

word read_word(State6502 * state, word address) {
	return state->memory[address] | state->memory[address + 1] << 8;
}

int emulate_6502_op(State6502 * state) {
	byte* opcode = &state->memory[state->pc++];
	switch (*opcode) {
	case BRK: state->running = 0;
		state->flags.b = 1;
		break; //BRK
	case NOP: break; //NOP
	case ORA_INDX: //ORA, indirect, x
	{
		//pre-indexed indirect
		//zero-page address is added to x register
		byte indirect_address = pop_byte(state) + state->x;
		//pointing to address of a word holding the address of the operand
		word address = read_word(state, indirect_address);
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_ZP: //ORA, zero page
	{
		byte address = pop_byte(state);
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_INDY: //ORA, indirect, y (post_indexed)
	{
		//post-indexed indirect
		//zero-page address as an argument
		byte indirect_address = pop_byte(state);
		//the address and the following byte is read as a word, adding Y register
		word address = read_word(state, indirect_address) + state->y;
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_IMM:
		ORA(state, pop_byte(state));
		break;
	case ORA_ZPX:
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
	case ORA_ABSX:
	{
		word address = pop_word(state) + state->x;
		ORA(state, state->memory[address]);
		break;
	}
	case ORA_ABSY:
	{
		word address = pop_word(state) + state->y;
		ORA(state, state->memory[address]);
		break;
	}
	case LDA_IMM:
	{
		LDA(state, pop_byte(state));
		break;
	}
	case LDA_ZP:
	{
		byte address = pop_byte(state);
		LDA(state, state->memory[address]);
		break;
	}
	case LDA_ZPX:
	{
		byte address = pop_byte(state) + state->x;
		LDA(state, state->memory[address]);
		break;
	}
	case LDA_ABS:
	{
		word address = pop_word(state);
		LDA(state, state->memory[address]);
		break;
	}
	case LDA_ABSX:
	{
		word address = pop_word(state) + state->x;
		LDA(state, state->memory[address]);
		break;
	}
	case LDA_ABSY:
	{
		word address = pop_word(state) + state->y;
		LDA(state, state->memory[address]);
		break;
	}
	case LDA_INDX:
	{
		//pre-indexed indirect
		//zero-page address is added to x register
		byte indirect_address = pop_byte(state) + state->x;
		//pointing to address of a word holding the address of the operand
		word address = read_word(state, indirect_address);
		LDA(state, state->memory[address]);
		break;
	}	
	case LDA_INDY:
	{
		//post-indexed indirect
		//zero-page address as an argument
		byte indirect_address = pop_byte(state);
		//the address and the following byte is read as a word, adding Y register
		word address = read_word(state, indirect_address) + state->y;
		LDA(state, state->memory[address]);
		break;
	}
	default:
		unimplemented_instruction(state); break;
	}
	return 0;
}
