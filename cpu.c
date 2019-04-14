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
void ORA(State6502* state, byte operand) {
	byte result = state->a | operand;
	set_NV_flags(state, result);
	state->a = result;
}

//bitwise and with accumulator
void AND(State6502* state, byte operand) {
	byte result = state->a & operand;
	set_NV_flags(state, result);
	state->a = result;
}

//load accumulator
void LDA(State6502* state, byte operand) {
	state->a = operand;
	set_NV_flags(state, state->a);
}

void LDX(State6502* state, byte operand) {
	state->x = operand;
	set_NV_flags(state, state->x);
}

void LDY(State6502* state, byte operand) {
	state->y = operand;
	set_NV_flags(state, state->y);
}

word pop_word(State6502* state) {
	byte low = pop_byte(state);
	byte high = pop_byte(state);
	word result = (high << 8) | low;
	return result;
}

word read_word(State6502 * state, word address) {
	return state->memory[address] | state->memory[address + 1] << 8;
}

byte get_byte_zero_page(State6502 * state) {
	//8 bit addressing, only the first 256 bytes of the memory
	byte address = pop_byte(state);
	return state->memory[address];
}

byte get_byte_zero_page_x(State6502 * state) {
	byte address = pop_byte(state) + state->x;
	return state->memory[address];
}

byte get_byte_zero_page_y(State6502 * state) {
	byte address = pop_byte(state) + state->y;
	return state->memory[address];
}

byte get_byte_absolute(State6502 * state)
{
	//absolute indexed, 16 bits
	word address = pop_word(state);
	return state->memory[address];
}

byte get_byte_absolute_x(State6502 * state) {
	//absolute added with the contents of x register
	word address = pop_word(state) + state->x;
	return state->memory[address];
}

byte get_byte_absolute_y(State6502 * state) {
	//absolute added with the contents of y register
	word address = pop_word(state) + state->y;
	return state->memory[address];
}

byte get_byte_indirect_x(State6502 * state) {
	//pre-indexed indirect with the X register
	//zero-page address is added to x register
	byte indirect_address = pop_byte(state) + state->x;
	//pointing to address of a word holding the address of the operand
	word address = read_word(state, indirect_address);
	return state->memory[address];
}

byte get_byte_indirect_y(State6502 * state) {
	//post-indexed indirect
	//zero-page address as an argument
	byte indirect_address = pop_byte(state);
	//the address and the following byte is read as a word, adding Y register
	word address = read_word(state, indirect_address) + state->y;
	return state->memory[address];
}

int emulate_6502_op(State6502 * state) {
	byte* opcode = &state->memory[state->pc++];
	switch (*opcode) {
	case ADC_IMM: unimplemented_instruction(state); break;
	case ADC_ZP: unimplemented_instruction(state); break;
	case ADC_ZPX: unimplemented_instruction(state); break;
	case ADC_ABS: unimplemented_instruction(state); break;
	case ADC_ABSX: unimplemented_instruction(state); break;
	case ADC_ABSY: unimplemented_instruction(state); break;
	case ADC_INDX: unimplemented_instruction(state); break;
	case ADC_INDY: unimplemented_instruction(state); break;
	case AND_IMM: AND(state, pop_byte(state)); break;
	case AND_ZP: AND(state, get_byte_zero_page(state)); break;
	case AND_ZPX: AND(state, get_byte_zero_page_x(state)); break;
	case AND_ABS: AND(state, get_byte_absolute(state)); break;
	case AND_ABSX: AND(state, get_byte_absolute_x(state)); break;
	case AND_ABSY: AND(state, get_byte_absolute_y(state)); break;
	case AND_INDX: AND(state, get_byte_indirect_x(state)); break;
	case AND_INDY: AND(state, get_byte_indirect_y(state)); break;
	case ASL_ACC: unimplemented_instruction(state); break;
	case ASL_ZP: unimplemented_instruction(state); break;
	case ASL_ZPX: unimplemented_instruction(state); break;
	case ASL_ABS: unimplemented_instruction(state); break;
	case ASL_ABSX: unimplemented_instruction(state); break;
	case BCC_REL: unimplemented_instruction(state); break;
	case BCS_REL: unimplemented_instruction(state); break;
	case BEQ_REL: unimplemented_instruction(state); break;
	case BMI_REL: unimplemented_instruction(state); break;
	case BNE_REL: unimplemented_instruction(state); break;
	case BPL_REL: unimplemented_instruction(state); break;
	case BVC_REL: unimplemented_instruction(state); break;
	case BVS_REL: unimplemented_instruction(state); break;
	case BIT_ZP: unimplemented_instruction(state); break;
	case BIT_ABS: unimplemented_instruction(state); break;
	case BRK: state->running = 0;
		state->flags.b = 1;
		break; //BRK
	case CLC: unimplemented_instruction(state); break;
	case CLD: unimplemented_instruction(state); break;
	case CLI: unimplemented_instruction(state); break;
	case CLV: unimplemented_instruction(state); break;
	case NOP: break; //NOP
	case PHA: unimplemented_instruction(state); break;
	case PLA: unimplemented_instruction(state); break;
	case PHP: //push processor status
		//push(state->flags);
		unimplemented_instruction(state); break;
	case PLP: //pull procesor status 
		//state->flags = pop();
		unimplemented_instruction(state); break;
	case RTI: unimplemented_instruction(state); break;
	case RTS: unimplemented_instruction(state); break;
	case SEC: unimplemented_instruction(state); break;
	case SED: unimplemented_instruction(state); break;
	case SEI: unimplemented_instruction(state); break;
	case TAX: unimplemented_instruction(state); break;
	case TXA: unimplemented_instruction(state); break;
	case TAY: unimplemented_instruction(state); break;
	case TYA: unimplemented_instruction(state); break;
	case TSX: unimplemented_instruction(state); break;
	case TXS: unimplemented_instruction(state); break;
	case CMP_IMM: unimplemented_instruction(state); break;
	case CMP_ZP: unimplemented_instruction(state); break;
	case CMP_ZPX: unimplemented_instruction(state); break;
	case CMP_ABS: unimplemented_instruction(state); break;
	case CMP_ABSX: unimplemented_instruction(state); break;
	case CMP_ABSY: unimplemented_instruction(state); break;
	case CMP_INDX: unimplemented_instruction(state); break;
	case CMP_INDY: unimplemented_instruction(state); break;
	case CPX_IMM: unimplemented_instruction(state); break;
	case CPX_ZP: unimplemented_instruction(state); break;
	case CPX_ABS: unimplemented_instruction(state); break;
	case CPY_IMM: unimplemented_instruction(state); break;
	case CPY_ZP: unimplemented_instruction(state); break;
	case CPY_ABS: unimplemented_instruction(state); break;
	case DEC_ZP: unimplemented_instruction(state); break;
	case DEC_ZPX: unimplemented_instruction(state); break;
	case DEC_ABS: unimplemented_instruction(state); break;
	case DEC_ABSX: unimplemented_instruction(state); break;
	case DEX: unimplemented_instruction(state); break;
	case DEY: unimplemented_instruction(state); break;
	case INX: unimplemented_instruction(state); break;
	case INY: unimplemented_instruction(state); break;
	case EOR_IMM: unimplemented_instruction(state); break;
	case EOR_ZP: unimplemented_instruction(state); break;
	case EOR_ZPX: unimplemented_instruction(state); break;
	case EOR_ABS: unimplemented_instruction(state); break;
	case EOR_ABSX: unimplemented_instruction(state); break;
	case EOR_ABSY: unimplemented_instruction(state); break;
	case EOR_INDX: unimplemented_instruction(state); break;
	case EOR_INDY: unimplemented_instruction(state); break;
	case INC_ZP: unimplemented_instruction(state); break;
	case INC_ZPX: unimplemented_instruction(state); break;
	case INC_ABS: unimplemented_instruction(state); break;
	case INC_ABSX: unimplemented_instruction(state); break;
	case JMP_ABS: unimplemented_instruction(state); break;
	case JMP_IND: unimplemented_instruction(state); break;
	case JSR_ABS: unimplemented_instruction(state); break;
	case LDA_IMM: LDA(state, pop_byte(state)); break;
	case LDA_ZP: LDA(state, get_byte_zero_page(state));	break;
	case LDA_ZPX: LDA(state, get_byte_zero_page_x(state)); break;
	case LDA_ABS: LDA(state, get_byte_absolute(state)); break;
	case LDA_ABSX: LDA(state, get_byte_absolute_x(state)); break;
	case LDA_ABSY: LDA(state, get_byte_absolute_y(state)); break;
	case LDA_INDX: LDA(state, get_byte_indirect_x(state)); break;
	case LDA_INDY: LDA(state, get_byte_indirect_y(state)); break;
	case LDX_IMM: LDX(state, pop_byte(state)); break;
	case LDX_ZP: LDX(state, get_byte_zero_page(state)); break;
	case LDX_ZPY: LDX(state, get_byte_zero_page_y(state)); break;
	case LDX_ABS: LDX(state, get_byte_absolute(state)); break;
	case LDX_ABSY: LDX(state, get_byte_absolute_y(state)); break;
	case LDY_IMM: LDY(state, pop_byte(state)); break;
	case LDY_ZP: LDY(state, get_byte_zero_page(state)); break;
	case LDY_ZPX: LDY(state, get_byte_zero_page_x(state)); break;
	case LDY_ABS: LDY(state, get_byte_absolute(state)); break;
	case LDY_ABSX: LDY(state, get_byte_absolute_x(state)); break;
	case LSR_ACC: unimplemented_instruction(state); break;
	case LSR_ZP: unimplemented_instruction(state); break;
	case LSR_ZPX: unimplemented_instruction(state); break;
	case LSR_ABS: unimplemented_instruction(state); break;
	case LSR_ABSX: unimplemented_instruction(state); break;
	case ORA_IMM: ORA(state, pop_byte(state)); break;
	case ORA_ZP: ORA(state, get_byte_zero_page(state));	break;
	case ORA_ZPX: ORA(state, get_byte_zero_page_x(state)); break;
	case ORA_ABS: ORA(state, get_byte_absolute(state)); break;
	case ORA_ABSX: ORA(state, get_byte_absolute_x(state)); break;
	case ORA_ABSY: ORA(state, get_byte_absolute_y(state)); break;
	case ORA_INDX: ORA(state, get_byte_indirect_x(state)); break;
	case ORA_INDY: ORA(state, get_byte_indirect_y(state)); break;
	case ROL_ACC: unimplemented_instruction(state); break;
	case ROL_ZP: unimplemented_instruction(state); break;
	case ROL_ZPX: unimplemented_instruction(state); break;
	case ROL_ABS: unimplemented_instruction(state); break;
	case ROL_ABSX: unimplemented_instruction(state); break;
	case ROR_ACC: unimplemented_instruction(state); break;
	case ROR_ZP: unimplemented_instruction(state); break;
	case ROR_ZPX: unimplemented_instruction(state); break;
	case ROR_ABS: unimplemented_instruction(state); break;
	case ROR_ABSX: unimplemented_instruction(state); break;
	case SBC_IMM: unimplemented_instruction(state); break;
	case SBC_ZP: unimplemented_instruction(state); break;
	case SBC_ZPX: unimplemented_instruction(state); break;
	case SBC_ABS: unimplemented_instruction(state); break;
	case SBC_ABSX: unimplemented_instruction(state); break;
	case SBC_ABSY: unimplemented_instruction(state); break;
	case SBC_INDX: unimplemented_instruction(state); break;
	case SBC_INDY: unimplemented_instruction(state); break;
	case STA_ZP: unimplemented_instruction(state); break;
	case STA_ZPX: unimplemented_instruction(state); break;
	case STA_ABS: unimplemented_instruction(state); break;
	case STA_ABSX: unimplemented_instruction(state); break;
	case STA_ABSY: unimplemented_instruction(state); break;
	case STA_INDX: unimplemented_instruction(state); break;
	case STA_INDY: unimplemented_instruction(state); break;
	case STX_ZP: unimplemented_instruction(state); break;
	case STX_ZPY: unimplemented_instruction(state); break;
	case STX_ABS: unimplemented_instruction(state); break;
	case STY_ZP: unimplemented_instruction(state); break;
	case STY_ZPX: unimplemented_instruction(state); break;
	case STY_ABS: unimplemented_instruction(state); break;

	default:
		unimplemented_instruction(state); break;
	}
	return 0;
}

