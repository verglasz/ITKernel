
#include "kernel.h"
#include "delay.h"
#include "display.h"
#include "eeprom.h"
#include "led_signals.h"
#include "serial_io.h"
#include "uart.h"

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
            "returned from jump carrying %u, epc was %p\n", kernel_ctx.data, kernel_ctx.epc);
    }
    display_init();
    display_string(0, 0, "Here be line 1!");
    display_string(0, 8, "Here be line 2!");
    display_string(0, 16, "Here be line 3!");
    display_string(0, 24, "Here be line 4!");

    LED_DEBUG(LED_SCREEN_PRINT);
    display_update();
    LED_DEBUG(LED_SCREEN_FLUSH);
        delay(100000000);

    serial_printf("In display textbuffer[0]: `%s`...\n", textbuffer[0]);
    u16 romaddr = 0x0;
    for (u8 i = 1;; i++) {
        serial_printf("Tell me something...\n");
        LED_DEBUG(LED_WAIT_ECHO);
        char linebuf[200];
        usize read = serial_gets_s(linebuf, 200);
        LED_DEBUG(LED_GOT_ECHO);
        serial_printf("I heard `%s`\n", linebuf);
        serial_printf("Writing it to the rom, %u bytes at addr 0x%x\n", read + 1, romaddr);
        if (eeprom_write(romaddr, linebuf, read + 1) != 0) {
            serial_printf("OOB write\n");
            continue;
        }
        serial_printf("Written, now reading back\n");
        char rombuf[200];
        if (eeprom_read(romaddr, rombuf, read + 1) != 0) {
            serial_printf("OOB read\n");
            continue;
        }
        if (serial_printf("Read back `%s`\n", rombuf) < 0) {
            serial_printf("Error formatting readback string \n");
        }
        romaddr += read;
        if (i % 5 == 0) {
            int scan = 0;
            while (scan + 200 < romaddr) {
                eeprom_read(scan, rombuf, 200);
                uart_write_n(rombuf, 200);
                scan += 200;
            }
            eeprom_read(scan, rombuf, romaddr - scan + 1);
            serial_printf("%s\n", rombuf);
        }
    }
}
