
#include "timers.h"

#include "led_signals.h"

#include <pic32mx.h>

/* We use timer 4 as system timer,
 * timer 2/3 as 32 bit timer pair for on demand waits
 * and for user program timeout;
 * all clocks set up with 64 presecaler/divisor
 *
 * Peripheral clock is 40MHz,
 * so the period is 1/(2^9 * 5^7) seconds, or 2^-6 5^-4 ms
 *
 * With prescaler set to 64 (== 2^6), that's a count of 625 (=5^4)
 * per millisecond of timeout,
 * and in general a minimum precision of 1.6us
 */

volatile static u32 global_ms_count = 0;

void timers_handle_t4() {
    LED_DEBUG(LED_T4INT);
    global_ms_count++;
}

void timers_setup() {
    serial_printf("Initializing timers\n");
    // clear control registers
    T2CON = 0;
    T3CON = 0;
    T4CON = 0;
    // clear values
    TMR2 = 0;
    TMR3 = 0;
    TMR4 = 0;
    // set prescalers
    T2CONSET = 0x60; // 0b110 prescaler
    T3CONSET = 0x60; // i think this is unneded (slave timer)
    T4CONSET = 0x60;

    // configure 32bit operation
    T2CONSET = 0x8;

    // set timer 4 timeout to 1ms
    PR4 = 1 * TIMER_MS_WAIT;
    // turn on timer 4
    T4CONSET = 0x8000;

    serial_printf("Enabling system timer interrupts\n");
    // enable interrupts from timer 4
    IECSET(0) = 1u << PIC32_IRQ_T4;
    IPCSET(4) = 0x4;
}

isize sleep(u32 millis) {
    if (millis > MAX_USIZE / TIMER_MS_WAIT) return -1;
    shortsleep(millis * TIMER_MS_WAIT);
    return 0;
}

void shortsleep(u32 units) {
    if (units == 0) return;
    // turn off timer
    T2CONCLR = 0x8000;
    // zero timer
    TMR2 = 0;
    TMR3 = 0;
    // load timeout value
    PR2 = units & 0xffff;
    PR3 = units >> 16;
    // clear interrupt flag
    IFSCLR(0) = 1u << PIC32_IRQ_T3;
    // turn on timer
    T2CONSET = 0x8000;
    // wait for timer interrupt
    while (!(IFS(0) & 1u << PIC32_IRQ_T3)) {}
    // turn off timer
    T2CONCLR = 0x8000;
    // clear IF
    IFSCLR(0) = 1u << PIC32_IRQ_T3;
}

u32 get_time() {
    return global_ms_count;
}

void timers_setup_user_timeout(u32 timeout) {}
