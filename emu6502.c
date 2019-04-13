// emu6502.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include "state.h"
#include "cpu.h"

int main()
{
    printf("Hello World!\n"); 
	State6502 state;
	emulate_6502_op(&state);
}