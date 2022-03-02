
#include "eeprom.h"

#include "delay.h"

#define EEPROM_ADDR 0xA0
#define EEPROM_PAGESIZE 64
#define EEPROM_PAGEMASK (EEPROM_PAGESIZE - 1)

/* write len bytes from `data` buffer to eeprom,
 * starting at address `addr`
 */
isize eeprom_write(u16 addr, u8 *data, u16 len) {
    if ((usize)len + (usize)addr > 0x7FFF) return -1; // OOB write (promotions ensure no overflow)

    while (len > 0) {
        do {
            // resend start until address gets ACK'd
            i2c_start();
            // start transmission with address, last bit 0 for writes
        } while (i2c_send(EEPROM_ADDR) == I2C_NACK);
        i2c_send(addr >> 8);
        i2c_send(addr & 0xFF);
        do {
            i2c_send(*data);
            data++;
            len--;
            addr++;
        } while (len > 0 && (addr & EEPROM_PAGEMASK)); // send bits up to page boundary
        i2c_stop();
        // we should wait for the write cycle to complete here (5ms)
        delay(1e6);
    }
    return 0;
}

/* write len bytes from `data` buffer to eeprom,
 * starting at address `addr`
 */
isize eeprom_read(u16 addr, u8 *buf, u16 len) {
    if ((usize)len + (usize)addr > 0x7FFF) return -1; // OOB read (promotions ensure no overflow)
    if (len == 0) return 0;                           // read 0 bytes...

    do {
        // resend start until address gets ACK'd
        i2c_start();
        // start transmission with address, last bit 0 for writes
    } while (i2c_send(EEPROM_ADDR) == I2C_NACK);
    i2c_send(addr >> 8);
    i2c_send(addr & 0xFF);
    i2c_start();               // restart
    i2c_send(EEPROM_ADDR | 1); // send read command (last bit 1)
    while (len > 1) {
        *buf = i2c_receive(I2C_ACK);
        buf++;
        len--;
    }
    *buf = i2c_receive(I2C_NACK);
    i2c_stop();
    return 0;
}
