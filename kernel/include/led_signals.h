
#pragma once

#include <pic32mx.h>
#include "delay.h"

/* #define LED_DEBUG(code) do {PORTE = code; delay(1);}while(0) */
#define LED_DEBUG(code) PORTE = code

#define LED_STARTUP	0x10

#define LED_BOOT		(LED_STARTUP | 0x0)
#define LED_INIT_GPIO	(LED_STARTUP | 0x1)
#define LED_INIT_MEM	(LED_STARTUP | 0x2)
#define LED_INIT_PERIPH	(LED_STARTUP | 0x8)
#define LED_INIT_OSC	(LED_STARTUP | 0x9)
#define LED_INIT_I2C	(LED_STARTUP | 0xa)
#define LED_INIT_UART	(LED_STARTUP | 0xb)
#define LED_INIT_SPI	(LED_STARTUP | 0xc)
#define LED_INIT_DISPL	(LED_STARTUP | 0xd)
#define LED_INIT_TIMER	(LED_STARTUP | 0xe)
#define LED_INIT_INTS	(LED_STARTUP | 0x4)
#define LED_INIT_INTEN	(LED_STARTUP | 0x5)

#define LED_KMAIN	0x20

#define LED_SERIAL_PRINT	(LED_KMAIN | 0x1)
#define LED_SETJMP			(LED_KMAIN | 0x2)
#define LED_SCREEN_PRINT	(LED_KMAIN | 0x3)
#define LED_SCREEN_FLUSH	(LED_KMAIN | 0x4)
#define LED_WAIT_ECHO		(LED_KMAIN | 0x5)
#define LED_GOT_ECHO		(LED_KMAIN | 0x6)

#define LED_UART	0x30

#define LED_UART_WRITE_START		(LED_UART | 0x1)
#define LED_UART_WRITE_PUT1		(LED_UART | 0x2)
#define LED_UART_WRITE_DONE		(LED_UART | 0x3)
#define LED_UART_READ_START		(LED_UART | 0x8)
#define LED_UART_READ_GOT1		(LED_UART | 0x9)
#define LED_UART_READ_DONE		(LED_UART | 0xa)

#define LED_SPI		0x40

#define LED_SPI_SENDREC_START	(LED_SPI | 0x1)
#define LED_SPI_PRESEND			(LED_SPI | 0x2)
#define LED_SPI_SENT1			(LED_SPI | 0x3)
#define LED_SPI_PRERECV			(LED_SPI | 0x4)
#define LED_SPI_RECV1			(LED_SPI | 0x5)

#define LED_DISPLAY	0x50

#define LED_DISPLAY_STARTFLUSH	(LED_DISPLAY | 0x1)


#define LED_TRAP		0xE0

#define LED_INT		(LED_TRAP | 0x1)
#define LED_SYS		(LED_TRAP | 0x2)
#define LED_EXC		(LED_TRAP | 0x3)
#define LED_U1RXINT	(LED_TRAP | 0x4)
#define LED_T4INT	(LED_TRAP | 0x5)
#define LED_T3INT	(LED_TRAP | 0x6)

#define LED_INTRET	(LED_TRAP | 0x8)

#define LED_NMI		(LED_TRAP | 0xe)

