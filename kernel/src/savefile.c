
#include "savefile.h"

#include "eeprom.h"
#include "uart.h"

#define BUFSIZE 64

isize receive_and_store(u16 addr) {
    isize filesize;
    uart_direct_read_n(&filesize, sizeof(filesize));
    isize remaining = filesize;
    u8 buffer[BUFSIZE];
    while (remaining >= BUFSIZE) {
        uart_direct_read_n(buffer, BUFSIZE);
        isize ret = eeprom_write(addr, buffer, BUFSIZE);
        if (ret != 0) return -1;
        remaining -= BUFSIZE;
        addr += BUFSIZE;
    }
    uart_direct_read_n(buffer, remaining);
    isize ret = eeprom_write(addr, buffer, remaining);
    if (ret != 0) return -1;
    return filesize;
}
