#include "itklibc.h"

int main(void) {
    int btncount = 0;
    int change = 1;

    while (btncount < 16) {
        if (change) {
            change = 0;

            screen_clear(0x0);
            if (btncount & 0x1) screen_print(0, " ", 0xf);
            if (btncount & 0x2) screen_print(1, " ", 0xf);
            if (btncount & 0x4) screen_print(2, " ", 0xf);
            if (btncount & 0x8) screen_print(3, " ", 0xf);
        }
        
        
        int btn = getbtns();
        
        if (btn & 0x4) {
            while(getbtns() & 0x4) {
            }
            btncount++;
            change = 1;
        }
        if (btn & 0x8) {
            while(getbtns() & 0x8) {
            }
            if (btncount > 0) btncount--;
            change = 1;
        }
        if (btn & 0x1) {
            btncount = 16;
        }
    }
}