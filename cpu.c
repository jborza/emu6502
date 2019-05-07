#include "state.h"
#include "cpu.h"
#include "opcodes.h"
#include "memory.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

void* unimplemented_instruction(State6502* state) {
	printf("Error: unimplemented instruction\n");
	exit(1);
}

int is_negative(byte value) {
	return ((1 << 7) & value) != 0;
}

void set_z_flag(State6502 * state, byte value) {
	state->flags.z = value == 0;
}

void set_NV_flags(State6502 * state, byte value) {
	state->flags.n = is_negative(value);
	state->flags.v = ((1 << 6) & value) != 0;
}

void set_NZ_flags(State6502 * state, byte value) {
	set_z_flag(state, value);
	//N flag
	state->flags.n = is_negative(value);
}

byte flags_as_byte(State6502 * state) {
	byte flags_value = 0;
	flags_value |= state->flags.c << 0;
	flags_value |= state->flags.z << 1;
	flags_value |= state->flags.i << 2;
	flags_value |= state->flags.d << 3;
	//flag B is active with PHP
	flags_value |= 1 << 4;
	//unused bit is active with PHP
	flags_value |= 1 << 5;
	flags_value |= state->flags.v << 6;
	flags_value |= state->flags.n << 7;
	return flags_value;
}

void clear_flags(State6502 * state) {
	state->flags.b =
		state->flags.c =
		state->flags.d =
		state->flags.i =
		state->flags.n =
		state->flags.v =
		state->flags.z = 0;
	state->flags.pad = 1; //unused is supposed to be 1
}

void clear_state(State6502 * state) {
	state->a = 0;
	state->x = 0;
	state->y = 0;
	state->pc = 0;
	state->sp = 0xFF;
	clear_flags(state);
	state->running = 1;
}

void push_byte_to_stack(State6502 * state, byte value) {
	//stack located between $0100 to $01FF
	state->memory[STACK_HOME + state->sp--] = value;
}

void push_word_to_stack(State6502 * state, word value) {
	push_byte_to_stack(state, (value >> 8) & 0xFF);
	push_byte_to_stack(state, value & 0xFF);
}

byte pop_byte_from_stack(State6502 * state) {
	return state->memory[STACK_HOME + ++(state->sp)];
}

word pop_word_from_stack(State6502 * state) {
	byte low = pop_byte_from_stack(state);
	byte high = pop_byte_from_stack(state);
	return low + ((word)high << 8);
}

//bitwise or with accumulator
void ORA(State6502 * state, byte operand) {
	byte result = state->a | operand;
	set_NZ_flags(state, result);
	state->a = result;
}

//bitwise and with accumulator
void AND(State6502 * state, byte operand) {
	byte result = state->a & operand;
	set_NZ_flags(state, result);
	state->a = result;
}

//load accumulator
void LDA(State6502 * state, byte operand) {
	state->a = operand;
	set_NZ_flags(state, state->a);
}

void LDX(State6502 * state, byte operand) {
	state->x = operand;
	set_NZ_flags(state, state->x);
}

void LDY(State6502 * state, byte operand) {
	state->y = operand;
	set_NZ_flags(state, state->y);
}

void STA(State6502 * state, word address) {
	state->memory[address] = state->a;
}

void STX(State6502 * state, word address) {
	state->memory[address] = state->x;
}

void STY(State6502 * state, word address) {
	state->memory[address] = state->y;
}

void INC(State6502 * state, word address) {
	state->memory[address] += 1;
	set_NZ_flags(state, state->memory[address]);
}

void DEC(State6502 * state, word address) {
	state->memory[address] -= 1;
	set_NZ_flags(state, state->memory[address]);
}

void EOR(State6502 * state, byte operand) {
	state->a = state->a ^ operand;
	set_NZ_flags(state, state->a);
}

void JMP(State6502 * state, word address) {
	state->pc = address;
}

void SBC(State6502 * state, byte operand) {
	//subtract operand from A
	word operand_word = operand;
	//borrow the complement of carry flag - if the carry flag is 1, borrow 0 and vice versa
	word result_word = state->a - operand_word - !state->flags.c;
	byte result = result_word & 0xFF;

	// overflow flag if the the result doesn't fit into the signed byte range -128 to 127
	state->flags.v = ((state->a ^ operand) & 0x80) && ((state->a ^ result) & 0x80);

	state->a = result;
	state->flags.n = is_negative(state->a);
	state->flags.z = state->a == 0;
	state->flags.c = result_word <= 0xFF;
}

void ADC(State6502 * state, byte operand) {
	//add operand to A
	word result_word = operand + state->a + (state->flags.c ? 1 : 0);
	byte result = result_word & 0xFF;
	//set overflow flag if the result's sign would change - the result doesn't fit into a signed byte
	//there is overflow if the inputs do not have different signs and the input sign is different from the output sign 
	//meaning two numbers that have the same sign are added, and the result has a different sign.

	//overflow = <'a' and 'arg' have the same sign> & <the sign of 'a'and 'sum' differs> & <extract sign bit>
	state->flags.v = !((state->a ^ operand) & 0x80) && ((state->a ^ result) & 0x80);

	state->a = result;
	state->flags.n = is_negative(state->a);
	state->flags.z = state->a == 0;
	state->flags.c = result_word > 0xFF;
}

void BIT(State6502 * state, byte operand) {
	//BIT sets the Z flag as though the value in the address tested were ANDed with the accumulator. 
	//The N and V flags are set to match bits 7 and 6 respectively in the value stored at the tested address. 
	set_NV_flags(state, operand);
	state->flags.z = (state->a & operand) == 0;
}

void cmp_internal(State6502 * state, byte register_value, byte operand) {
	//set carry flag if A >= M
	state->flags.c = register_value >= operand;
	//set zero flag if A == M
	state->flags.z = register_value == operand;
	//set negative flag if A - M is negative
	state->flags.n = is_negative(register_value - operand);
}

void CMP(State6502 * state, byte operand) {
	cmp_internal(state, state->a, operand);
}

void CPX(State6502 * state, byte operand) {
	cmp_internal(state, state->x, operand);
}

void CPY(State6502 * state, byte operand) {
	cmp_internal(state, state->y, operand);
}

//Aritmetic Shift Left
byte asl(State6502 * state, byte operand) {
	byte result = operand << 1;
	state->flags.c = (operand & 0x80) == 0x80;
	set_NZ_flags(state, result);
	return result;
}

void ASL_A(State6502 * state) {
	state->a = asl(state, state->a);
}

void ASL_MEM(State6502 * state, word address) {
	byte operand = state->memory[address];
	state->memory[address] = operand;
	state->memory[address] = asl(state, operand);
}

byte lsr(State6502 * state, byte operand) {
	byte result = operand >> 1;
	state->flags.c = (operand & 0x01) != 0;
	set_NZ_flags(state, result);
	return result;
}

void LSR_A(State6502 * state) {
	state->a = lsr(state, state->a);
}

void LSR_MEM(State6502 * state, word address) {
	byte operand = state->memory[address];
	state->memory[address] = lsr(state, operand);
}

byte rol(State6502 * state, byte operand) {
	word result_word = (operand << 1) | state->flags.c;
	state->flags.c = result_word > 0xFF;
	byte result = result_word & 0xFF;
	set_NZ_flags(state, result);
	return result;
}

void ROL_A(State6502 * state) {
	state->a = rol(state, state->a);
}

void ROL_MEM(State6502 * state, word address) {
	byte operand = state->memory[address];
	state->memory[address] = rol(state, operand);
}

byte ror(State6502 * state, byte operand) {
	byte result = (operand >> 1) | (state->flags.c << 7);
	state->flags.c = (operand & 0x01) != 0;
	set_NZ_flags(state, result);
	return result;
}

void ROR_A(State6502 * state) {
	state->a = ror(state, state->a);
}

void ROR_MEM(State6502 * state, word address) {
	byte operand = state->memory[address];
	state->memory[address] = ror(state, operand);
}

void JSR(State6502 * state, word address) {
	//JSR pushes the address-1 of the next operation on to the stack before transferring program control to the following address.
	word address_to_push = state->pc - 1;
	push_byte_to_stack(state, (address_to_push >> 8 & 0xFF));
	push_byte_to_stack(state, address_to_push & 0xFF);
	state->pc = address;
}

void RTS_(State6502 * state) {
	word address = pop_word_from_stack(state);
	state->pc = address + 1;
}

void RTI_(State6502 * state) {
	//interrupt pushes PC first, then status register
	//RTI should pull status register and program counter from the stack
	byte value = pop_byte_from_stack(state);
	//we don't read the BRK flag
	value &= ~(1 << 4);
	//the bit 5 always comes in as true
	value |= 1 << 5;
	memset(&state->flags, value, sizeof(Flags));
	word address = pop_word_from_stack(state);
	state->pc = address;
}

void BEQ(State6502 * state) {
	word address = get_address_relative(state);
	if (state->flags.z)
		state->pc = address;
}

void BNE(State6502 * state) {
	word address = get_address_relative(state);
	if (!state->flags.z)
		state->pc = address;
}

void BCC(State6502 * state) {
	word address = get_address_relative(state);
	if (!state->flags.c)
		state->pc = address;
}

void BCS(State6502 * state) {
	word address = get_address_relative(state);
	if (state->flags.c)
		state->pc = address;
}

void BMI(State6502 * state) {
	word address = get_address_relative(state);
	if (state->flags.n)
		state->pc = address;
}

void BPL(State6502 * state) {
	word address = get_address_relative(state);
	if (!state->flags.n)
		state->pc = address;
}

void BVS(State6502 * state) {
	word address = get_address_relative(state);
	if (state->flags.v)
		state->pc = address;
}

void BVC(State6502 * state) {
	word address = get_address_relative(state);
	if (!state->flags.v)
		state->pc = address;
}

void PLA_(State6502 * state) {
	state->a = pop_byte_from_stack(state);
	set_NZ_flags(state, state->a);
}

void PLP_(State6502 * state) {
	byte value = pop_byte_from_stack(state);
	//we don't read the BRK flag
	value &= ~(1 << 4);
	//the bit 5 always comes in as true
	value |= 1 << 5;
	memset(&state->flags, value, sizeof(Flags));
}

void PHP_(State6502 * state) {
	byte flags_value = flags_as_byte(state);
	push_byte_to_stack(state, flags_value);
}

int emulate_6502_op(State6502 * state) {
	byte* opcode = &state->memory[state->pc++];
	switch (*opcode) {
	case ADC_IMM: ADC(state, fetch_byte(state)); break;
	case ADC_ZP:  ADC(state, get_byte_zero_page(state)); break;
	case ADC_ZPX: ADC(state, get_byte_zero_page_x(state)); break;
	case ADC_ABS: ADC(state, get_byte_absolute(state)); break;
	case ADC_ABSX: ADC(state, get_byte_absolute_x(state)); break;
	case ADC_ABSY: ADC(state, get_byte_absolute_y(state)); break;
	case ADC_INDX: ADC(state, get_byte_indirect_x(state)); break;
	case ADC_INDY: ADC(state, get_byte_indirect_y(state)); break;
	case AND_IMM: AND(state, fetch_byte(state)); break;
	case AND_ZP: AND(state, get_byte_zero_page(state)); break;
	case AND_ZPX: AND(state, get_byte_zero_page_x(state)); break;
	case AND_ABS: AND(state, get_byte_absolute(state)); break;
	case AND_ABSX: AND(state, get_byte_absolute_x(state)); break;
	case AND_ABSY: AND(state, get_byte_absolute_y(state)); break;
	case AND_INDX: AND(state, get_byte_indirect_x(state)); break;
	case AND_INDY: AND(state, get_byte_indirect_y(state)); break;
	case ASL_ACC: ASL_A(state); break;
	case ASL_ZP: ASL_MEM(state, get_address_zero_page(state)); break;
	case ASL_ZPX: ASL_MEM(state, get_address_zero_page_x(state)); break;
	case ASL_ABS: ASL_MEM(state, get_address_absolute(state)); break;
	case ASL_ABSX: ASL_MEM(state, get_address_absolute_x(state)); break;
	case BCC_REL: BCC(state); break;
	case BCS_REL: BCS(state); break;
	case BEQ_REL: BEQ(state); break;
	case BMI_REL: BMI(state); break;
	case BNE_REL: BNE(state); break;
	case BPL_REL: BPL(state); break;
	case BVC_REL: BVC(state); break;
	case BVS_REL: BVS(state); break;
	case BIT_ZP: BIT(state, get_byte_zero_page(state)); break;
	case BIT_ABS: BIT(state, get_byte_absolute(state)); break;
	case BRK: state->running = 0;
		state->flags.b = 1;
		break; //BRK
	case CLC: state->flags.c = 0; break;
	case CLD: state->flags.d = 0; break;
	case CLI: state->flags.i = 0; break;
	case CLV: state->flags.v = 0; break;
	case NOP: break; //NOP
	case PHA: push_byte_to_stack(state, state->a); break; //push accumulator to stack
	case PLA: PLA_(state); break; //pull accumulator from stack
	case PHP: PHP_(state); break; //push processor status
	case PLP: PLP_(state); break; //pull procesor status 
	case RTI: RTI_(state); break;
	case RTS: RTS_(state); break;
	case SEC: state->flags.c = 1; break;
	case SED: state->flags.d = 1; break;
	case SEI: state->flags.i = 1; break;
	case TAX: state->x = state->a; set_NZ_flags(state, state->x); break;
	case TXA: state->a = state->x; set_NZ_flags(state, state->a); break;
	case TAY: state->y = state->a; set_NZ_flags(state, state->y); break;
	case TYA: state->a = state->y; set_NZ_flags(state, state->a);  break;
	case TSX: state->x = state->sp; set_NZ_flags(state, state->x); break;
	case TXS: state->sp = state->x; break;
	case CMP_IMM: CMP(state, fetch_byte(state)); break; //TODO test
	case CMP_ZP: CMP(state, get_byte_zero_page(state)); break; //TODO test
	case CMP_ZPX: CMP(state, get_byte_zero_page_x(state)); break; //TODO test
	case CMP_ABS: CMP(state, get_byte_absolute(state)); break;
	case CMP_ABSX: CMP(state, get_byte_absolute_x(state)); break;//TODO test
	case CMP_ABSY: CMP(state, get_byte_absolute_y(state)); break;//TODO test
	case CMP_INDX: CMP(state, get_byte_indirect_x(state)); break;//TODO test
	case CMP_INDY: CMP(state, get_byte_indirect_y(state)); break;//TODO test
	case CPX_IMM: CPX(state, fetch_byte(state)); break;//TODO test
	case CPX_ZP: CPX(state, get_byte_zero_page(state)); break;//TODO test
	case CPX_ABS: CPX(state, get_byte_absolute(state)); break;//TODO test
	case CPY_IMM: CPY(state, fetch_byte(state)); break;//TODO test
	case CPY_ZP: CPY(state, get_byte_zero_page(state)); break;//TODO test
	case CPY_ABS: CPY(state, get_byte_absolute(state)); break;//TODO test
	case DEC_ZP: DEC(state, get_address_zero_page(state)); break;
	case DEC_ZPX: DEC(state, get_address_zero_page_x(state)); break;
	case DEC_ABS: DEC(state, get_address_absolute(state)); break;
	case DEC_ABSX: DEC(state, get_address_absolute_x(state)); break;
	case DEX: state->x -= 1; set_NZ_flags(state, state->x); break;
	case DEY: state->y -= 1; set_NZ_flags(state, state->y); break;
	case INX: state->x += 1; set_NZ_flags(state, state->x); break;
	case INY: state->y += 1; set_NZ_flags(state, state->y); break;
	case EOR_IMM: EOR(state, fetch_byte(state)); break;
	case EOR_ZP: EOR(state, get_byte_zero_page(state));	break;
	case EOR_ZPX: EOR(state, get_byte_zero_page_x(state)); break;
	case EOR_ABS: EOR(state, get_byte_absolute(state)); break;
	case EOR_ABSX: EOR(state, get_byte_absolute_x(state)); break;
	case EOR_ABSY: EOR(state, get_byte_absolute_y(state)); break;
	case EOR_INDX: EOR(state, get_byte_indirect_x(state)); break;
	case EOR_INDY: EOR(state, get_byte_indirect_y(state)); break;
	case INC_ZP: INC(state, get_address_zero_page(state)); break;
	case INC_ZPX: INC(state, get_address_zero_page_x(state)); break;
	case INC_ABS: INC(state, get_address_absolute(state)); break;
	case INC_ABSX: INC(state, get_address_absolute_x(state)); break;
	case JMP_ABS: JMP(state, get_address_absolute(state)); break;
	case JMP_IND: JMP(state, get_address_indirect_jmp(state)); break;
	case JSR_ABS: JSR(state, get_address_absolute(state)); break;
	case LDA_IMM: LDA(state, fetch_byte(state)); break;
	case LDA_ZP: LDA(state, get_byte_zero_page(state));	break;
	case LDA_ZPX: LDA(state, get_byte_zero_page_x(state)); break;
	case LDA_ABS: LDA(state, get_byte_absolute(state)); break;
	case LDA_ABSX: LDA(state, get_byte_absolute_x(state)); break;
	case LDA_ABSY: LDA(state, get_byte_absolute_y(state)); break;
	case LDA_INDX: LDA(state, get_byte_indirect_x(state)); break;
	case LDA_INDY: LDA(state, get_byte_indirect_y(state)); break;
	case LDX_IMM: LDX(state, fetch_byte(state)); break;
	case LDX_ZP: LDX(state, get_byte_zero_page(state)); break;
	case LDX_ZPY: LDX(state, get_byte_zero_page_y(state)); break;
	case LDX_ABS: LDX(state, get_byte_absolute(state)); break;
	case LDX_ABSY: LDX(state, get_byte_absolute_y(state)); break;
	case LDY_IMM: LDY(state, fetch_byte(state)); break;
	case LDY_ZP: LDY(state, get_byte_zero_page(state)); break;
	case LDY_ZPX: LDY(state, get_byte_zero_page_x(state)); break;
	case LDY_ABS: LDY(state, get_byte_absolute(state)); break;
	case LDY_ABSX: LDY(state, get_byte_absolute_x(state)); break;
	case LSR_ACC: LSR_A(state);  break;
	case LSR_ZP: LSR_MEM(state, get_address_zero_page(state)); break;
	case LSR_ZPX: LSR_MEM(state, get_address_zero_page_x(state)); break;
	case LSR_ABS: LSR_MEM(state, get_address_absolute(state)); break;
	case LSR_ABSX: LSR_MEM(state, get_address_absolute_x(state)); break;
	case ORA_IMM: ORA(state, fetch_byte(state)); break;
	case ORA_ZP: ORA(state, get_byte_zero_page(state));	break;
	case ORA_ZPX: ORA(state, get_byte_zero_page_x(state)); break;
	case ORA_ABS: ORA(state, get_byte_absolute(state)); break;
	case ORA_ABSX: ORA(state, get_byte_absolute_x(state)); break;
	case ORA_ABSY: ORA(state, get_byte_absolute_y(state)); break;
	case ORA_INDX: ORA(state, get_byte_indirect_x(state)); break;
	case ORA_INDY: ORA(state, get_byte_indirect_y(state)); break;
	case ROL_ACC: ROL_A(state); break;
	case ROL_ZP: ROL_MEM(state, get_address_zero_page(state)); break;
	case ROL_ZPX: ROL_MEM(state, get_address_zero_page_x(state)); break;
	case ROL_ABS: ROL_MEM(state, get_address_absolute(state)); break;
	case ROL_ABSX: ROL_MEM(state, get_address_absolute_x(state)); break;
	case ROR_ACC: ROR_A(state); break;
	case ROR_ZP: ROR_MEM(state, get_address_zero_page(state)); break;
	case ROR_ZPX: ROR_MEM(state, get_address_zero_page_x(state)); break;
	case ROR_ABS: ROR_MEM(state, get_address_absolute(state)); break;
	case ROR_ABSX: ROR_MEM(state, get_address_absolute_x(state)); break;
	case SBC_IMM: SBC(state, fetch_byte(state)); break;
	case SBC_ZP: SBC(state, get_byte_zero_page(state)); break;
	case SBC_ZPX: SBC(state, get_byte_zero_page_x(state)); break;
	case SBC_ABS: SBC(state, get_byte_absolute(state)); break;
	case SBC_ABSX: SBC(state, get_byte_absolute_x(state)); break;
	case SBC_ABSY: SBC(state, get_byte_absolute_y(state)); break;
	case SBC_INDX: SBC(state, get_byte_indirect_x(state)); break;
	case SBC_INDY: SBC(state, get_byte_indirect_y(state)); break;
	case STA_ZP: STA(state, get_address_zero_page(state)); break;
	case STA_ZPX: STA(state, get_address_zero_page_x(state)); break;
	case STA_ABS: STA(state, get_address_absolute(state)); break;
	case STA_ABSX: STA(state, get_address_absolute_x(state)); break;
	case STA_ABSY: STA(state, get_address_absolute_y(state)); break;
	case STA_INDX: STA(state, get_address_indirect_x(state)); break;
	case STA_INDY: STA(state, get_address_indirect_y(state)); break;
	case STX_ZP: STX(state, get_address_zero_page(state)); break;
	case STX_ZPY: STX(state, get_address_zero_page_y(state)); break;
	case STX_ABS: STX(state, get_address_absolute(state)); break;
	case STY_ZP: STY(state, get_address_zero_page(state));  break;
	case STY_ZPX: STY(state, get_address_zero_page_x(state)); break;
	case STY_ABS:  STY(state, get_address_absolute(state)); break;

	default:
		unimplemented_instruction(state); break;
	}
	return 0;
}