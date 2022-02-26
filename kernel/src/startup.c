#include "led_signals.h"
#include "types.h"

#include <pic32mx.h>

static void main_loop();
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
    init();
    PORTECLR = 0xff;
    main_loop();
    for (;;) {}
}

static void main_loop() {}

static void init() {
    setup_gpio();
    PORTE = LED_SIGBOOT;
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
    BMXDKPBA = 8 * 1024;
    // Kernel data = 8KB
    // Optional kernel program = 8 - 8 = 0 KB
    BMXDUDBA = 8 * 1024;
    // User data = 20 - 8 = 12 KB
    BMXDUPBA = 20 * 1024;
    // User program = 32 - 20 = 12 KB
}

static void setup_peripherals() {
    SYSKEY = 0xAA996655;                 // Unlock OSCCON, step 1
    SYSKEY = 0x556699AA;                 // Unlock OSCCON, step 2
    while (OSCCON & (1 << 21)) {}        // Wait until PBDIV ready
    OSCCONCLR = PIC32_OSCCON_PBDIV_MASK; // Clear PBDIV
    while (OSCCON & (1 << 21)) {}        // Wait until PBDIV ready
    OSCCONSET = PIC32_OSCCON_PBDIV_2;    // Set PBCLK to 40MHz (divisor 2)
    SYSKEY = 0x0;                        // Lock OSCCON
}

static void setup_interrupts() {
    return;
}
