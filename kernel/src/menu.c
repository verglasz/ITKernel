#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include "serial_io.h"


#include "delay.h"
#include "display.h"
#include "gpio.h"
#include "types.h"
#include "screensaver.h"


void render_menu(char * menuItems[], int length, u8 selected) {
    
    display_clear();
    int k = 0;
    int i = (selected - 3);
    if (i < 0) i = 0;
    for (i ; i < length; i++, k++) {
        if(i != selected) {
            display_string(0, (k * 8), menuItems[i]);
        }
        else {
            display_string_inverted(0, (k * 8), menuItems[i]);
        }
    }

    display_update();
}

void display_menu(char * items[], int length) {
    u8 selected = 0;
    int run = 1;
    int count = 0;
    u8 consumed = 0x0;


    while (run) {
        if (count % 10 == 0) {
            count = 0;
            consumed = 0x0;
            render_menu(items, length, selected);
        }
        
        int btn = getbtns();
        
        if (btn & 0x2) {
            if (selected == 0 || selected == 7) {
                screensaver();
                continue;
            }
            if (selected == 1 || selected == 4 || selected == 10) {
                run = 0;
                display_clear();
                display_string(8, 12, "Awaiting input");
                display_update();
            }

        }
        
        if (btn & 0x4) {
            if (consumed & 0x1) continue;
            if (selected < length-1) {
                consumed |= 0x1;
                selected++;
            }
        }
        
        if (btn & 0x8) {
            if (consumed & 0x1) continue;
            if (selected > 0) {
                consumed |= 0x1;
                selected--;
                }
        }
        count++;
    }
}




