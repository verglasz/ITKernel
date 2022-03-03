#pragma once

#include "types.h"

#define SYSCALL_EXIT			1
#define SYSCALL_GETBTNS			2
#define SYSCALL_GETSWS			3
#define SYSCALL_SCREEN_PRINT	4
#define SYSCALL_SERIAL_WRITE	5
#define SYSCALL_SCREEN_CLEAR    6

void sys_exit(usize status);
void sys_getbtns(u8 * result);
void sys_getsws(u8 * result);
void sys_screen_print(usize line, char * str, usize flags);
void sys_serial_write(u8 * buffer, usize len);
void sys_screen_clear(usize flags);

