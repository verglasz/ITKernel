
#include "itklibc.h"

#define PRINTF_BUFSIZE 2048

ATTRIBUTE_PRINTF_1
isize serial_printf(const char *fmt, ...) {
    char buffer[PRINTF_BUFSIZE];
    va_list ap;
    isize written;

    va_start(ap, fmt);
    written =
        __builtin_vsnprintf(buffer, PRINTF_BUFSIZE, fmt, ap); // thank god for builtin vsnprintf
    va_end(ap);

    if (written < 0) return EFMT;
    if (written > PRINTF_BUFSIZE) return ENOSPACE;

    serial_write((u8 *)buffer, written);

    return written;
}

