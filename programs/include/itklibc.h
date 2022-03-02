
#pragma once
#include "types.h"

#define ATTRIBUTE_PRINTF_1 \
    __attribute__((__format__(__printf__, 1, 2))) __attribute__((__nonnull__(1)))

void syscall(usize code, ...);

__attribute__((noreturn)) void exit(usize status);
u8 getbtns();
u8 getsws();
void screen_print(usize line, char *str, usize flags);
void serial_write(u8 *buffer, usize len);


ATTRIBUTE_PRINTF_1
isize serial_printf(const char *fmt, ...);
// intentionally not const-qualified
isize sum(isize * vec, usize len);

