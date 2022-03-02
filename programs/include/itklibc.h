
#pragma once
#include "types.h"


void syscall(usize code, ...);

__attribute__((noreturn)) void exit(usize status);
u8 getbtns();
u8 getsws();
void screen_print(usize line, char *str, usize flags);
void serial_write(u8 *buffer, usize len);

// intentionally not const-qualified
isize sum(isize * vec, usize len);

