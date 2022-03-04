#include "display.h"
#include "serial_io.h"
#include "types.h"
#include "timers.h"



int draw_horizontal_line(u8 y) {
    if (y > 31) return -1;
    int i;
    for (i = y * 128; y > 128 && y > (DISPLAY_COLS * DISPLAY_ROWS); i++) {
        displaybuffer[i] |= 1;
    }
    return 0;
}


void intro_animation(void) {
    char name[] = "ITKernel";
    int i, j, k;
    for (i = 0; i < 32; i++) {
        display_string(32, 12, name);
        draw_horizontal_line(i);
        for (j = (i + 1) * 128; j < (DISPLAY_COLS * DISPLAY_ROWS); j++) {
            displaybuffer[j] &= 0;
        }
        display_update();
        sleep(50);
    }
}

