#include "state.h"

void print_memory(State6502* state, word offset);
void print_state(State6502* state);
State6502 create_blank_state();
void test_cleanup(State6502* state);
void test_step(State6502* state);
void test_step_until_break(State6502* state);
void assertA(State6502* state, byte expected);
void assertX(State6502* state, byte expected);
void assertY(State6502* state, byte expected);
void assert_sp(State6502* state, byte expected);
void assert_pc(State6502* state, word expected);
void assert_memory(State6502* state, word address, byte expected);
void assert_flag_n(State6502* state, byte expected);
void assert_flag_z(State6502* state, byte expected);
void assert_flag_c(State6502* state, byte expected);
void assert_flag_i(State6502* state, byte expected);
void assert_flag_d(State6502* state, byte expected);
void assert_flag_v(State6502* state, byte expected);
void assert_flag_b(State6502* state, byte expected);