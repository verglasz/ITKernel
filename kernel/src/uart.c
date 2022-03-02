/*
 * We're using UART1, the module has 4-level rx and tx buffer
 */

#include "uart.h"

#include "common.h"
#include "led_signals.h"

#include <pic32mx.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    UART_NONE = 0,
    UART_RX = 1 << 0,
    UART_TX = 1 << 1,
    UART_ALL = UART_RX | UART_TX,
} UartIrq;

#define UART_BUFFER_SIZE 1024 // 1kiB

typedef struct {
    u16 head; // u16s to save space, since we know the buffer size will fit
    u16 tail;
    u8 buffer[UART_BUFFER_SIZE];
} RingBuffer;

static volatile RingBuffer rx_buffer;

static UartIrq current_irq;

void uart_handle_rx_int() {
    LED_DEBUG(LED_U1RXINT);
    while (((rx_buffer.tail + 1) % UART_BUFFER_SIZE != rx_buffer.head)
           && (U1STA & PIC32_USTA_URXDA)) {
        rx_buffer.buffer[rx_buffer.tail] = U1RXREG;
        rx_buffer.tail = (rx_buffer.tail + 1) % UART_BUFFER_SIZE;
        if (!(U1STA & PIC32_USTA_URXDA)) { U1STACLR = PIC32_USTA_OERR; }
    }
}

void uart_interrupt_enable(UartIrq select) {
    current_irq |= select;
    if (select != UART_NONE) {
        IPCSET(6) = 0b00100; // if we're enabling something, set priority 1:0
    }
    // for UART1, interrupt control is on IEC0,
    // receiver is bit 27, transmitter is bit 28
    IECSET(0) = select << 27;
}

void uart_interrupt_disable(UartIrq select) {
    current_irq &= ~select;
    if (select == UART_ALL) {
        IPCCLR(6) = 0b11111; // clear priority
    }
    IECCLR(0) = select << 27;
}

void uart_setup() {
    // initialize buffer pointers
    rx_buffer.head = 0;
    rx_buffer.tail = 0;
    // set baud rate to 19200
    U1BRG = PIC32_BRG_BAUD(PERIPH_CLOCK_FREQ, 19200);
    // clear status
    U1STA = 0;
    // clear mode
    U1MODE = 0;
    // Enable TX and RX
    U1STASET = PIC32_USTA_UTXEN | PIC32_USTA_URXEN;
    // U1MODESET = 3 << 8;
    // enable
    U1MODESET = PIC32_UMODE_ON;
    // enable rx interrupt
    // uart_interrupt_enable(UART_RX);
}

/* write first `len` bytes from buffer to uart (blocking)
 * `buffer` MUST be a valid pointer to at least `len` bytes of valid memory
 */
void uart_write_n(const u8 *buffer, usize len) {
    LED_DEBUG(LED_UART_WRITE_START);
    for (usize i = 0; i < len; i++) {
        while (U1STA & PIC32_USTA_UTXBF) {} // wait for space in the tx buffer
        LED_DEBUG(LED_UART_WRITE_PUT1);
        U1TXREG = buffer[i];
    }
    LED_DEBUG(LED_UART_WRITE_DONE);
}

u8 uart_async_read_byte() {
    while (rx_buffer.head == rx_buffer.tail) {}
    u8 ret = rx_buffer.buffer[rx_buffer.head];
    rx_buffer.head = (rx_buffer.head + 1) % UART_BUFFER_SIZE;
    return ret;
}

u8 uart_direct_read_byte() {
    while (!(U1STA & PIC32_USTA_URXDA)) { U1STACLR = PIC32_USTA_OERR; }
    return U1RXREG;
}

/*  read `len` bytes from uart rx buffers to provided char buffer,
 * `buffer` MUST be a valid pointer to at least `len` bytes of usable memory
 */
void uart_direct_read_n(u8 *buffer, usize len) {
    LED_DEBUG(LED_UART_READ_START);
    for (usize i = 0; i < len; i++) {
        if (!(U1STA & PIC32_USTA_URXDA)) { U1STACLR = PIC32_USTA_OERR; }
        while (!(U1STA & PIC32_USTA_URXDA)) {}
        LED_DEBUG(LED_UART_READ_GOT1);
        buffer[i] = U1RXREG;
    }
    LED_DEBUG(LED_UART_READ_DONE);
}
