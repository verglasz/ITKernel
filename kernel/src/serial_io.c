
#include "serial_io.h"

#include "uart.h"

#include <stdio.h>

#define PRINTF_BUFSIZE (1 * 1024)

FILE *const stdout;

/* print to serial output
 * (attribute to get the compiler to check that args are consistent with format string)
 */
ATTRIBUTE_PRINTF_1
int serial_printf(const char *fmt, ...) {
    char buffer[PRINTF_BUFSIZE];
    va_list ap;
    int written;

    va_start(ap, fmt);
    written =
        __builtin_vsnprintf(buffer, PRINTF_BUFSIZE, fmt, ap); // thank god for builtin vsnprintf
    va_end(ap);

    if (written < 0) return EFMT;
    if (written > PRINTF_BUFSIZE) return ENOSPACE;

    uart_write_n((u8 *)buffer, written);

    return written;
}
