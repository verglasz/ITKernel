#pragma once

#include "types.h"


void uart_setup();
void uart_write_n(const u8 *buffer, usize len);
void uart_direct_read_n(u8 *buffer, usize len);

u8 uart_async_read_byte();
u8 uart_direct_read_byte();
void uart_handle_interrupt();

