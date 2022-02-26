
#include "uart.h"

#include <stdio.h>

#define PRINTF_BUFSIZE 1 * 1024
#define ATTRIBUTE_PRINTF_1 \
    __attribute__((__format__(__printf__, 1, 2))) __attribute__((__nonnull__(1)))

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

    uart_write_n(buffer, written);

    return written;
}
