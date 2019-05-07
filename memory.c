#include "memory.h"

byte fetch_byte(State6502* state) {
	return state->memory[state->pc++];
}

word fetch_word(State6502* state) {
	byte low = fetch_byte(state);
	byte high = fetch_byte(state);
	word result = (high << 8) | low;
	return result;
}

word read_word(State6502 * state, word address) {
	return state->memory[address] | state->memory[address + 1] << 8;
}

word read_word_wrap(State6502 * state, word address) {
	word address_low = address;
	//page wraparound
	word address_high = (address_low & 0xFF) == 0xFF ? address - 0xFF : address_low + 1;
	return state->memory[address_low] | state->memory[address_high] << 8;
}

word get_address_zero_page(State6502 * state) {
	return fetch_byte(state);
}

byte get_byte_zero_page(State6502 * state) {
	//8 bit addressing, only the first 256 bytes of the memory
	return state->memory[get_address_zero_page(state)];
}

word get_address_zero_page_x(State6502 * state) {
	//address is zero page, so wraparound byte
	byte address = fetch_byte(state) + state->x;
	return address;
}

byte get_byte_zero_page_x(State6502 * state) {
	return state->memory[get_address_zero_page_x(state)];
}

word get_address_zero_page_y(State6502 * state) {
	//address is zero page, so wraparound byte
	byte address = fetch_byte(state) + state->y;
	return address;
}

byte get_byte_zero_page_y(State6502 * state) {
	return state->memory[get_address_zero_page_y(state)];
}

word get_address_absolute(State6502 * state) {
	//absolute indexed, 16 bits
	word address = fetch_word(state);
	return address;
}

byte get_byte_absolute(State6502 * state)
{
	//absolute indexed, 16 bits
	return state->memory[get_address_absolute(state)];
}

word get_address_absolute_x(State6502 * state) {
	//absolute added with the contents of x register
	word address = fetch_word(state) + state->x;
	return address;
}

byte get_byte_absolute_x(State6502 * state) {
	return state->memory[get_address_absolute_x(state)];
}

word get_address_absolute_y(State6502 * state) {
	//absolute added with the contents of x register
	word address = fetch_word(state) + state->y;
	return address;
}

byte get_byte_absolute_y(State6502 * state) {
	//absolute added with the contents of y register
	return state->memory[get_address_absolute_y(state)];
}

word get_address_indirect_jmp(State6502 * state) {
	word indirect_address = fetch_word(state);
	//AN INDIRECT JUMP MUST NEVER USE A	VECTOR BEGINNING ON THE LAST BYTE OF A PAGE
	return read_word_wrap(state, indirect_address);
}

word get_address_indirect_x(State6502 * state) {
	//pre-indexed indirect with the X register
	//zero-page address is added to x register
	byte indirect_address = fetch_byte(state) + state->x;
	//pointing to address of a word holding the address of the operand
	word address = read_word_wrap(state, indirect_address);
	return address;
}

byte get_byte_indirect_x(State6502 * state) {
	//pre-indexed indirect with the X register
	return state->memory[get_address_indirect_x(state)];
}

word get_address_indirect_y(State6502 * state) {
	//post-indexed indirect
	//zero-page address as an argument
	byte indirect_address = fetch_byte(state);
	//the address and the following byte is read as a word, adding Y register
	word address = read_word_wrap(state, indirect_address) + state->y;
	return address;
}

byte get_byte_indirect_y(State6502 * state) {
	return state->memory[get_address_indirect_y(state)];
}

word get_address_relative(State6502 * state) {
	int8_t address = (int8_t)fetch_byte(state);
	return state->pc + address;
}