
#include "kernel.h"

#include "display.h"
#include "serial_io.h"

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

void set_kernel_mode() {
    usize status;
    __asm__ __volatile__("mfc0	%0, $12, 0" : "=r"(status));
    status &= ~0x10; // clear UM bit
    __asm__ __volatile__("mtc0	%0, $12, 0" : : "r"(status));
}

void set_user_mode() {
    usize status;
    __asm__ __volatile__("mfc0	%0, $12, 0" : "=r"(status));
    status |= 0x10; // set UM bit
    __asm__ __volatile__("mtc0	%0, $12, 0" : : "r"(status));
}

void kmain() {
    kernel_ctx.data = 0;
    kernel_ctx.epc = 0;
    serial_printf("Hello from the other si... ITKernel\n");
    isize ret = setjmp(&kernel_ctx);
    if (ret == 0) { // first return
        serial_printf("jump point set\n");
    } else { // return from longjmp
        serial_printf(
            "returned from jump carrying %u, epc was %p\n", kernel_ctx.data, kernel_ctx.epc);
    }

    display_string(0, "Test with text!");
    display_update();
    for (;;) {
        serial_printf("Tell me something...\n");
        char linebuf[200];
        serial_gets_s(linebuf, 200);
        serial_printf("I heard `%s`\n", linebuf);
    }
}
