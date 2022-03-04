#include "display.h" /* Declarations for this file. */

#include "delay.h"
#include "led_signals.h"
#include "serial_io.h"
#include "types.h"

#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <string.h>  /* Import strlen */

// buffers
u8 displaybuffer[DISPLAY_BUFFER_SIZE];
const u8 font[] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   94,  0,   0,   0,   0,   0,   0,   4,   3,   4,   3,   0,   0,   0,   36,  126, 36,  36,
    126, 36,  0,   0,   36,  74,  255, 82,  36,  0,   0,   0,   70,  38,  16,  8,   100, 98,  0,
    0,   52,  74,  74,  52,  32,  80,  0,   0,   0,   0,   4,   3,   0,   0,   0,   0,   0,   0,
    126, 129, 0,   0,   0,   0,   0,   0,   129, 126, 0,   0,   0,   0,   42,  28,  62,  28,  42,
    0,   0,   0,   8,   8,   62,  8,   8,   0,   0,   0,   0,   0,   128, 96,  0,   0,   0,   0,
    8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   96,  0,   0,   0,   0,   64,  32,  16,
    8,   4,   2,   0,   0,   62,  65,  73,  65,  62,  0,   0,   0,   0,   66,  127, 64,  0,   0,
    0,   0,   0,   98,  81,  73,  70,  0,   0,   0,   0,   34,  73,  73,  54,  0,   0,   0,   0,
    14,  8,   127, 8,   0,   0,   0,   0,   35,  69,  69,  57,  0,   0,   0,   0,   62,  73,  73,
    50,  0,   0,   0,   0,   1,   97,  25,  7,   0,   0,   0,   0,   54,  73,  73,  54,  0,   0,
    0,   0,   6,   9,   9,   126, 0,   0,   0,   0,   0,   102, 0,   0,   0,   0,   0,   0,   128,
    102, 0,   0,   0,   0,   0,   0,   8,   20,  34,  65,  0,   0,   0,   0,   20,  20,  20,  20,
    0,   0,   0,   0,   65,  34,  20,  8,   0,   0,   0,   2,   1,   81,  9,   6,   0,   0,   0,
    28,  34,  89,  89,  82,  12,  0,   0,   0,   126, 9,   9,   126, 0,   0,   0,   0,   127, 73,
    73,  54,  0,   0,   0,   0,   62,  65,  65,  34,  0,   0,   0,   0,   127, 65,  65,  62,  0,
    0,   0,   0,   127, 73,  73,  65,  0,   0,   0,   0,   127, 9,   9,   1,   0,   0,   0,   0,
    62,  65,  81,  50,  0,   0,   0,   0,   127, 8,   8,   127, 0,   0,   0,   0,   65,  127, 65,
    0,   0,   0,   0,   0,   32,  64,  64,  63,  0,   0,   0,   0,   127, 8,   20,  99,  0,   0,
    0,   0,   127, 64,  64,  64,  0,   0,   0,   127, 2,   4,   2,   127, 0,   0,   0,   127, 6,
    8,   48,  127, 0,   0,   0,   0,   62,  65,  65,  62,  0,   0,   0,   0,   127, 9,   9,   6,
    0,   0,   0,   0,   62,  65,  97,  126, 64,  0,   0,   0,   127, 9,   9,   118, 0,   0,   0,
    0,   38,  73,  73,  50,  0,   0,   0,   1,   1,   127, 1,   1,   0,   0,   0,   0,   63,  64,
    64,  63,  0,   0,   0,   31,  32,  64,  32,  31,  0,   0,   0,   63,  64,  48,  64,  63,  0,
    0,   0,   0,   119, 8,   8,   119, 0,   0,   0,   3,   4,   120, 4,   3,   0,   0,   0,   0,
    113, 73,  73,  71,  0,   0,   0,   0,   127, 65,  65,  0,   0,   0,   0,   2,   4,   8,   16,
    32,  64,  0,   0,   0,   0,   65,  65,  127, 0,   0,   0,   4,   2,   1,   2,   4,   0,   0,
    0,   64,  64,  64,  64,  64,  64,  0,   0,   0,   1,   2,   4,   0,   0,   0,   0,   0,   48,
    72,  40,  120, 0,   0,   0,   0,   127, 72,  72,  48,  0,   0,   0,   0,   48,  72,  72,  0,
    0,   0,   0,   0,   48,  72,  72,  127, 0,   0,   0,   0,   48,  88,  88,  16,  0,   0,   0,
    0,   126, 9,   1,   2,   0,   0,   0,   0,   80,  152, 152, 112, 0,   0,   0,   0,   127, 8,
    8,   112, 0,   0,   0,   0,   0,   122, 0,   0,   0,   0,   0,   0,   64,  128, 128, 122, 0,
    0,   0,   0,   127, 16,  40,  72,  0,   0,   0,   0,   0,   127, 0,   0,   0,   0,   0,   120,
    8,   16,  8,   112, 0,   0,   0,   0,   120, 8,   8,   112, 0,   0,   0,   0,   48,  72,  72,
    48,  0,   0,   0,   0,   248, 40,  40,  16,  0,   0,   0,   0,   16,  40,  40,  248, 0,   0,
    0,   0,   112, 8,   8,   16,  0,   0,   0,   0,   72,  84,  84,  36,  0,   0,   0,   0,   8,
    60,  72,  32,  0,   0,   0,   0,   56,  64,  32,  120, 0,   0,   0,   0,   56,  64,  56,  0,
    0,   0,   0,   56,  64,  32,  64,  56,  0,   0,   0,   0,   72,  48,  48,  72,  0,   0,   0,
    0,   24,  160, 160, 120, 0,   0,   0,   0,   100, 84,  84,  76,  0,   0,   0,   0,   8,   28,
    34,  65,  0,   0,   0,   0,   0,   126, 0,   0,   0,   0,   0,   0,   65,  34,  28,  8,   0,
    0,   0,   0,   4,   2,   4,   2,   0,   0,   0,   120, 68,  66,  68,  120, 0,   0,
};

/* spi_send_recv:
    Send data to the SPI2 bus
*/
u8 spi_send_recv(u8 data) {
    LED_DEBUG(LED_SPI_SENDREC_START);
    // Wait for buffer to be ready
    while (!(SPI2STAT & 0x08))
        ;
    LED_DEBUG(LED_SPI_PRESEND);
    // Put the data on the buffer
    SPI2BUF = data;
    LED_DEBUG(LED_SPI_SENT1);
    // Wait for buffer response.
    while (!(SPI2STAT & 1))
        ;
    LED_DEBUG(LED_SPI_PRERECV);
    // Return buffer response.
    u8 ret = SPI2BUF;
    LED_DEBUG(LED_SPI_RECV1);
    return ret;
}

void display_clear(void) {
    int i;
    for (i = 0; i < DISPLAY_BUFFER_SIZE; i++) { displaybuffer[i] = 0; }
}

void display_white(void) {
    int i;
    for (i = 0; i < DISPLAY_BUFFER_SIZE; i++) { displaybuffer[i] = 255; }
}

void spi2init(void) {
    // Disable interrupts for SPI2
    IECCLR(1) = 0b111 << 5;

    // Reset config and clear SPI buffer
    SPI2CON = 0;
    SPI2BUF = 0;

    // Set Baud Rate Register to 4mhz
    SPI2BRG = 4;

    // Clear the overflow
    SPI2STATCLR = PIC32_SPISTAT_SPIROV;

    /* (Idle clock) Basic I/O Shield -> Display Example */
    SPI2CONSET = PIC32_SPICON_CKP | PIC32_SPICON_MSTEN;
    // Enable SPI
    SPI2CONSET = PIC32_SPICON_ON;
}

void display_pins_init(void) {
    /*
    Vdd = RF6
    VBAT_en = RF5
    Command Select = RF4

    Reset = RD9
    */
    // Set the pins for the display
    PORTFSET = (1 << 4) | (1 << 5) | (1 << 6); // RF4 to RF6 to high
    TRISFCLR = (1 << 4) | (1 << 5) | (1 << 6); // RF4 to RF6 set to outputs

    // Set the reset pin
    PORTGSET = (1 << 9); // RG9 (reset pin) set to high
    TRISGCLR = (1 << 9); // RG9 set to output
}

void display_init(void) {
    serial_printf("Running display_init\n");

    serial_printf("> Running spi2init\n");
    spi2init();

    serial_printf("> Running display_pins_init\n");
    display_pins_init();

    serial_printf("> Clearing displaybuffer\n");
    display_clear();

    serial_printf("> Running remaining of display_init\n");
    /*
    Power on sequence:
        1. Apply power to VDD.
        2. Send Display Off command.
        3. Initialize display to desired operating mode.
        4. Clear screen.
        5. Apply power to VBAT.
        6. Delay 100ms.
        7. Send Display On command.
    */

    DISPLAY_CHANGE_TO_COMMAND_MODE;
    // Startup sequence
    delay(10000);
    DISPLAY_ACTIVATE_VDD;
    delay(1000000);
    // Turn display off
    spi_send_recv(0xAE);

    // Bring reset low, wait for driver to reset then turn reset high.
    DISPLAY_ACTIVATE_RESET;
    delay(10000);
    DISPLAY_DO_NOT_RESET;
    delay(10000);
    // End of startup sequence

    // Charge pump setting
    spi_send_recv(0x8D);
    // Value
    spi_send_recv(0x14);
    // Pre-charge period
    spi_send_recv(0xD9);
    // Num of CLK
    spi_send_recv(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    delay(10000);

    // Sets the mapping to display data column adress (Put origin in top-left corner)
    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    // Set COM pins config
    spi_send_recv(0xDA);
    // OLED config
    spi_send_recv(0x20);

    // Turn on screen (0xAF for normal mode) | (0xAE for sleep mode)
    spi_send_recv(0xAF);

    serial_printf("Display init finished!\n");
}

int display_addstring(u8 x, u8 y, const char *text, int invert) {
    int c, r, offset;
    u8 i = 0;
    u8 row = y / 8;
    u8 shift = y % 8;
    u8 column = x;
    u8 columndata, bit;
    if (x > 127) {
        serial_printf("x was greater than 127");
        return -1;
    }
    if (((128 - x) / 8) < 1) {
        serial_printf("There were not enough room to print characters: x\n");
        return -1;
    }

    if (((32 - y) / 8) < 1) {
        serial_printf("There was not enough room to print character: y\n");
        return -1;
    }

    while (text[i] != '\0') {
        if (i > 15) {
            serial_printf("String was too long (x). Cutting off at %d\n", i);
            return -1;
        }

        // Loop through each character
        for (c = 0; c < 8; c++) {
            // loop through each column of the character
            column = x + c + i * 8;
            columndata = font[text[i] * 8 + c];
            offset = (y / 8) * DISPLAY_COLS + column;

            if (!invert) { // Set the background to be white if the inversion is enabled.
                displaybuffer[offset] &= 0;
            }

            for (row = y, r = 0; row < y + 8; row++, r++) { // Add each pixel to the displaybuffer

                bit = (columndata >> r) & 1;
                offset = (row / 8) * DISPLAY_COLS + column;

                if (invert) {
                    displaybuffer[offset] |= (1 << (row % 8));
                    displaybuffer[offset] &= ~(bit << (row % 8));
                } else {
                    displaybuffer[offset] |= (bit << (row % 8));
                }
            }
        }
        i++;
    }
    return 0;
}

void display_string(uint8_t x, uint8_t y, const char *text) {
    display_addstring(x, y, text, 0);
}

void display_string_inverted(uint8_t x, uint8_t y, const char *text) {
    display_addstring(x, y, text, 1);
}

void display_update(void) {
    // page addr
    int i, j;
    for (i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);

        // start at left column
        spi_send_recv(0x00);

        spi_send_recv(0x10);
        // spi_send_recv(0x00);

        DISPLAY_CHANGE_TO_DATA_MODE;

        for (j = 0; j < DISPLAY_COLS; j++) { spi_send_recv(displaybuffer[i * DISPLAY_COLS + j]); }
    }
}

