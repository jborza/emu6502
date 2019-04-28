#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "state.h"
#include "cpu.h"
#include "disassembler.h"
#include "opcodes.h"
#include <windows.h> 
#include <conio.h>

#define MEMORY_SIZE 0xFFFF
#define PRG_START 0x0600

#define DISP_WIDTH 32
#define DISP_HEIGHT 32

#define FRAME_RIGHT 35

int glob_file_size;
int last_key;

HANDLE hStdOut;

void init_console() {
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SMALL_RECT windowSize = { 0, 0,  80, 40 };

	/* Set the window size */
	SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);
}

void con_set_color(unsigned int fg_color, unsigned int bg_color)
{
	if (hStdOut == INVALID_HANDLE_VALUE) {
		return;
	}

	SetConsoleTextAttribute(hStdOut, (bg_color << 4) | fg_color);
}

void con_set_xy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void print_mem(State6502 * state) {
	SMALL_RECT console_write_area = { 1, 1, DISP_WIDTH + 1, DISP_HEIGHT + 1 };
	CHAR_INFO console_buffer[DISP_WIDTH * DISP_HEIGHT];
	COORD character_position = { 0, 0 };
	COORD console_buffer_size = { DISP_WIDTH, DISP_HEIGHT };
	for (int y = 0; y < DISP_WIDTH; y++)
		for (int x = 0; x < DISP_HEIGHT; x++) {
			console_buffer[x + y * DISP_WIDTH].Char.AsciiChar = 0;
			byte color_bg = state->memory[0x200 + x + y * DISP_WIDTH];
			console_buffer[x + y * DISP_WIDTH].Attributes = color_bg << 4;
		}
	WriteConsoleOutputA(hStdOut, console_buffer, console_buffer_size, character_position, &console_write_area);
}

void print_state_debug(State6502 * state) {
	con_set_xy(FRAME_RIGHT, 0);
	printf("A=$%02X X=$%02X Y=$%02X", state->a, state->x, state->y);
	con_set_xy(FRAME_RIGHT, 1);
	printf("SP=$%02X PC=$%04X", state->sp, state->pc);
	con_set_xy(FRAME_RIGHT, 3);
	printf("NV-BDIZC");
	con_set_xy(FRAME_RIGHT, 4);
	printf("%d%d%d%d%d%d%d%d", state->flags.n, state->flags.v, state->flags.pad, state->flags.b, state->flags.d, state->flags.i, state->flags.z, state->flags.c);
}

void print_stack(State6502 * state) {
	con_set_xy(FRAME_RIGHT, 16);
	for (int i = state->sp + 1; i <= 0xFF; i++)
		printf("%02x ", state->memory[STACK_HOME + i]);
}

byte * read_bin() {
	FILE* file = fopen("bins\\snake_fast.bin", "rb");
	if (!file) {
		int err = errno;
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	glob_file_size = ftell(file);
	rewind(file);

	byte* buffer = malloc(glob_file_size);
	int read = fread(buffer, sizeof(byte), glob_file_size, file);
	fclose(file);
	return buffer;
}

void check_keys() {
	int keys[] = { 'W', 'S', 'A', 'D' };
	for (int i = 0; i < sizeof(keys); i++)
		if (GetAsyncKeyState(keys[i])) {
			last_key = keys[i];
		}
}

void print_frame() {
	con_set_color(0x08, 0x00);
	//horizontal
	con_set_xy(0, 0);
	_putch('/');
	int y = 0;
	for (int x = 1; x <= 32; x++) {
		con_set_xy(x, y);
		_putch('-');
	}
	_putch('\\');
	y = 33;
	con_set_xy(0, y);
	_putch('\\');
	for (int x = 1; x < 32; x++) {
		con_set_xy(x, y);
		_putch('-');
	}
	_putch('/');
	//vertical frame
	int x = 0;
	for (int y = 1; y <= 32; y++) {
		con_set_xy(x, y);
		_putch('|');
	}
	x = 33;
	for (int y = 1; y <= 32; y++) {
		con_set_xy(x, y);
		_putch('|');
	}
}

int main(int argc, char* argv[]) {
	State6502 state;
	clear_state(&state);
	state.memory = malloc(MEMORY_SIZE);
	memset(state.memory, 0, MEMORY_SIZE);
	//load a binary
	byte* bin = read_bin();

	memcpy(state.memory + PRG_START, bin, glob_file_size);

	state.pc = PRG_START;
	//white - 0x0F
	init_console();
	print_frame();
	//update screen every N ticks
	do
	{
		con_set_xy(FRAME_RIGHT, 8);
		printf("                                        ");
		con_set_xy(FRAME_RIGHT, 8);
		con_set_color(0x0F, 0x00); //white FG, black BG

		disassemble_6502(state.memory, state.pc);
		for (int i = 0; i < 1; i++) {
			emulate_6502_op(&state);
			check_keys();
			state.memory[0xFF] = last_key & 0xFF;
		}
		print_mem(&state);
		con_set_color(0x0F, 0x00); //white FG, black BG
		print_state_debug(&state);
		//print_stack(&state);
		state.memory[0xfe] = rand() & 0xFF;
	} while (state.flags.b != 1);
}