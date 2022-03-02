#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
//#include "mipslab.h"  /* Declatations for these labs */
#include "display.h"  /* Declarations for this file. */



int main(void) {
    display_init();

    while(1) {
        display_string(0, "Test with text!");
        display_update();
        int i;
        for (i = 0; i < 1000000; i++){

        }
    }
    return 0;
}