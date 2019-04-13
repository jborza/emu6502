#pragma once
#include "types.h"
#include "opcodes.h"

void disassemble_6502(byte* buffer, word pc) {
	printf("%04d  ", pc);
	byte* code = &buffer[pc];
	switch (*code) {
	case 0x00: printf("BRK"); break;
	case ORA_IND_X: printf("ORA ($%02x,X)", code[1]); break;
	case ORA_IND_Y: printf("ORA ($%02x),Y", code[1]); break;
	case ORA_ZP: printf("ORA $%02x", code[1]); break;
	case ORA_IMM: printf("ORA #$%02x", code[1]); break;
	case ORA_ZP_X: printf("ORA $%02x,X", code[1]); break;
	case ORA_ABS: printf("ORA $%02x%02x", code[2], code[1]); break;
	case ORA_ABS_X: printf("ORA $%02x%02x,X", code[2], code[1]); break;
	case ORA_ABS_Y:  printf("ORA $%02x%02x,Y", code[2], code[1]); break;
	case LDA_IMM: printf("LDA #$%02x", code[1]); break;

	case 0xEA: printf("NOP"); break;
	default:
		printf("UNKNOWN %02x", *code);
	}
	printf("\n");
}