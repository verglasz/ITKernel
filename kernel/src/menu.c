#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include "serial_io.h"


#include "timers.h"
#include "display.h"
#include "gpio.h"
#include "types.h"
#include "screensaver.h"

#define EVER ;;
void help_menu(void) {
    display_clear();
    display_string(0, 0, "BTN1: SEL");
    display_string(0, 8, "BTN2: DOWN");
    display_string(0, 16, "BTN3: UP");
    display_string(0, 24, "BTN4: EXIT");
    display_update();
    while (!(getbtns() & 0x8)) {

    }
}

void render_menu(char * menuItems[], int length, int selected) {

    display_clear();

    int modifier = 0;
    int i;
    if (selected < 3) {
        i = 0;
    } else {
        i = selected;
        modifier = -3;
    }
    int k;
    for (k = 0; k < 4 && k < length; k++) {
        if (i + modifier != selected) {
            display_string(0, (k * 8), menuItems[i + modifier]);
        }
        else {
            display_string_inverted(0, (k * 8), menuItems[i + modifier]);
        }
        i++;
    }


    // int k = 0;
    // int i = (selected - 3);
    // if (i < 0) i = 0;
    // for (i ; i < length; i++, k++) {
    //     if(i != selected) {
    //         display_string(0, (k * 8), menuItems[i]);
    //     }
    //     else {
    //         display_string_inverted(0, (k * 8), menuItems[i]);
    //     }
    // }
    display_update();
}

/*
    display_menu:
    @param char * items[] is a list of pointers to strings
    @param int length is the number of elements of the above list

    Function will give you a menu you can traverse and will then return the index of the item that is selected.

*/
int display_menu(char * items[], int length) {
    int selected = 0;
    u8 has_changed = 0x1;


    for (EVER) {
        if (has_changed & 0x1) {
            has_changed = 0x0;
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
            if (selected < length - 1) {
                has_changed = 0x1;
                selected++;
            }
        }

        if (btn & 0x4) {
            while (getbtns() & 0x4) {
            }
            if (selected > 0) {
                has_changed = 0x1;
                selected--;
            }
        }

        if (btn & 0x8) {
            return -1;
        }
    }
}

