
#pragma once
#include "types.h"


static __inline__ u8 getsws() {
	return (PORTD >> 8) & 0xF;
}

static __inline__ u8 getbtns() {
	return (PORTD >> 4) & 0xE | PORTF & 0x1;
}
