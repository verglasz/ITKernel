#include "kernel.h"

#include "delay.h"
#include "display.h"
#include "eeprom.h"
#include "elf.h"
#include "gpio.h"
#include "led_signals.h"
#include "menu.h"
#include "screensaver.h"
#include "serial_io.h"
#include "tests.h"
#include "timers.h"
#include "uart.h"
#include "ui.h"
#include "usermode.h"
#include "ustar.h"

struct context_t {
    usize sp;
    usize ra;
    usize gp;
    usize s0;
    usize s1;
    usize s2;
    usize s3;
    usize s4;
    usize s5;
    usize s6;
    usize s7;
    usize s8;
    usize data;
    void *epc;
};

Context kernel_ctx;

int get_retval(Context *ctx) {
    return ctx->data & 0xffff;
}

void set_kernel_mode() {
    usize status;
    __asm__ __volatile__("mfc0	%0, $12, 0" : "=r"(status));
    status &= ~0x10; // clear UM bit
    __asm__ __volatile__("mtc0	%0, $12, 0" : : "r"(status));
}

void set_user_mode() {
    // can't really call this since it's in kernel memory
    usize status;
    __asm__ __volatile__("mfc0	%0, $12, 0" : "=r"(status));
    status |= 0x10; // set UM bit
    __asm__ __volatile__("mtc0	%0, $12, 0" : : "r"(status));
}

int run_file(FileInfo *f) {
    EntryPoint entry = elf_load_file(f->rom_addr);
    serial_printf("run_file: elf loaded, entry: 0x%x\n", (usize)entry);
    if (entry == NULL) return -1;
    if (setjmp(&kernel_ctx) == 0) {
        serial_printf("run_file: jumping to user program\n");
        timers_setup_user_timeout(60 * TIMER_MS_WAIT * 1000);
        usermode_jump(entry, (usize *)USER_DATA_END);
    } else {
        serial_printf("run_file: user program exited\n");
        return get_retval(&kernel_ctx) & 0xffff;
    }
}

void kmain() {
    LED_DEBUG(LED_KMAIN);
    kernel_ctx.data = 0;
    kernel_ctx.epc = 0;
    serial_printf("Hello from the other si... ITKernel\n");
    LED_DEBUG(LED_SERIAL_PRINT);
    isize ret = setjmp(&kernel_ctx);
    LED_DEBUG(LED_SETJMP);
    if (ret == 0) { // first return
        serial_printf("jump point set\n");
    } else { // return from longjmp
        serial_printf(
            "returned from jump carrying %u, epc was 0x%x\n", kernel_ctx.data, kernel_ctx.epc);
    }

    main_menu();
}
