
#include "syscalls.h"

#include "display.h"
#include "gpio.h"
#include "kernel.h"
#include "serial_io.h"
#include "uart.h"

void sys_exit(usize status) {
    set_kernel_mode();
    serial_printf("in syscall exit %x\n", status);
    eret_longjmp(&kernel_ctx, status);
}

void sys_getbtns(u8 *result) {
    *result = getbtns();
}

void sys_getsws(u8 *result) {
    *result = getsws();
}

void sys_screen_print(usize line, char *str, usize flags) {
    serial_printf("in syscall screen_print %x %x %x\n", line, str, flags);
    if (line > 3) { // Make sure that the line is less than 4
        return;
    }
    u8 y = (line * 8);
    u8 x = 0;
    char blank[] = "                ";
    display_addstring(x, y, blank, (flags & 0x1));
    int res = display_addstring(x, y, str, (flags & 0x1));
    if (res == 0) display_update();
}

void sys_serial_write(u8 *buffer, usize len) {
    serial_printf("in syscall serial_write %x %x \n", buffer, len);
    uart_write_n(buffer, len);
}

void sys_screen_clear(usize flag) {
    serial_printf("in syscall screen_clear %x \n", flag);
    if (flag & 0x1) {
        display_clear();
    } else {
        display_white();
    }
    display_update();
}
