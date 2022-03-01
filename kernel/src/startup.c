#include "i2c.h"
#include "interrupts.h"
#include "led_signals.h"
#include "serial_io.h"
#include "types.h"
#include "uart.h"
#include "kernel.h"

#include <pic32mx.h>

static void init();
static void setup_memory();
static void setup_gpio();
static void setup_peripherals();
static void setup_interrupts();

void _nmi_handler() {
    PORTE = LED_SIGNMI;
    for (;;) {}
}

void _on_reset() {}
void _on_bootstrap() {}

void startup(void *stack_pointer) {
    PORTE = LED_SIGBOOT;
    init();
    PORTECLR = 0xff;
    enable_interrupts();
    kmain();
    for (;;) {}
}

static void init() {
    setup_gpio();
    setup_memory();
    setup_peripherals();
    setup_interrupts();
}

static void setup_gpio() {
    TRISECLR = 0xFF;  // LEDs as outputs
    TRISFSET = 1;     // BTN1 as input
    TRISDSET = 0xFE0; // SW1-4 and BTN2-4 as inputs
}

static void setup_memory() {
    // Kernel data ram = 8KB
    BMXDKPBA = 8 * 1024;
    // Optional kernel program ram = 8 - 8 = 0 KB
    BMXDUDBA = 8 * 1024;
    // User data ram = 20 - 8 = 12 KB
    BMXDUPBA = 20 * 1024;
    // User program ram = 32 - 20 = 12 KB
}

static void setup_peripherals() {
    SYSKEY = 0x0;                        // Ensure OSCCON locked so unlocking surely works
    SYSKEY = 0xAA996655;                 // Unlock OSCCON, step 1
    SYSKEY = 0x556699AA;                 // Unlock OSCCON, step 2
    while (OSCCON & (1 << 21)) {}        // Wait until PBDIV ready
    OSCCONCLR = PIC32_OSCCON_PBDIV_MASK; // Clear PBDIV
    while (OSCCON & (1 << 21)) {}        // Wait until PBDIV ready
    OSCCONSET = PIC32_OSCCON_PBDIV_2;    // Set PBCLK to 40MHz (divisor 2)
    SYSKEY = 0x0;                        // Lock OSCCON
    i2c_setup();
    uart_setup();
}

/* Configure cpu and interrupt controller
 * for vectored interrupts
 */
static void setup_interrupts() {
    usize cause;
    __asm__("mfc0	%0, $13, 0" : "=r"(cause));       // read Cause register
    cause |= 0x00800000;                              // interrupts to special interrupt vectors
    __asm__("mtc0	%0, $13, 0\n" ::"r"(cause));      // write Cause register
    __asm__("mtc0	%0, $12, 1\n" ::"r"(0x1 << 5));   // write IntCtl register (vector spacing)
    __asm__("mtc0	%0, $15, 1\n" ::"r"(0x9d001000)); // write Ebase
    __asm__("ehb");
    INTCON = PIC32_INTCON_MVEC | (0x4 << 16); // enable multi-vector and set vector spacing
}

