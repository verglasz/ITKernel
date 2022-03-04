#include "animation.h"
#include "display.h"
#include "elf.h"
#include "gpio.h"
#include "kernel.h"
#include "menu.h"
#include "serial_io.h"
#include "tests.h"
#include "usermode.h"
#include "ustar.h"

#define MAX_FILES 20

int program_selector();

void main_menu(void) {
    const char *const menuItems[] = {
        "#0: HELP", "#1: SCRNSVR", "#2: INPUT", "#3: PROGRAMS", "#4: ANIMATION", "#5: LOAD",
    };
    serial_printf("main_menu: entering the help menu.\n");
    help_menu(); // Start the display on the help menu
    while (1) {
        int resp = display_menu(menuItems, sizeof(menuItems));

        switch (resp) {
        case 0:
            help_menu();
            serial_printf("main_menu: entering the help_menu.\n");
            break;
        case 1:
            screensaver();
            serial_printf("main_menu: entering the screensaver.\n");
            break;
        case 2:
            test_get_input();
            serial_printf("main_menu: entering the get_input.\n");
            break;
        case 3:
            program_selector();
            serial_printf("main_menu: entering the program_selector.\n");
            break;
        case 4:
            intro_animation();
            break;
        case 5:
            test_savefile();
            break;
        default:
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
    serial_printf("program_selector: loading elf\n");
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
