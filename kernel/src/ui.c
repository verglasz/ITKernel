#include "menu.h"
#include "display.h"



void main_menu(void) {
    int MENU_LEN = 8;
    const char *menuItems[] = {
        "#0: HELP", "#1: SCRNSVR", "#2: INPUT", "#3: PROGRAMS", "#4: None", "#5: None", "#6: None", "#7: None"
    };
    help_menu(); // Start the display on the help menu
    while (1) {
        int resp = display_menu(menuItems, MENU_LEN);

        
        if (resp == 0) {
            help_menu();
        }

        if (resp == 1) {
            screensaver();
        }

        if (resp == 2) {
            get_input();
        }
        
        if (resp == 3) {
            program_loop();
        }
    }
}

void program_loop() {

}

