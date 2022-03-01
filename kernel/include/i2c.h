#pragma once
#include "types.h"

typedef enum {
	I2C_NACK = 0,
	I2C_ACK = 1,
} AckStatus;

void i2c_setup();
void i2c_start();
void i2c_restart();
void i2c_stop();
AckStatus i2c_send(u8 byte);
u8 i2c_receive(AckStatus ack);

