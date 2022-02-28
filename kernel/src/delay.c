
#include "delay.h"

void delay(usize count) {
    for (; count > 0; count--) {
        __asm__("nop"); // prevent the compiler from optimizing away the loop
    }
}
