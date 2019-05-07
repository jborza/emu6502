#pragma once
#include "state.h"

byte fetch_byte(State6502* state);

word fetch_word(State6502* state);

word read_word(State6502* state, word address);

word read_word_wrap(State6502* state, word address);

word get_address_zero_page(State6502* state);

byte get_byte_zero_page(State6502* state);

word get_address_zero_page_x(State6502* state);

byte get_byte_zero_page_x(State6502* state);

word get_address_zero_page_y(State6502* state);

byte get_byte_zero_page_y(State6502* state);

word get_address_absolute(State6502* state);

byte get_byte_absolute(State6502* state);

word get_address_absolute_x(State6502* state);

byte get_byte_absolute_x(State6502* state);

word get_address_absolute_y(State6502* state);

byte get_byte_absolute_y(State6502* state);

word get_address_indirect_jmp(State6502* state);

word get_address_indirect_x(State6502* state);

byte get_byte_indirect_x(State6502* state);

word get_address_indirect_y(State6502* state);

byte get_byte_indirect_y(State6502* state);

word get_address_relative(State6502* state);