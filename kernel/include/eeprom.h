#pragma once
#include "types.h"
#include "i2c.h"

isize eeprom_read(u16 addr, u8 *buf, u16 len);
isize eeprom_write(u16 addr, u8 *data, u16 len);

