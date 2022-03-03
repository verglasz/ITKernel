#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include "serial_io.h"


#include "timers.h"
#include "display.h"
#include "gpio.h"
#include "types.h"
#include "screensaver.h"


void render_menu(char * menuItems[], int length, int selected) {
    
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

int display_menu(char * items[], int length) {
    int selected = 0;
    u8 consumed = 0x1;


    while (1) {

        if (consumed & 0x1) {
            consumed = 0x0;
            render_menu(items, length, selected);
        }
              
        int btn = getbtns();


        if (btn & 0x1) {
            while (getbtns() & 0x1) {
            }
            return (selected);
        }
        
        if (btn & 0x2) {
            while (getbtns() & 0x2) {
            }
            if (selected < length-1) {
                consumed = 0x1;
                selected++;
            }
        }
        
        if (btn & 0x4) {
            while (getbtns() & 0x4) {
            }
            if (selected > 0) {
                consumed = 0x1;
                selected--;
            }
        }

        if (btn & 0x8) {
            return -1;
        }
    }
}




