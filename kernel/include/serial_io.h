#pragma once

#include "errorcodes.h"
#include "types.h"

#define ATTRIBUTE_PRINTF_1 \
    __attribute__((__format__(__printf__, 1, 2))) __attribute__((__nonnull__(1)))

ATTRIBUTE_PRINTF_1
int serial_printf(const char *fmt, ...);

usize serial_gets_s(char * buf, usize len);

