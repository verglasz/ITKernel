
#include "serial_io.h"

#include "uart.h"

#include <stdio.h>

#define PRINTF_BUFSIZE (1 * 1024)

FILE *const stdout;

/* printf to serial output
 * (attribute to get the compiler to check that args are consistent with format string)
 * return number of bytes written
 */
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

    uart_write_n((u8 *)buffer, written);

    return written;
}

/* Read string from serial input into buf,
 * stopping when a newline or null byte is encountered
 * or when len-1 bytes have been read;
 * then null-terminate the string.
 * Return number of bytes read, excluding null byte (max len-1)
 */
usize serial_gets_s(char *buf, usize len) {
    isize read = 0;
    while ((usize)read + 1 < len) {
        buf[read] = uart_direct_read_byte();
        if (buf[read] == '\0' || buf[read] == '\n') break;
        read++;
    }
    buf[read] = '\0';
    return read;
}
