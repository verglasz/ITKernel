#include "display.h"
#include "elf.h"
#include "kernel.h"
#include "menu.h"
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

        if (resp == 0) { help_menu(); }

        if (resp == 1) { screensaver(); }

        if (resp == 2) { test_get_input(); }

        if (resp == 3) { program_selector(); }
    }
}

int program_selector() {
    FileInfo fbuf[MAX_FILES];
    isize ret = ustar_list_files(fbuf, MAX_FILES);
    serial_printf("program_selector: ustar_list_files returned %d\n", ret);
    if (ret <= 0) { return -1; }
    char *fnames[MAX_FILES];
    for (int i = 0; i < ret; i++) { fnames[i] = fbuf[i].filename; }

    serial_printf("program_selector: showing menu\n");
    int selection = display_menu(fnames, ret);
    serial_printf("program_selector: selected %d\n", selection);
    if (selection == -1) return -1;
    serial_printf("program_selector: loading elf\n");
    EntryPoint entry = elf_load_file(fbuf[selection].rom_addr);
    serial_printf("program_selector: elf loaded, entry: 0x%x\n", entry);
    if (entry == NULL) return -2;
    if (setjmp(&kernel_ctx) == 0) {
        // fingers crossed
        serial_printf("program_selector: jumping to user program\n");
        usermode_jump(entry, USER_DATA_END);
    } else {
        serial_printf("program_selector: user program exited\n");
        int retval = get_retval(&kernel_ctx);
        display_clear();
        char buf[24];
        __builtin_sprintf(buf, "returned: %d", retval);
        display_string(0, 0, "Program exited");
        display_string(0, 8, buf);
        return 0;
    }
}
