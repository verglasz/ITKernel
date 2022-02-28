#pragma once

typedef enum {
	I2C_NACK = 0,
	I2C_ACK = 1,
} AckStatus;

void i2c_setup();

