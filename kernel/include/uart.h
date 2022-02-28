#pragma once

#include "types.h"

void uart_setup();
void uart_write_n(const u8 *buffer, usize len);
void uart_direct_read_n(u8 *buffer, usize len);

