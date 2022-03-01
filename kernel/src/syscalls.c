
#include "syscalls.h"

#include "gpio.h"
#include "kernel.h"
#include "uart.h"
/* #include "farcall.h" */

void sys_exit(usize status) {
    set_kernel_mode();
    eret_longjmp(&kernel_ctx, status);
}

void sys_getbtns(u8 *result) {
    *result = getbtns();
}

void sys_getsws(u8 *result) {
    *result = getsws();
}

void sys_screen_print(usize line, char *str, usize flags) {}

void sys_serial_write(u8 *buffer, usize len) {
    uart_write_n(buffer, len);
}
