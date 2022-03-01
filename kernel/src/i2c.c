#include "i2c.h"

#include <pic32mx.h>

#define IFS_I2C1M (0x80000000u) //  1<<31, bit in IFS0 for i2c1 master events

/* We're using module I2C1;
 * The I2CBRG register should be set to:
 * ((1/2*I2CSystemFreq - Delay) * PeripheralBusClockFreq) - 2
 * (assuming our chip is using the default rate (PBCLK and not PBCLK/2))
 * (also assuming that I understood the definition correctly and there's a typo
 * in the manual, since if each BRG period should be half an I2C system period
 * then we should use 2*I2Cfreq not 1*...),
 * the delay required should be around 120ns
 * so for 100 kHz transmission
 * (the eeprom should support 400kHz since realistically it's getting +5V Vcc but let's be safe)
 * and with our 40MHz PBCLK
 * we get (5us - 120ns) * 40MHz - 2
 * = 4.88e-6 * 40e6 - 2
 * = 195.2 - 2 ~= 193
 * (using 193 imples 115 ns delay, which should be good enough;
 * even if the delay ends up being 100ns we get an i2c system half-period of
 * 195 / 40MHz + 100ns = 4.975us, so a frequency of 102.5kHz,
 * should be good enough and the clock time is in specs for the eeprom assuming fast rise/fall)
 * if we wanted 400kHz we'd do 1.15us * 40MHz - 2 = 44 (delay 100 ns conservatively)
 */

void i2c_setup() {
    // clear status and control registers
    I2C1CON = 0;
    I2C1STAT = 0;
    // set up "baud rate"
    I2C1BRG = 193;
    // enable
    I2C1CONSET = PIC32_I2CCON_ON; // why isn't this macro called `_EN` like the others?
}

static void _wait_until_ready(u32 extra_statflags) {
    // lowest 5 bits of INTCON are set until the respective operation has finished,
    // TRSTAT is set in INTSTAT if a transmission is in progress
    // we just wait for both of these to be done,
    // plus for whatever other status flags we pass in
    while (I2C1CON & 0x1f || I2C1STAT & (PIC32_I2CSTAT_TRSTAT | extra_statflags)) {}
}

/* wait until the interrupt flag associated with master events is set,
 * then clear it
 */
static __inline__ void _wait_and_clear_if() {
    while (!(IFS(0) & IFS_I2C1M)) {} // wait for interrupt signalling completed event
    IFSCLR(0) = IFS_I2C1M;           // clear i2c master event interrupt flag
}

static __inline__ void _send_ack() {
    _wait_until_ready(0);
    I2C1CONCLR = PIC32_I2CCON_ACKDT;
    I2C1CONSET = PIC32_I2CCON_ACKEN;
}
static __inline__ void _send_nack() {
    _wait_until_ready(0);
    I2C1CONSET = PIC32_I2CCON_ACKDT;
    I2C1CONSET = PIC32_I2CCON_ACKEN;
}

void i2c_start() {
    _wait_until_ready(0);
    IFSCLR(0) = IFS_I2C1M;         // clear i2c master event interrupt flag
    I2C1CONSET = PIC32_I2CCON_SEN; // begin start event
    _wait_and_clear_if();
}

void i2c_restart() {
    _wait_until_ready(0);
    IFSCLR(0) = IFS_I2C1M;          // clear i2c master event interrupt flag
    I2C1CONSET = PIC32_I2CCON_RSEN; // begin start event
    _wait_and_clear_if();
}

void i2c_stop() {
    _wait_until_ready(0);
    IFSCLR(0) = IFS_I2C1M;         // clear i2c master event interrupt flag
    I2C1CONSET = PIC32_I2CCON_PEN; // begin stop event
    _wait_and_clear_if();
}

/* Send single byte over i2c.
 * Must be called after starting transmission with i2c_start
 */
AckStatus i2c_send(u8 byte) {
    _wait_until_ready(PIC32_I2CSTAT_TBF); // wait for buffer to be free
    IFSCLR(0) = IFS_I2C1M;                // clear i2c master event interrupt flag
    I2C1TRN = byte;
    _wait_and_clear_if(); // interrupt is signalled after ack status is recorded
    return (I2C1STAT & PIC32_I2CSTAT_ACKSTAT) ? I2C_NACK : I2C_ACK; // return ack status
}

/* Receive single byte over i2c.
 * Must be called after starting transmission with i2c_start
 */
u8 i2c_receive(AckStatus ack) {
    if (I2C1STAT & PIC32_I2CSTAT_RBF) { // if there's data in the receive buffer, use it
        return I2C1RCV;
    }
    _wait_until_ready(0);
    I2C1CONSET = PIC32_I2CCON_RCEN;
    while (!(I2C1STAT & PIC32_I2CSTAT_RBF)) {} // wait for data in the buffer
    if (ack == I2C_NACK) {
        _send_nack();
    } else {
        _send_ack();
    }
    return I2C1RCV;
}
