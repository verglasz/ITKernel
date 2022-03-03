#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <string.h>  /* Import strlen */
#include "delay.h"
#include "display.h" /* Declarations for this file. */
#include "led_signals.h"
#include "types.h"
#include "serial_io.h"
#include "gpio.h"
#include "timers.h"


void screensaver(void) {
    int x = 1, y = 1, xSpeed = 1, ySpeed = 1;
    display_clear();
    display_update();
    int sw = getsws();

    while(sw & 0x8) {
        if(x + 40 > 128) xSpeed *= -1;
        if (x <= 0) {
            xSpeed *= -1;
            x = 0;
        } 
        if(y + 8 >= 32) ySpeed *= -1;
        if (y <= 0) { 
            ySpeed *= -1;
            y = 0;
        }
        x += xSpeed;
        y += ySpeed;
        sw = getsws();

        if (sw & 0x4) {
            display_white();
            display_string_inverted(x, y, "Ding!");
        }
        else {
            display_clear();
            display_string(x , y, "Ding!");
        }
        display_update();
        sleep(50);
    }
}