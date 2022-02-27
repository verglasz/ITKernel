#pragma once

#include "errorcodes.h"

#define ATTRIBUTE_PRINTF_1 \
    __attribute__((__format__(__printf__, 1, 2))) __attribute__((__nonnull__(1)))

ATTRIBUTE_PRINTF_1
int serial_printf(const char *fmt, ...);

