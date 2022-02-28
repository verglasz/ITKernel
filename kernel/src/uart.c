/*
 * We're using UART1, the module has 4-level rx and tx buffer
 */

#include "uart.h"

#include "common.h"

#include <pic32mx.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define UART_BUFFER_SIZE 512

typedef struct {
    u16 head; // u16s to save space, since we know the buffer size will fit
    u16 tail;
    u8 buffer[UART_BUFFER_SIZE];
} RingBuffer;

static RingBuffer rx_buffer;

typedef enum {
    UART_NONE = 0,
    UART_RX = 1 << 0,
    UART_TX = 1 << 1,
    UART_ALL = UART_RX | UART_TX,
} UartIrq;

void uart_interrupt_enable(UartIrq select) {
    if (select != UART_NONE) {
        IPCSET(6) = 0b00100; // if we're enabling something, set priority 1:0
    }
    // for UART1, interrupt control is on IEC0,
    // receiver is bit 27, transmitter is bit 28
    IECSET(0) = select << 27;
}

void uart_interrupt_disable(UartIrq select) {
    if (select == UART_ALL) {
        IPCCLR(6) = 0b11111; // clear priority
    }
    IECCLR(0) = select << 27;
}

void uart_setup() {
    // initialize buffer pointers (should be unnecessary since globals are zeroed..)
    rx_buffer.head = 0;
    rx_buffer.tail = 0;
    // set baud rate to 19200
    U1BRG = PIC32_BRG_BAUD(PERIPH_CLOCK_FREQ, 19200);
    // clear status
    U1STA = 0;
    // Enable UART1, clear every other bit
    U1MODE = PIC32_UMODE_ON;
    // Enable TX and RX
    U1STASET = PIC32_USTA_UTXEN | PIC32_USTA_URXEN;
}

/* write first `len` bytes from buffer to uart (blocking)
 * `buffer` MUST be a valid pointer to at least `len` bytes of valid memory
 */
void uart_write_n(const u8 *buffer, usize len) {
    for (usize i = 0; i < len; i++) {
        while (U1STA & PIC32_USTA_UTXBF) {} // wait for space in the tx buffer
        U1TXREG = buffer[i];
    }
}

/*  read `len` bytes from uart rx buffers to provided char buffer,
 * `buffer` MUST be a valid pointer to at least `len` bytes of usable memory
 */
void uart_direct_read_n(u8 *buffer, usize len) {
    for (usize i = 0; i < len; i++) {
        while (!(U1STA & PIC32_USTA_URXDA)) {}
        buffer[i] = U1RXREG;
    }
}
