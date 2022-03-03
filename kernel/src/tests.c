#include "tests.h"
#include "timers.h"
#include "display.h"
#include "eeprom.h"
#include "elf.h"
#include "kernel.h"
#include "led_signals.h"
#include "savefile.h"
#include "serial_io.h"
#include "uart.h"
#include "usermode.h"
#include "gpio.h"
#include "screensaver.h"
#include "types.h"

#include <stddef.h>

#define ADDR_TEXT 0x7F004F40

typedef struct elf {
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdrs[4];
    unsigned char pad[12];
    unsigned char text[376];
    unsigned char data[4];
} elf;

elf foo = {
    /* ehdr */
    { { 0x7F,
        'E',
        'L',
        'F',
        ELFCLASS32,
        ELFDATA2LSB,
        EV_CURRENT,
        ELFOSABI_NONE,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0 },
      ET_EXEC,
      EM_MIPS_RS3_LE,
      EV_CURRENT,
      ADDR_TEXT + offsetof(elf, text),
      offsetof(elf, phdrs),
      0,
      0x70000001,
      sizeof(Elf32_Ehdr),
      sizeof(Elf32_Phdr),
      sizeof foo.phdrs / sizeof *foo.phdrs,
      sizeof(Elf32_Shdr),
      0,
      SHN_UNDEF },
    /* phdrs */
    { { PT_MIPS_ABIFLAGS,
        offsetof(elf, text) + 0x0160,
        ADDR_TEXT + offsetof(elf, text) + 0x0160,
        ADDR_TEXT + offsetof(elf, text) + 0x0160,
        24,
        24,
        PF_R,
        8 },
      { PT_LOAD, 0, 0x7F002000, 0x7F002000, 0, 8, PF_R | PF_W, 0x10 },
      { PT_LOAD,
        offsetof(elf, text),
        ADDR_TEXT + offsetof(elf, text),
        ADDR_TEXT + offsetof(elf, text),
        sizeof foo.text,
        sizeof foo.text,
        PF_R | PF_X,
        0x10 },
      { PT_LOAD,
        offsetof(elf, data),
        0x7F002008,
        ADDR_TEXT + offsetof(elf, data),
        sizeof foo.data,
        sizeof foo.data + 8,
        PF_R | PF_W,
        0x10 } },
    /* pad */
    { 0 },
    /* text */
    { 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x05, 0x20, 0xF0, 0xFF, 0xBD, 0x23, 0x08, 0x00, 0xBE,
      0xAF, 0x0C, 0x00, 0xBF, 0xAF, 0x43, 0x14, 0xC0, 0x0F, 0x00, 0x00, 0xBE, 0x23, 0x00, 0x00,
      0x00, 0x00, 0x11, 0x14, 0xC0, 0x0F, 0x00, 0x00, 0x44, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0xDD, 0x23, 0x08, 0x00, 0xBE, 0x8F, 0x0C, 0x00, 0xBF, 0x8F, 0x08, 0x00, 0xE0, 0x03,
      0x10, 0x00, 0xBD, 0x23, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xFF, 0xBD, 0x27, 0x25, 0x28, 0x80,
      0x00, 0x14, 0x00, 0xBF, 0xAF, 0x3F, 0x14, 0xC0, 0x0F, 0x01, 0x00, 0x04, 0x24, 0xE0, 0xFF,
      0xBD, 0x27, 0x02, 0x00, 0x04, 0x24, 0x1C, 0x00, 0xBF, 0xAF, 0x3F, 0x14, 0xC0, 0x0F, 0x10,
      0x00, 0xA5, 0x27, 0x1C, 0x00, 0xBF, 0x8F, 0x10, 0x00, 0xA2, 0x93, 0x08, 0x00, 0xE0, 0x03,
      0x20, 0x00, 0xBD, 0x27, 0xE0, 0xFF, 0xBD, 0x27, 0x03, 0x00, 0x04, 0x24, 0x1C, 0x00, 0xBF,
      0xAF, 0x3F, 0x14, 0xC0, 0x0F, 0x10, 0x00, 0xA5, 0x27, 0x1C, 0x00, 0xBF, 0x8F, 0x10, 0x00,
      0xA2, 0x93, 0x08, 0x00, 0xE0, 0x03, 0x20, 0x00, 0xBD, 0x27, 0x25, 0x10, 0x80, 0x00, 0x04,
      0x00, 0x04, 0x24, 0x25, 0x38, 0xC0, 0x00, 0x25, 0x30, 0xA0, 0x00, 0x3F, 0x14, 0xC0, 0x0B,
      0x25, 0x28, 0x40, 0x00, 0x25, 0x10, 0x80, 0x00, 0x05, 0x00, 0x04, 0x24, 0x25, 0x30, 0xA0,
      0x00, 0x3F, 0x14, 0xC0, 0x0B, 0x25, 0x28, 0x40, 0x00, 0x09, 0x00, 0xA0, 0x10, 0x80, 0x28,
      0x05, 0x00, 0x25, 0x10, 0x00, 0x00, 0x21, 0x28, 0x85, 0x00, 0x00, 0x00, 0x83, 0x8C, 0x04,
      0x00, 0x84, 0x24, 0xFD, 0xFF, 0x85, 0x14, 0x21, 0x10, 0x43, 0x00, 0x08, 0x00, 0xE0, 0x03,
      0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xE0, 0x03, 0x25, 0x10, 0x00, 0x00, 0x0C, 0x00, 0x00,
      0x00, 0x08, 0x00, 0xE0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F,
      0x02, 0x3C, 0xD8, 0xFF, 0xBD, 0x27, 0x04, 0x00, 0x05, 0x24, 0x10, 0x20, 0x46, 0x8C, 0x00,
      0x7F, 0x02, 0x3C, 0x10, 0x00, 0xA4, 0x27, 0x24, 0x00, 0xBF, 0xAF, 0x08, 0x20, 0x43, 0x8C,
      0x00, 0x7F, 0x02, 0x3C, 0x10, 0x00, 0xA6, 0xAF, 0x0C, 0x20, 0x42, 0x8C, 0x14, 0x00, 0xA3,
      0xAF, 0x18, 0x00, 0xA2, 0xAF, 0x0D, 0x00, 0x02, 0x24, 0x33, 0x14, 0xC0, 0x0F, 0x1C, 0x00,
      0xA2, 0xAF, 0x14, 0x00, 0xA3, 0x8F, 0x24, 0x00, 0xBF, 0x8F, 0x28, 0x00, 0xBD, 0x27, 0x08,
      0x00, 0xE0, 0x03, 0x23, 0x10, 0x43, 0x00, 0x00, 0x00, 0x20, 0x02, 0x01, 0x00, 0x00, 0x03,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00 },
    /* data */
    { 0x2A, 0x00, 0x00, 0x00 }
};

void test_elfload() {
    serial_printf("Testing elf load from file with builtin payload\n");
    eeprom_write(0x100, &foo, sizeof(foo));
    serial_printf("Paylod written to rom\n");
    EntryPoint entry = elf_load_file(0x100);
    serial_printf("Paylod loaded, entry point: %p\n", entry);
    if (entry == NULL) return;
    serial_printf("Jumping into usermode!\n");
    usermode_jump(entry, USER_DATA_END - 0x10);
    serial_printf("Uhh... this is weird, we returned from usermode_jump\n");
}

void test_savefile() {
    u8 buffer[100];
    serial_printf("Testing receive_and_store\n");
    serial_printf("Send a little-endian (LSB first) size and then a file of that size\n");
    isize sz = receive_and_store(0x10);
    serial_printf("Stored file of size %d\n", sz);
    serial_printf("Reading back first %d bytes...\n", sizeof(buffer));
    eeprom_read(0x10, buffer, sizeof(buffer));
    serial_printf("Here are bytes read:\n");
    uart_write_n(buffer, sizeof(buffer));
}

void test_serial_rom_echo() {
    u16 romaddr = 0x0;
    for (u8 i = 1;; i++) {
        serial_printf("Tell me something...\n");
        LED_DEBUG(LED_WAIT_ECHO);
        char linebuf[200];
        usize read = serial_gets_s(linebuf, 200);
        LED_DEBUG(LED_GOT_ECHO);
        serial_printf("I heard `%s`\n", linebuf);
        serial_printf("Writing it to the rom, %u bytes at addr 0x%x\n", read + 1, romaddr);
        if (eeprom_write(romaddr, linebuf, read + 1) != 0) {
            serial_printf("OOB write\n");
            continue;
        }
        serial_printf("Written, now reading back\n");
        char rombuf[200];
        if (eeprom_read(romaddr, rombuf, read + 1) != 0) {
            serial_printf("OOB read\n");
            continue;
        }
        if (serial_printf("Read back `%s`\n", rombuf) < 0) {
            serial_printf("Error formatting read-back string \n");
        }
        romaddr += read;
        if (i % 5 == 0) {
            int scan = 0;
            while (scan + 200 < romaddr) {
                eeprom_read(scan, rombuf, 200);
                uart_write_n(rombuf, 200);
                scan += 200;
            }
            eeprom_read(scan, rombuf, romaddr - scan + 1);
            serial_printf("%s\n", rombuf);
        }
    }
}

void test_display_misc() {
    /* ------------------------------------- Pretty Print ------------------------------------- */
    // Line 1
    display_string(0, 0, "ABCDEFG");

    // Line 2
    display_string(0, 8, "Inverted? No");

    // Line 3
    display_string_inverted(0, 16, "Inverted? Yes");

    int p, q, count = 0;
    for (p = 0; p < 9; p++) {
        for (q = 0; q < 9; q++) {
            if (q != count) {
                display_string((u8)(q * 8), 24, "#");
            } else {
                display_string_inverted((u8)(q * 8), 24, "#");
            }
        }
        count++;
        display_update();
        sleep(200);
    }

    // Line 4
    display_string(0, 24, "1");
    display_string_inverted(8, 24, "2");
    display_string(16, 24, "3");
    display_string_inverted(24, 24, "4");
    display_string(32, 24, "5");
    display_string_inverted(40, 24, "6");
    display_string(48, 24, "7");
    display_string_inverted(56, 24, "8");
    display_string(64, 24, "9");
    /* ---------------------------------------------------------------------------------------- */

    LED_DEBUG(LED_SCREEN_PRINT);
    display_update();
    LED_DEBUG(LED_SCREEN_FLUSH);
}

void test_sleep() {
    usize ms = 2358;
    serial_printf("Global time: %ums\n", get_time());
    serial_printf("Sleeping for %ums\n", ms);
    sleep(ms);
    serial_printf("Woke up!\n");
    serial_printf("Global time: %ums\n", get_time());
}

void test_menu(void) {
    int MENU_LEN = 3;
    const char *menuItems[] = {
        "#0: HELP", "#1: SCRNSVR", "#2: INPUT"
    };
    help_menu(); // Start the display on the help menu
    while (getsws() & 0x1) {
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
    }
    serial_printf("Exited the menu loop.");
}

void get_input(void) {
    display_clear();
    display_string(8, 12, "Awaiting input");
    display_update();
    serial_printf("Awaiting input for the display.\n");
    char linebuf[200];
    usize read = serial_gets_s(linebuf, 200);
    
    display_clear();
    display_string(0, 0, "I recieved:");
    display_string(0, 8, linebuf);
    display_string(0, 24, "BTN4 to exit.");
    display_update();

    serial_printf("'%s' was sent to the display.\n", linebuf);
    int exit = 1;
    while (exit) {
        if (getbtns() & 0x8) {
            while(getbtns() & 0x8) {
            }
            break;
        }
    }
}

void test_syscall_display(void) {
    
    sys_screen_print(0, "Regular",  0x0);
    sys_screen_print(1, "Regular", 0x0);
    sys_screen_print(1, "Inverted", 0x1);
    sys_screen_print(3, " ", 0x1);
}