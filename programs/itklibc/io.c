
#include "itklibc.h"
#include <stdarg.h>

#ifndef NOIO

#include <stdio.h>


#define PRINTF_BUFSIZE 2048

FILE * const stdout;

ATTRIBUTE_PRINTF_(1)
isize serial_printf(const char *fmt, ...) {
    char buffer[PRINTF_BUFSIZE];
    isize written;

    va_list ap;
    va_start(ap, fmt);
    written =
        __builtin_vsnprintf(buffer, PRINTF_BUFSIZE, fmt, ap); // thank god for builtin vsnprintf
    va_end(ap);

    if (written < 0) return -1;
    if (written > PRINTF_BUFSIZE) return -2;

    serial_write((u8 *)buffer, written);

    return written;
}

#else  // NOIO def'd

ATTRIBUTE_PRINTF_(1)
isize serial_printf(const char *fmt, ...) {
	return -1;
}


#endif

