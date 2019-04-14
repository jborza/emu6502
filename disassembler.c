#include "types.h"
#include "opcodes.h"
#include "disassembler.h"
#include <stdio.h>

void disassemble_6502(byte* buffer, word pc) {
	printf("%04d  ", pc);
	byte* code = &buffer[pc];
	int bytes = 1;
	char op[128];
	switch (*code) {
	case 0x00: sprintf(op,"BRK"); break;
	case ORA_IMM: sprintf(op, "ORA #$%02x", code[1]); break;
	case ORA_ZP: sprintf(op, "ORA $%02x", code[1]); break;
	case ORA_ZPX: sprintf(op, "ORA $%02x,X", code[1]); break;
	case ORA_ABS: sprintf(op, "ORA $%02x%02x", code[2], code[1]); break;
	case ORA_ABSX: sprintf(op, "ORA $%02x%02x,X", code[2], code[1]); break;
	case ORA_ABSY:  sprintf(op, "ORA $%02x%02x,Y", code[2], code[1]); break;
	case ORA_INDX: sprintf(op, "ORA ($%02x,X)", code[1]); break;
	case ORA_INDY: sprintf(op, "ORA ($%02x),Y", code[1]); break;
	case LDA_IMM: sprintf(op, "LDA #$%02x", code[1]); break;
	case LDA_ZP: sprintf(op, "LDA $%02x", code[1]); break;
	case LDA_ZPX: sprintf(op, "LDA $%02x,X", code[1]); break;
	case LDA_ABS: sprintf(op, "LDA $%02x%02x", code[2], code[1]); break;
	case LDA_ABSX: sprintf(op, "LDA $%02x%02x,X", code[2], code[1]); break;
	case LDA_ABSY:  sprintf(op, "LDA $%02x%02x,Y", code[2], code[1]); break;
	case LDA_INDX: sprintf(op, "LDA ($%02x,X)", code[1]); break;
	case LDA_INDY: sprintf(op, "LDA ($%02x),Y", code[1]); break;

	case 0xEA: printf("NOP"); break;
	default:
		printf("UNKNOWN %02x", *code);
	}

	if (bytes > 1) {
		printf("%02x ", code[1]);
	}
	else {
		printf("   ");
	}
	if (bytes > 2) {
		printf("%02x ", code[2]);
	}
	else {
		printf("   ");
	}
	printf(op);

	printf("\n");
}