#include "types.h"
#include "opcodes.h"
#include "disassembler.h"
#include <stdio.h>

//returns the length of the disassembled line

char* disassemble_6502_to_string(byte* buffer, word pc) {
	static char dasm_buffer[64];
	byte* code = &buffer[pc];
	int bytes = 1;
	char op[32] = "";
	switch (*code) {
		case ADC_IMM: sprintf(op, "ADC #$%02X", code[1]); bytes = 2; break;
		case ADC_ZP: sprintf(op, "ADC $%02X", code[1]); bytes = 2; break;
		case ADC_ZPX: sprintf(op, "ADC $%02X,X", code[1]); bytes = 2; break;
		case ADC_ABS: sprintf(op, "ADC $%02X%02X", code[2], code[1]); bytes = 3; break;
		case ADC_ABSX: sprintf(op, "ADC $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case ADC_ABSY: sprintf(op, "ADC $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case ADC_INDX: sprintf(op, "ADC ($%02X,X)", code[1]); bytes = 2; break;
		case ADC_INDY: sprintf(op, "ADC ($%02X),Y", code[1]); bytes = 2; break;
		case AND_IMM: sprintf(op, "AND #$%02X", code[1]); bytes = 2; break;
		case AND_ZP: sprintf(op, "AND $%02X", code[1]); bytes = 2; break;
		case AND_ZPX: sprintf(op, "AND $%02X,X", code[1]); bytes = 2; break;
		case AND_ABS: sprintf(op, "AND $%02X%02X", code[2], code[1]); bytes = 3; break;
		case AND_ABSX: sprintf(op, "AND $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case AND_ABSY: sprintf(op, "AND $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case AND_INDX: sprintf(op, "AND ($%02X,X)", code[1]); bytes = 2; break;
		case AND_INDY: sprintf(op, "AND ($%02X),Y", code[1]); bytes = 2; break;
		case ASL_ACC: sprintf(op, "ASL A"); break;
		case ASL_ZP: sprintf(op, "ASL $%02X", code[1]); bytes = 2; break;
		case ASL_ZPX: sprintf(op, "ASL $%02X,X", code[1]); bytes = 2; break;
		case ASL_ABS: sprintf(op, "ASL $%02X%02X", code[2], code[1]); bytes = 3; break;
		case ASL_ABSX: sprintf(op, "ASL $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case BCC_REL: sprintf(op, "BCC $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BCS_REL: sprintf(op, "BCS $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BEQ_REL: sprintf(op, "BEQ $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BMI_REL: sprintf(op, "BMI $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BNE_REL: sprintf(op, "BNE $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BPL_REL: sprintf(op, "BPL $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BVC_REL: sprintf(op, "BVC $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BVS_REL: sprintf(op, "BVS $%04X", pc + 2 + (int8_t)code[1]); bytes = 2; break;
		case BIT_ZP: sprintf(op, "BIT $%02X", code[1]); bytes = 2; break;
		case BIT_ABS: sprintf(op, "BIT $%02X%02X", code[2], code[1]); bytes = 3; break;
		case BRK: sprintf(op, "BRK"); break;
		case CLC: sprintf(op, "CLC"); break;
		case CLD: sprintf(op, "CLD"); break;
		case CLI: sprintf(op, "CLI"); break;
		case CLV: sprintf(op, "CLV"); break;
		case NOP: sprintf(op, "NOP"); break;
		case PHA: sprintf(op, "PHA"); break;
		case PLA: sprintf(op, "PLA"); break;
		case PHP: sprintf(op, "PHP"); break;
		case PLP: sprintf(op, "PLP"); break;
		case RTI: sprintf(op, "RTI"); break;
		case RTS: sprintf(op, "RTS"); break;
		case SEC: sprintf(op, "SEC"); break;
		case SED: sprintf(op, "SED"); break;
		case SEI: sprintf(op, "SEI"); break;
		case TAX: sprintf(op, "TAX"); break;
		case TXA: sprintf(op, "TXA"); break;
		case TAY: sprintf(op, "TAY"); break;
		case TYA: sprintf(op, "TYA"); break;
		case TSX: sprintf(op, "TSX"); break;
		case TXS: sprintf(op, "TXS"); break;
		case CMP_IMM: sprintf(op, "CMP #$%02X", code[1]); bytes = 2; break;
		case CMP_ZP: sprintf(op, "CMP $%02X", code[1]); bytes = 2; break;
		case CMP_ZPX: sprintf(op, "CMP $%02X,X", code[1]); bytes = 2; break;
		case CMP_ABS: sprintf(op, "CMP $%02X%02X", code[2], code[1]); bytes = 3; break;
		case CMP_ABSX: sprintf(op, "CMP $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case CMP_ABSY: sprintf(op, "CMP $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case CMP_INDX: sprintf(op, "CMP ($%02X,X)", code[1]); bytes = 2; break;
		case CMP_INDY: sprintf(op, "CMP ($%02X),Y", code[1]); bytes = 2; break;
		case CPX_IMM: sprintf(op, "CPX #$%02X", code[1]); bytes = 2; break;
		case CPX_ZP: sprintf(op, "CPX $%02X", code[1]); bytes = 2; break;
		case CPX_ABS: sprintf(op, "CPX $%02X%02X", code[2], code[1]); bytes = 3; break;
		case CPY_IMM: sprintf(op, "CPY #$%02X", code[1]); bytes = 2; break;
		case CPY_ZP: sprintf(op, "CPY $%02X", code[1]); bytes = 2; break;
		case CPY_ABS: sprintf(op, "CPY $%02X%02X", code[2], code[1]); bytes = 3; break;
		case DEC_ZP: sprintf(op, "DEC $%02X", code[1]); bytes = 2; break;
		case DEC_ZPX: sprintf(op, "DEC $%02X,X", code[1]); bytes = 2; break;
		case DEC_ABS: sprintf(op, "DEC $%02X%02X", code[2], code[1]); bytes = 3; break;
		case DEC_ABSX: sprintf(op, "DEC $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case DEX: sprintf(op, "DEX"); break;
		case DEY: sprintf(op, "DEY"); break;
		case INX: sprintf(op, "INX"); break;
		case INY: sprintf(op, "INY"); break;
		case EOR_IMM: sprintf(op, "EOR #$%02X", code[1]); bytes = 2; break;
		case EOR_ZP: sprintf(op, "EOR $%02X", code[1]); bytes = 2; break;
		case EOR_ZPX: sprintf(op, "EOR $%02X,X", code[1]); bytes = 2; break;
		case EOR_ABS: sprintf(op, "EOR $%02X%02X", code[2], code[1]); bytes = 3; break;
		case EOR_ABSX: sprintf(op, "EOR $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case EOR_ABSY: sprintf(op, "EOR $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case EOR_INDX: sprintf(op, "EOR ($%02X,X)", code[1]); bytes = 2; break;
		case EOR_INDY: sprintf(op, "EOR ($%02X),Y", code[1]); bytes = 2; break;
		case INC_ZP: sprintf(op, "INC $%02X", code[1]); bytes = 2; break;
		case INC_ZPX: sprintf(op, "INC $%02X,X", code[1]); bytes = 2; break;
		case INC_ABS: sprintf(op, "INC $%02X%02X", code[2], code[1]); bytes = 3; break;
		case INC_ABSX: sprintf(op, "INC $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case JMP_ABS: sprintf(op, "JMP $%02X%02X", code[2], code[1]); bytes = 3; break;
		case JMP_IND: sprintf(op, "JMP ($%02X%02X)", code[2], code[1]); bytes = 3; break;
		case JSR_ABS: sprintf(op, "JSR $%02X%02X", code[2], code[1]); bytes = 3; break;
		case LDA_IMM: sprintf(op, "LDA #$%02X", code[1]); bytes = 2; break;
		case LDA_ZP: sprintf(op, "LDA $%02X", code[1]); bytes = 2; break;
		case LDA_ZPX: sprintf(op, "LDA $%02X,X", code[1]); bytes = 2; break;
		case LDA_ABS: sprintf(op, "LDA $%02X%02X", code[2], code[1]); bytes = 3; break;
		case LDA_ABSX: sprintf(op, "LDA $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case LDA_ABSY: sprintf(op, "LDA $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case LDA_INDX: sprintf(op, "LDA ($%02X,X)", code[1]); bytes = 2; break;
		case LDA_INDY: sprintf(op, "LDA ($%02X),Y", code[1]); bytes = 2; break;
		case LDX_IMM: sprintf(op, "LDX #$%02X", code[1]); bytes = 2; break;
		case LDX_ZP: sprintf(op, "LDX $%02X", code[1]); bytes = 2; break;
		case LDX_ZPY: sprintf(op, "LDX $%02X,Y", code[1]); bytes = 2; break;
		case LDX_ABS: sprintf(op, "LDX $%02X%02X", code[2], code[1]); bytes = 3; break;
		case LDX_ABSY: sprintf(op, "LDX $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case LDY_IMM: sprintf(op, "LDY #$%02X", code[1]); bytes = 2; break;
		case LDY_ZP: sprintf(op, "LDY $%02X", code[1]); bytes = 2; break;
		case LDY_ZPX: sprintf(op, "LDY $%02X,X", code[1]); bytes = 2; break;
		case LDY_ABS: sprintf(op, "LDY $%02X%02X", code[2], code[1]); bytes = 3; break;
		case LDY_ABSX: sprintf(op, "LDY $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case LSR_ACC: sprintf(op, "LSR A"); break;
		case LSR_ZP: sprintf(op, "LSR $%02X", code[1]); bytes = 2; break;
		case LSR_ZPX: sprintf(op, "LSR $%02X,X", code[1]); bytes = 2; break;
		case LSR_ABS: sprintf(op, "LSR $%02X%02X", code[2], code[1]); bytes = 3; break;
		case LSR_ABSX: sprintf(op, "LSR $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case ORA_IMM: sprintf(op, "ORA #$%02X", code[1]); bytes = 2; break;
		case ORA_ZP: sprintf(op, "ORA $%02X", code[1]); bytes = 2; break;
		case ORA_ZPX: sprintf(op, "ORA $%02X,X", code[1]); bytes = 2; break;
		case ORA_ABS: sprintf(op, "ORA $%02X%02X", code[2], code[1]); bytes = 3; break;
		case ORA_ABSX: sprintf(op, "ORA $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case ORA_ABSY: sprintf(op, "ORA $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case ORA_INDX: sprintf(op, "ORA ($%02X,X)", code[1]); bytes = 2; break;
		case ORA_INDY: sprintf(op, "ORA ($%02X),Y", code[1]); bytes = 2; break;
		case ROL_ACC: sprintf(op, "ROL A"); break;
		case ROL_ZP: sprintf(op, "ROL $%02X", code[1]); bytes = 2; break;
		case ROL_ZPX: sprintf(op, "ROL $%02X,X", code[1]); bytes = 2; break;
		case ROL_ABS: sprintf(op, "ROL $%02X%02X", code[2], code[1]); bytes = 3; break;
		case ROL_ABSX: sprintf(op, "ROL $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case ROR_ACC: sprintf(op, "ROR A"); break;
		case ROR_ZP: sprintf(op, "ROR $%02X", code[1]); bytes = 2; break;
		case ROR_ZPX: sprintf(op, "ROR $%02X,X", code[1]); bytes = 2; break;
		case ROR_ABS: sprintf(op, "ROR $%02X%02X", code[2], code[1]); bytes = 3; break;
		case ROR_ABSX: sprintf(op, "ROR $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case SBC_IMM: sprintf(op, "SBC #$%02X", code[1]); bytes = 2; break;
		case SBC_ZP: sprintf(op, "SBC $%02X", code[1]); bytes = 2; break;
		case SBC_ZPX: sprintf(op, "SBC $%02X,X", code[1]); bytes = 2; break;
		case SBC_ABS: sprintf(op, "SBC $%02X%02X", code[2], code[1]); bytes = 3; break;
		case SBC_ABSX: sprintf(op, "SBC $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case SBC_ABSY: sprintf(op, "SBC $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case SBC_INDX: sprintf(op, "SBC ($%02X,X)", code[1]); bytes = 2; break;
		case SBC_INDY: sprintf(op, "SBC ($%02X),Y", code[1]); bytes = 2; break;
		case STA_ZP: sprintf(op, "STA $%02X", code[1]); bytes = 2; break;
		case STA_ZPX: sprintf(op, "STA $%02X,X", code[1]); bytes = 2; break;
		case STA_ABS: sprintf(op, "STA $%02X%02X", code[2], code[1]); bytes = 3; break;
		case STA_ABSX: sprintf(op, "STA $%02X%02X,X", code[2], code[1]); bytes = 3; break;
		case STA_ABSY: sprintf(op, "STA $%02X%02X,Y", code[2], code[1]); bytes = 3; break;
		case STA_INDX: sprintf(op, "STA ($%02X,X)", code[1]); bytes = 2; break;
		case STA_INDY: sprintf(op, "STA ($%02X),Y", code[1]); bytes = 2; break;
		case STX_ZP: sprintf(op, "STX $%02X", code[1]); bytes = 2; break;
		case STX_ZPY: sprintf(op, "STX $%02X,Y", code[1]); bytes = 2; break;
		case STX_ABS: sprintf(op, "STX $%02X%02X", code[2], code[1]); bytes = 3; break;
		case STY_ZP: sprintf(op, "STY $%02X", code[1]); bytes = 2; break;
		case STY_ZPX: sprintf(op, "STY $%02X,X", code[1]); bytes = 2; break;
		case STY_ABS: sprintf(op, "STY $%02X%02X", code[2], code[1]); bytes = 3; break;


		default:
			sprintf(op, "UNKNOWN %02x", *code);
	}

	char arg1[5];
	if (bytes > 1) {
		/*if (op == BCC_REL || op == BEQ_REL || op == BMI_REL || op == BPL_REL || op == BVS_REL || op == BCS_REL || op == BNE_REL || op == BVC_REL) {
			sprintf(arg1, "%04X", code[1]);
		}*/
		sprintf(arg1,"%02X", code[1]);
	}
	else {
		sprintf(arg1,"  ");
	}

	char arg2[3];
	if (bytes > 2) {
		sprintf(arg2,"%02X", code[2]);
	}
	else {
		sprintf(arg2, "  ");
	}

	sprintf(dasm_buffer, "%04X  %02X %s %s  %s", pc, *code, arg1, arg2, op);
	return dasm_buffer;
}

void disassemble_6502(byte* buffer, word pc) {

	printf("%s", disassemble_6502_to_string(buffer, pc));
}