#include "display.h"
#include "serial_io.h"
#include "timers.h"
#include "types.h"

int draw_horizontal_line(u8 y) {
    if (y > 31) return -1;
    u8 row = y / 8;
    u8 pixel = 1 << y % 8;
    for (int i = 0; i < DISPLAY_COLS; i++) {
        // maybe?
        displaybuffer[DISPLAY_COLS * row + i] |= pixel;
    }
    return 0;
}

int clear_horizontal_line(u8 y) {
    if (y > 31) return -1;
    u8 row = y / 8;
    u8 pixel = 1 << y % 8;
    for (int i = 0; i < DISPLAY_COLS; i++) {
        // maybe?
        displaybuffer[DISPLAY_COLS * row + i] &= ~pixel;
    }
    return 0;
}

void intro_animation(void) {
    char name[] = "ITKernel";
    int i;
    display_white();
    display_update();
    for (i = 0; i < 32; i++) {
        display_string_inverted(32, 12, name);
        clear_horizontal_line(i);
        draw_horizontal_line(i + 1);
        display_update();
        sleep(50);
    }
}
