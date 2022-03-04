#include "display.h"
#include "i2c.h"
#include "interrupts.h"
#include "kernel.h"
#include "led_signals.h"
#include "serial_io.h"
#include "timers.h"
#include "types.h"
#include "uart.h"

#include <pic32mx.h>

static void init();
static void setup_memory();
static void setup_gpio();
static void setup_peripherals();
static void setup_interrupts();

void _nmi_handler() {
    LED_DEBUG(LED_NMI);
    for (;;) {}
}

void _on_reset() {}
void _on_bootstrap() {}

void startup(void *stack_pointer) {
    LED_DEBUG(LED_BOOT);
    init();
    serial_printf("Initialization complete, enabling interrupts globally\n");
    enable_interrupts();
    LED_DEBUG(LED_INIT_INTEN);
    serial_printf("Interrupts enabled\n");
    // to do: Add startup/init for display
    kmain();
    for (;;) {}
}

static void init() {
    setup_gpio();
    LED_DEBUG(LED_INIT_GPIO);
    setup_memory();
    LED_DEBUG(LED_INIT_MEM);
    setup_interrupts();
    LED_DEBUG(LED_INIT_INTS);
    setup_peripherals();
    LED_DEBUG(LED_INIT_PERIPH);
}

static void setup_gpio() {
    TRISECLR = 0xFF;  // LEDs as outputs
    TRISFSET = 1 << 1;   // BTN1 as input
    TRISDSET = 0xFE0; // SW1-4 and BTN2-4 as inputs
}

static void setup_memory() {
    // setup ram
    // Kernel data ram = 8KB
    BMXDKPBA = 8 * 1024;
    // Optional kernel program ram = 8 - 8 = 0 KB
    BMXDUDBA = 8 * 1024;
    // User data ram = 20 - 8 = 12 KB
    BMXDUPBA = 20 * 1024;
    // User program ram = 32 - 20 = 12 KB
    //
    // setup flash
    // // we have 512kiB (0x80_000), some of it has special uses
    // // so let's just say last 0x10_000 (64k) are for user data and misc stuff
    // the above was the oritinal plans but my tools are broken so fuck it,
    // a very short program it is
    BMXPUPBA = 0x09000;
    // all the rest is for the kernel
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
    LED_DEBUG(LED_INIT_I2C);
    uart_setup();
    LED_DEBUG(LED_INIT_UART);
    timers_setup();
    LED_DEBUG(LED_INIT_TIMER);
    display_init();
    LED_DEBUG(LED_INIT_DISPL);
}

/* Configure cpu and interrupt controller
 * for vectored interrupts
 */
static void setup_interrupts() {
    // everything disabled
    IEC(0) = 0;
    IEC(1) = 0;
    // every flag cleared
    IFS(0) = 0;
    IFS(1) = 0;
    // priorities set to zero
    IPC(0) = 0;
    IPC(1) = 0;
    IPC(2) = 0;
    IPC(3) = 0;
    IPC(4) = 0;
    IPC(5) = 0;
    IPC(6) = 0;
    IPC(7) = 0;
    IPC(8) = 0;
    IPC(9) = 0;
    IPC(10) = 0;
    IPC(11) = 0;
    usize cause;
    __asm__("mfc0	%0, $13, 0" : "=r"(cause));        // read Cause register
    cause |= 0x00800000;                               // interrupts to special interrupt vectors
    __asm__("mtc0	%0, $13, 0\n" ::"r"(cause));       // write Cause register
    __asm__("mtc0	%0, $12, 1\n" ::"r"(0x1 << 5));    // write IntCtl register (vector spacing)
    __asm__("mtc0	%0, $15, 1\n" ::"r"(0x9d000000u)); // write Ebase
    __asm__("ehb");
    INTCON = PIC32_INTCON_MVEC | (0x4 << 16); // enable multi-vector and set vector spacing
}
