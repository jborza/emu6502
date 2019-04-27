#include "memory.h"

word read_word(State6502* state, word address) {
	return state->memory[address] | state->memory[address + 1] << 8;
}

word get_address_zero_page(State6502* state) {
	return pop_byte(state);
}

byte get_byte_zero_page(State6502* state) {
	//8 bit addressing, only the first 256 bytes of the memory
	return state->memory[get_address_zero_page(state)];
}

word get_address_zero_page_x(State6502* state) {
	//address is zero page, so wraparound byte
	byte address = pop_byte(state) + state->x;
	return address;
}

byte get_byte_zero_page_x(State6502* state) {
	return state->memory[get_address_zero_page_x(state)];
}

word get_address_zero_page_y(State6502* state) {
	//address is zero page, so wraparound byte
	byte address = pop_byte(state) + state->y;
	return address;
}

byte get_byte_zero_page_y(State6502* state) {
	return state->memory[get_address_zero_page_y(state)];
}

word get_address_absolute(State6502* state) {
	//absolute indexed, 16 bits
	word address = pop_word(state);
	return address;
}

byte get_byte_absolute(State6502* state)
{
	//absolute indexed, 16 bits
	return state->memory[get_address_absolute(state)];
}

word get_address_absolute_x(State6502* state) {
	//absolute added with the contents of x register
	word address = pop_word(state) + state->x;
	return address;
}

byte get_byte_absolute_x(State6502* state) {
	return state->memory[get_address_absolute_x(state)];
}

word get_address_absolute_y(State6502* state) {
	//absolute added with the contents of x register
	word address = pop_word(state) + state->y;
	return address;
}

byte get_byte_absolute_y(State6502* state) {
	//absolute added with the contents of y register
	return state->memory[get_address_absolute_y(state)];
}

word get_address_indirect_jmp(State6502* state) {
	//AN INDIRECT JUMP MUST NEVER USE A	VECTOR BEGINNING ON THE LAST BYTE OF A PAGE
	word indirect_address = pop_word(state);
	if ((indirect_address & 0xFF) == 0xFF) {
		//avoid crossing the page boundary
		return state->memory[indirect_address] | state->memory[indirect_address - 0xFF] << 8;
	}
	else {
		return read_word(state, indirect_address);
	}
}

word get_address_indirect_x(State6502* state) {
	//pre-indexed indirect with the X register
	//zero-page address is added to x register
	byte indirect_address = pop_byte(state) + state->x;
	//pointing to address of a word holding the address of the operand
	word address = read_word(state, indirect_address);
	return address;
}

byte get_byte_indirect_x(State6502* state) {
	//pre-indexed indirect with the X register
	return state->memory[get_address_indirect_x(state)];
}

word get_address_indirect_y(State6502* state) {
	//post-indexed indirect
	//zero-page address as an argument
	byte indirect_address = pop_byte(state);
	//the address and the following byte is read as a word, adding Y register
	word address = read_word(state, indirect_address) + state->y;
	return address;
}

byte get_byte_indirect_y(State6502* state) {
	return state->memory[get_address_indirect_y(state)];
}

word get_address_relative(State6502* state) {
	int8_t address = (int8_t)pop_byte(state);
	return state->pc + address;
}