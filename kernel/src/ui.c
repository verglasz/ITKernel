#include "animation.h"
#include "display.h"
#include "elf.h"
#include "gpio.h"
#include "kernel.h"
#include "menu.h"
#include "screensaver.h"
#include "serial_io.h"
#include "tests.h"
#include "usermode.h"
#include "ustar.h"

#define MAX_FILES 20

int program_selector();

void main_menu(void) {
    int MENU_LEN = 8;
    const char *menuItems[] = { "#0: HELP", "#1: SCRNSVR", "#2: INPUT", "#3: PROGRAMS",
                                "#4: None", "#5: None",    "#6: None",  "#7: None" };
    help_menu(); // Start the display on the help menu
    while (1) {
        int resp = display_menu(menuItems, MENU_LEN);

        switch (resp) {
        case 0:
            help_menu();
            break;
        case 1:
            screensaver();
            break;
        case 2:
            test_get_input();
            break;
        case 3:
            program_selector();
            break;
        case 4:
            intro_animation();
            break;
        }
    }
}

int program_selector() {
    FileInfo fbuf[MAX_FILES];
    isize ret = ustar_list_files(0x0, fbuf, MAX_FILES);
    serial_printf("program_selector: ustar_list_files returned %d\n", ret);
    if (ret <= 0) { return -1; }
    char *fnames[MAX_FILES];
    for (int i = 0; i < ret; i++) { fnames[i] = fbuf[i].filename; }

    serial_printf("program_selector: showing menu\n");
    int selection = display_menu(fnames, ret);
    serial_printf("program_selector: selected %d\n", selection);
    if (selection == -1) return -1;
    serial_printf("program_selector: running program\n");
    int retval = run_file(&fbuf[selection]);
    if (retval == -1) return -2;
    display_clear();
    char buf[24];
    __builtin_sprintf(buf, "returned: %d", retval);
    display_string(0, 0, "Program exited");
    display_string(0, 8, buf);
    display_update();
    while (!getbtns()) {}
    while (getbtns()) {}
    return 0;
}
