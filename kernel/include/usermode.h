#pragma once

#define USER_DATA_START 0x7F002000u
#define USER_DATA_END 0x7F005000u
#define USER_PROG_START 0x7F005000u
#define USER_PROG_END 0x7F008000u

/* linker defs for user programs
  kuseg_data_mem       (w!x) : ORIGIN = 0x7F002000, LENGTH = 0x3000
  kuseg_program_mem    (rx)  : ORIGIN = 0x7F005000, LENGTH = 0x3000
*/

__attribute__((noreturn))
void usermode_jump(void (*jump_target)(void), usize * stack_pointer);

