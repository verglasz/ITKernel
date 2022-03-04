#include "tests.h"

#include "display.h"
#include "eeprom.h"
#include "elf.h"
#include "gpio.h"
#include "interrupts.h"
#include "kernel.h"
#include "led_signals.h"
#include "savefile.h"
#include "screensaver.h"
#include "serial_io.h"
#include "timers.h"
#include "types.h"
#include "uart.h"
#include "usermode.h"
#include "ustar.h"

#include <pic32mx.h>>

// usermode
#define ADDR_TEXT 0x7F004F40

typedef struct elf {
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdrs[4];
    unsigned char pad[12];
    unsigned char text[376];
    unsigned char data[4];
} elf;

elf foo = {
    // ehdr
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
    // phdrs
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
    // pad
    { 0 },
    // text
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
    // data
    { 0x2A, 0x00, 0x00, 0x00 }
};
/*/

// Kernelmode
#define ADDR_TEXT 0x7F004F40

typedef struct elf {
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdrs[4];
    unsigned char pad[12];
    unsigned char text[2232];
    unsigned char data[4];
} elf;

elf foo = {
    // ehdr
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
      EM_MIPS,
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
    // phdrs
    { { PT_MIPS_ABIFLAGS,
        offsetof(elf, text) + 0x08A0,
        ADDR_TEXT + offsetof(elf, text) + 0x08A0,
        ADDR_TEXT + offsetof(elf, text) + 0x08A0,
        sizeof foo.text / 93,
        sizeof foo.text / 93,
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
    // pad
    { 0 },
    // text
    { 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x05, 0x20, 0xF0, 0xFF, 0xBD, 0x23, 0x08, 0x00, 0xBE,
      0xAF, 0x0C, 0x00, 0xBF, 0xAF, 0x0D, 0x16, 0xC0, 0x0F, 0x00, 0x00, 0xBE, 0x23, 0x00, 0x00,
      0x00, 0x00, 0x2D, 0x14, 0xC0, 0x0F, 0x00, 0x00, 0x44, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0xDD, 0x23, 0x08, 0x00, 0xBE, 0x8F, 0x0C, 0x00, 0xBF, 0x8F, 0x08, 0x00, 0xE0, 0x03,
      0x10, 0x00, 0xBD, 0x23, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF7, 0xBD, 0x27, 0x24, 0x08, 0xA2,
      0x27, 0x24, 0x08, 0xA5, 0xAF, 0x00, 0x08, 0x05, 0x24, 0x28, 0x08, 0xA6, 0xAF, 0x25, 0x30,
      0x80, 0x00, 0x10, 0x00, 0xA4, 0x27, 0x2C, 0x08, 0xA7, 0xAF, 0x25, 0x38, 0x40, 0x00, 0x1C,
      0x08, 0xBF, 0xAF, 0x18, 0x08, 0xB0, 0xAF, 0xD0, 0x15, 0xC0, 0x0F, 0x10, 0x08, 0xA2, 0xAF,
      0x07, 0x00, 0x40, 0x04, 0xFF, 0xFF, 0x10, 0x24, 0x25, 0x80, 0x40, 0x00, 0x01, 0x08, 0x42,
      0x28, 0x08, 0x00, 0x40, 0x10, 0x10, 0x00, 0xA4, 0x27, 0x4A, 0x14, 0xC0, 0x0F, 0x25, 0x28,
      0x00, 0x02, 0x1C, 0x08, 0xBF, 0x8F, 0x25, 0x10, 0x00, 0x02, 0x18, 0x08, 0xB0, 0x8F, 0x08,
      0x00, 0xE0, 0x03, 0x20, 0x08, 0xBD, 0x27, 0xFA, 0xFF, 0x00, 0x10, 0xFE, 0xFF, 0x10, 0x24,
      0xE8, 0xFF, 0xBD, 0x27, 0x25, 0x28, 0x80, 0x00, 0x14, 0x00, 0xBF, 0xAF, 0x5B, 0x14, 0xC0,
      0x0F, 0x01, 0x00, 0x04, 0x24, 0xE0, 0xFF, 0xBD, 0x27, 0x02, 0x00, 0x04, 0x24, 0x1C, 0x00,
      0xBF, 0xAF, 0x5B, 0x14, 0xC0, 0x0F, 0x10, 0x00, 0xA5, 0x27, 0x1C, 0x00, 0xBF, 0x8F, 0x10,
      0x00, 0xA2, 0x93, 0x08, 0x00, 0xE0, 0x03, 0x20, 0x00, 0xBD, 0x27, 0xE0, 0xFF, 0xBD, 0x27,
      0x03, 0x00, 0x04, 0x24, 0x1C, 0x00, 0xBF, 0xAF, 0x5B, 0x14, 0xC0, 0x0F, 0x10, 0x00, 0xA5,
      0x27, 0x1C, 0x00, 0xBF, 0x8F, 0x10, 0x00, 0xA2, 0x93, 0x08, 0x00, 0xE0, 0x03, 0x20, 0x00,
      0xBD, 0x27, 0x25, 0x10, 0x80, 0x00, 0x04, 0x00, 0x04, 0x24, 0x25, 0x38, 0xC0, 0x00, 0x25,
      0x30, 0xA0, 0x00, 0x5B, 0x14, 0xC0, 0x0B, 0x25, 0x28, 0x40, 0x00, 0x25, 0x10, 0x80, 0x00,
      0x05, 0x00, 0x04, 0x24, 0x25, 0x30, 0xA0, 0x00, 0x5B, 0x14, 0xC0, 0x0B, 0x25, 0x28, 0x40,
      0x00, 0x09, 0x00, 0xA0, 0x10, 0x80, 0x28, 0x05, 0x00, 0x25, 0x10, 0x00, 0x00, 0x21, 0x28,
      0x85, 0x00, 0x00, 0x00, 0x83, 0x8C, 0x04, 0x00, 0x84, 0x24, 0xFD, 0xFF, 0x85, 0x14, 0x21,
      0x10, 0x43, 0x00, 0x08, 0x00, 0xE0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xE0, 0x03,
      0x25, 0x10, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x08, 0x00, 0xE0, 0x03, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0xA6, 0x8C, 0x01, 0x00, 0x02, 0x24, 0x07, 0x00,
      0xA7, 0x80, 0xF4, 0x01, 0x40, 0x00, 0x1B, 0x00, 0x82, 0x00, 0x12, 0x18, 0x00, 0x00, 0x2B,
      0x18, 0x67, 0x00, 0x07, 0x00, 0x60, 0x10, 0x37, 0x00, 0x09, 0x24, 0x25, 0x38, 0x00, 0x00,
      0x57, 0x00, 0x0A, 0x24, 0x05, 0x00, 0x40, 0x14, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xE0,
      0x03, 0x00, 0x00, 0xC0, 0xA0, 0xF3, 0xFF, 0x00, 0x10, 0x02, 0x10, 0x47, 0x70, 0xF4, 0x01,
      0x40, 0x00, 0x1B, 0x00, 0x82, 0x00, 0x07, 0x00, 0xA3, 0x80, 0x10, 0x20, 0x00, 0x00, 0x12,
      0x40, 0x00, 0x00, 0xF4, 0x01, 0x60, 0x00, 0x1B, 0x00, 0x43, 0x00, 0x07, 0x00, 0xE0, 0x14,
      0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x1D, 0x2B, 0x18, 0x43, 0x00, 0x0E, 0x00, 0x60,
      0x14, 0x00, 0x00, 0x00, 0x00, 0xEC, 0xFF, 0x00, 0x10, 0x12, 0x10, 0x00, 0x00, 0x0A, 0x00,
      0x03, 0x29, 0x04, 0x00, 0x60, 0x14, 0x30, 0x00, 0x02, 0x24, 0x08, 0x00, 0xA3, 0x80, 0x25,
      0x10, 0x40, 0x01, 0x0B, 0x10, 0x23, 0x01, 0x21, 0x10, 0x48, 0x00, 0x01, 0x00, 0xE7, 0x24,
      0x00, 0x00, 0xC2, 0xA0, 0xF4, 0xFF, 0x00, 0x10, 0x01, 0x00, 0xC6, 0x24, 0xFA, 0xFF, 0x00,
      0x10, 0x30, 0x00, 0x02, 0x24, 0xE0, 0xFF, 0xBD, 0x27, 0x00, 0x00, 0x82, 0x8C, 0x10, 0x00,
      0xA5, 0xA3, 0x1C, 0x00, 0xBF, 0xAF, 0x00, 0x00, 0x42, 0x8C, 0x05, 0x00, 0x40, 0x14, 0x01,
      0x00, 0x06, 0x24, 0x25, 0x10, 0x00, 0x00, 0x1C, 0x00, 0xBF, 0x8F, 0x08, 0x00, 0xE0, 0x03,
      0x20, 0x00, 0xBD, 0x27, 0x09, 0xF8, 0x40, 0x00, 0x10, 0x00, 0xA5, 0x27, 0x01, 0x00, 0x03,
      0x24, 0xF8, 0xFF, 0x43, 0x14, 0x01, 0x00, 0x02, 0x24, 0xF8, 0xFF, 0x00, 0x10, 0x1C, 0x00,
      0xBF, 0x8F, 0xD8, 0xFF, 0xBD, 0x27, 0x0C, 0x00, 0xA2, 0x8C, 0x10, 0x00, 0xB0, 0xAF, 0x20,
      0x00, 0xB4, 0xAF, 0x25, 0xA0, 0xA0, 0x00, 0x18, 0x00, 0xB2, 0xAF, 0x25, 0x90, 0x80, 0x00,
      0x24, 0x00, 0xBF, 0xAF, 0x1C, 0x00, 0xB3, 0xAF, 0x14, 0x00, 0xB1, 0xAF, 0x00, 0x00, 0xB0,
      0x8C, 0x00, 0x00, 0x43, 0x80, 0x39, 0x00, 0x60, 0x14, 0x01, 0x00, 0x42, 0x24, 0x05, 0x00,
      0x82, 0x82, 0x3A, 0x00, 0x40, 0x14, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x82, 0x82, 0x05,
      0x00, 0x40, 0x10, 0x10, 0x00, 0x03, 0x24, 0x07, 0x00, 0x82, 0x82, 0x36, 0x00, 0x43, 0x14,
      0x08, 0x00, 0x03, 0x24, 0xFE, 0xFF, 0x10, 0x26, 0x04, 0x00, 0x82, 0x82, 0x25, 0x88, 0x00,
      0x00, 0x39, 0x00, 0x40, 0x10, 0x25, 0x98, 0x00, 0x02, 0x05, 0x00, 0x85, 0x82, 0x04, 0x00,
      0xA0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x8C, 0x14, 0xC0, 0x0F, 0x25, 0x20, 0x40, 0x02, 0x21,
      0x88, 0x22, 0x02, 0x06, 0x00, 0x82, 0x82, 0x10, 0x00, 0x40, 0x10, 0x10, 0x00, 0x03, 0x24,
      0x07, 0x00, 0x82, 0x82, 0x34, 0x00, 0x43, 0x14, 0x08, 0x00, 0x03, 0x24, 0x30, 0x00, 0x05,
      0x24, 0x8C, 0x14, 0xC0, 0x0F, 0x25, 0x20, 0x40, 0x02, 0x58, 0x00, 0x03, 0x24, 0x08, 0x00,
      0x85, 0x82, 0x21, 0x88, 0x51, 0x00, 0x78, 0x00, 0x02, 0x24, 0x0B, 0x10, 0x65, 0x00, 0x25,
      0x28, 0x40, 0x00, 0x8C, 0x14, 0xC0, 0x0F, 0x25, 0x20, 0x40, 0x02, 0x21, 0x88, 0x22, 0x02,
      0x04, 0x00, 0x82, 0x82, 0x03, 0x00, 0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00,
      0x1E, 0x30, 0x00, 0x05, 0x24, 0x0C, 0x00, 0x90, 0x8E, 0x00, 0x00, 0x05, 0x82, 0x28, 0x00,
      0xA0, 0x14, 0x01, 0x00, 0x10, 0x26, 0x24, 0x00, 0xBF, 0x8F, 0x25, 0x10, 0x20, 0x02, 0x20,
      0x00, 0xB4, 0x8F, 0x1C, 0x00, 0xB3, 0x8F, 0x18, 0x00, 0xB2, 0x8F, 0x14, 0x00, 0xB1, 0x8F,
      0x10, 0x00, 0xB0, 0x8F, 0x08, 0x00, 0xE0, 0x03, 0x28, 0x00, 0xBD, 0x27, 0xC7, 0xFF, 0x00,
      0x1A, 0x00, 0x00, 0x00, 0x00, 0xC2, 0xFF, 0x00, 0x10, 0xFF, 0xFF, 0x10, 0x26, 0xC6, 0xFF,
      0x00, 0x10, 0xFF, 0xFF, 0x10, 0x26, 0xCB, 0xFF, 0x43, 0x14, 0x00, 0x00, 0x00, 0x00, 0xC9,
      0xFF, 0x00, 0x10, 0xFF, 0xFF, 0x10, 0x26, 0x8C, 0x14, 0xC0, 0x0F, 0x25, 0x20, 0x40, 0x02,
      0xFF, 0xFF, 0x73, 0x26, 0x21, 0x88, 0x22, 0x02, 0xFB, 0xFF, 0x60, 0x1E, 0x20, 0x00, 0x05,
      0x24, 0x00, 0x00, 0x03, 0x2A, 0xFF, 0xFF, 0x02, 0x26, 0x0B, 0x80, 0x03, 0x00, 0xC2, 0xFF,
      0x00, 0x10, 0x23, 0x80, 0x50, 0x00, 0xD8, 0xFF, 0x43, 0x14, 0x30, 0x00, 0x05, 0x24, 0xD3,
      0xFF, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x8C, 0x14, 0xC0, 0x0F, 0x25, 0x20, 0x40, 0x02,
      0xFF, 0xFF, 0x10, 0x26, 0xD4, 0xFF, 0x00, 0x10, 0x21, 0x88, 0x22, 0x02, 0x8C, 0x14, 0xC0,
      0x0F, 0x25, 0x20, 0x40, 0x02, 0xD3, 0xFF, 0x00, 0x10, 0x21, 0x88, 0x22, 0x02, 0xB0, 0xFF,
      0xBD, 0x27, 0x20, 0x00, 0xA2, 0x27, 0x40, 0x00, 0xB4, 0xAF, 0x25, 0xA0, 0x00, 0x00, 0x3C,
      0x00, 0xB3, 0xAF, 0x23, 0x00, 0x13, 0x24, 0x38, 0x00, 0xB2, 0xAF, 0x0A, 0x00, 0x12, 0x24,
      0x44, 0x00, 0xB5, 0xAF, 0x25, 0xA8, 0xC0, 0x00, 0x34, 0x00, 0xB1, 0xAF, 0x25, 0x88, 0x80,
      0x00, 0x4C, 0x00, 0xBF, 0xAF, 0x48, 0x00, 0xB6, 0xAF, 0x30, 0x00, 0xB0, 0xAF, 0x1C, 0x00,
      0xA2, 0xAF, 0x01, 0x00, 0xB0, 0x24, 0x00, 0x00, 0xA5, 0x80, 0x0C, 0x00, 0xA0, 0x14, 0x25,
      0x00, 0x02, 0x24, 0x4C, 0x00, 0xBF, 0x8F, 0x25, 0x10, 0x80, 0x02, 0x48, 0x00, 0xB6, 0x8F,
      0x44, 0x00, 0xB5, 0x8F, 0x40, 0x00, 0xB4, 0x8F, 0x3C, 0x00, 0xB3, 0x8F, 0x38, 0x00, 0xB2,
      0x8F, 0x34, 0x00, 0xB1, 0x8F, 0x30, 0x00, 0xB0, 0x8F, 0x08, 0x00, 0xE0, 0x03, 0x50, 0x00,
      0xBD, 0x27, 0x05, 0x00, 0xA2, 0x10, 0x25, 0x20, 0x00, 0x00, 0x8C, 0x14, 0xC0, 0x0F, 0x25,
      0x20, 0x20, 0x02, 0x47, 0x00, 0x00, 0x10, 0x21, 0xA0, 0x82, 0x02, 0x10, 0x00, 0xA0, 0xAF,
      0x25, 0x18, 0x00, 0x00, 0x14, 0x00, 0xA0, 0xA7, 0x25, 0x30, 0x00, 0x00, 0x16, 0x00, 0xA0,
      0xA3, 0x25, 0x28, 0x00, 0x00, 0x30, 0x00, 0x07, 0x24, 0x00, 0x00, 0x02, 0x82, 0x05, 0x00,
      0x40, 0x10, 0x01, 0x00, 0x10, 0x26, 0x10, 0x00, 0x53, 0x10, 0x00, 0x00, 0x00, 0x00, 0x11,
      0x00, 0x47, 0x10, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xC0, 0x10, 0x00, 0x00, 0x00, 0x00,
      0x14, 0x00, 0xA5, 0xA3, 0x02, 0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0xA3,
      0xA3, 0xD0, 0xFF, 0x43, 0x24, 0xFF, 0x00, 0x63, 0x30, 0x0A, 0x00, 0x63, 0x2C, 0x1E, 0x00,
      0x60, 0x10, 0x6C, 0x00, 0x03, 0x24, 0x0B, 0x00, 0x00, 0x10, 0x25, 0x20, 0x00, 0x00, 0x01,
      0x00, 0x04, 0x24, 0xEA, 0xFF, 0x00, 0x10, 0x01, 0x00, 0x03, 0x24, 0x01, 0x00, 0x06, 0x24,
      0xE7, 0xFF, 0x00, 0x10, 0x01, 0x00, 0x05, 0x24, 0x02, 0x10, 0x92, 0x70, 0x01, 0x00, 0x10,
      0x26, 0x21, 0x20, 0x43, 0x00, 0xFF, 0xFF, 0x02, 0x82, 0xFF, 0x00, 0x43, 0x30, 0xD0, 0xFF,
      0x65, 0x24, 0x0A, 0x00, 0xA5, 0x2C, 0x04, 0x00, 0xA0, 0x10, 0x9F, 0xFF, 0x65, 0x24, 0xD0,
      0xFF, 0x42, 0x24, 0xF5, 0xFF, 0x00, 0x10, 0x20, 0x1C, 0x02, 0x7C, 0x06, 0x00, 0xA5, 0x2C,
      0x19, 0x00, 0xA0, 0x10, 0xBF, 0xFF, 0x63, 0x24, 0xA9, 0xFF, 0x43, 0x24, 0x20, 0x1C, 0x03,
      0x7C, 0xEE, 0xFF, 0x72, 0x10, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xA4, 0xAF, 0x6C, 0x00,
      0x03, 0x24, 0x03, 0x00, 0x43, 0x14, 0x69, 0x00, 0x03, 0x24, 0x00, 0x00, 0x02, 0x82, 0x01,
      0x00, 0x10, 0x26, 0x17, 0x00, 0x43, 0x10, 0x6A, 0x00, 0x43, 0x28, 0x25, 0x00, 0x60, 0x10,
      0x58, 0x00, 0x03, 0x24, 0x31, 0x00, 0x43, 0x10, 0x59, 0x00, 0x43, 0x28, 0x0C, 0x00, 0x60,
      0x10, 0x63, 0x00, 0x03, 0x24, 0xAC, 0xFF, 0x40, 0x10, 0x25, 0x00, 0x03, 0x24, 0xB7, 0xFF,
      0x43, 0x10, 0x25, 0x00, 0x05, 0x24, 0xA4, 0xFF, 0x00, 0x10, 0x25, 0x28, 0x00, 0x02, 0x06,
      0x00, 0x63, 0x2C, 0xEA, 0xFF, 0x60, 0x10, 0xC9, 0xFF, 0x43, 0x24, 0xE6, 0xFF, 0x00, 0x10,
      0x20, 0x1C, 0x03, 0x7C, 0x2E, 0x00, 0x43, 0x10, 0x25, 0x20, 0x20, 0x02, 0x64, 0x00, 0x03,
      0x24, 0x9A, 0xFF, 0x43, 0x14, 0x25, 0x28, 0x00, 0x02, 0x00, 0x00, 0xA4, 0x8E, 0x0A, 0x00,
      0x02, 0x24, 0x04, 0x00, 0xB6, 0x26, 0x04, 0x00, 0x81, 0x04, 0x17, 0x00, 0xA2, 0xA3, 0x2D,
      0x00, 0x02, 0x24, 0x23, 0x20, 0x04, 0x00, 0x15, 0x00, 0xA2, 0xA3, 0x10, 0x00, 0xA5, 0x27,
      0x5F, 0x14, 0xC0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x9E, 0x14, 0xC0, 0x0F, 0x25, 0x20, 0x20,
      0x02, 0x21, 0xA0, 0x82, 0x02, 0xE5, 0xFF, 0x00, 0x10, 0x25, 0xA8, 0xC0, 0x02, 0x73, 0x00,
      0x03, 0x24, 0x1D, 0x00, 0x43, 0x10, 0x74, 0x00, 0x43, 0x28, 0x06, 0x00, 0x60, 0x10, 0x75,
      0x00, 0x03, 0x24, 0x6F, 0x00, 0x03, 0x24, 0xDD, 0xFF, 0x43, 0x14, 0x08, 0x00, 0x02, 0x24,
      0x0A, 0x00, 0x00, 0x10, 0x17, 0x00, 0xA2, 0xA3, 0x0C, 0x00, 0x43, 0x10, 0x78, 0x00, 0x03,
      0x24, 0xD7, 0xFF, 0x43, 0x14, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x24, 0x58, 0x00,
      0x42, 0x38, 0x01, 0x00, 0x42, 0x2C, 0x17, 0x00, 0xA3, 0xA3, 0x18, 0x00, 0xA2, 0xA3, 0x00,
      0x00, 0xA4, 0x8E, 0x04, 0x00, 0xB6, 0x26, 0xE3, 0xFF, 0x00, 0x10, 0x10, 0x00, 0xA5, 0x27,
      0x0A, 0x00, 0x02, 0x24, 0xFA, 0xFF, 0x00, 0x10, 0x17, 0x00, 0xA2, 0xA3, 0x00, 0x00, 0xA5,
      0x82, 0x8C, 0x14, 0xC0, 0x0F, 0x04, 0x00, 0xB6, 0x26, 0xE0, 0xFF, 0x00, 0x10, 0x21, 0xA0,
      0x82, 0x02, 0x00, 0x00, 0xA2, 0x8E, 0x10, 0x00, 0xA5, 0x27, 0x25, 0x20, 0x20, 0x02, 0x04,
      0x00, 0xB6, 0x26, 0x9E, 0x14, 0xC0, 0x0F, 0x1C, 0x00, 0xA2, 0xAF, 0x21, 0xA0, 0x82, 0x02,
      0x20, 0x00, 0xA2, 0x27, 0xD6, 0xFF, 0x00, 0x10, 0x1C, 0x00, 0xA2, 0xAF, 0x06, 0x15, 0xC0,
      0x0B, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0xBD, 0x27, 0x28, 0x00, 0xA6, 0xAF, 0x28, 0x00,
      0xA6, 0x27, 0x1C, 0x00, 0xBF, 0xAF, 0x2C, 0x00, 0xA7, 0xAF, 0x06, 0x15, 0xC0, 0x0F, 0x10,
      0x00, 0xA6, 0xAF, 0x1C, 0x00, 0xBF, 0x8F, 0x08, 0x00, 0xE0, 0x03, 0x20, 0x00, 0xBD, 0x27,
      0xE0, 0xFF, 0xBD, 0x27, 0x00, 0x7F, 0x02, 0x3C, 0x24, 0x00, 0xA5, 0xAF, 0x25, 0x28, 0x80,
      0x00, 0x28, 0x00, 0xA6, 0xAF, 0x24, 0x00, 0xA6, 0x27, 0x1C, 0x00, 0xBF, 0xAF, 0x2C, 0x00,
      0xA7, 0xAF, 0x10, 0x00, 0xA6, 0xAF, 0x06, 0x15, 0xC0, 0x0F, 0x94, 0x58, 0x44, 0x8C, 0x1C,
      0x00, 0xBF, 0x8F, 0x08, 0x00, 0xE0, 0x03, 0x20, 0x00, 0xBD, 0x27, 0xD0, 0xFF, 0xBD, 0x27,
      0x24, 0x00, 0xB0, 0xAF, 0x25, 0x80, 0xC0, 0x00, 0xFF, 0xFF, 0xA6, 0x24, 0x2C, 0x00, 0xBF,
      0xAF, 0x25, 0x28, 0x80, 0x00, 0x28, 0x00, 0xB1, 0xAF, 0x10, 0x00, 0xA4, 0x27, 0x05, 0x16,
      0xC0, 0x0F, 0x25, 0x88, 0xE0, 0x00, 0x25, 0x30, 0x20, 0x02, 0x25, 0x28, 0x00, 0x02, 0x06,
      0x15, 0xC0, 0x0F, 0x25, 0x20, 0x40, 0x00, 0x14, 0x00, 0xA2, 0x8F, 0x00, 0x00, 0x40, 0xA0,
      0x2C, 0x00, 0xBF, 0x8F, 0x18, 0x00, 0xA2, 0x8F, 0x28, 0x00, 0xB1, 0x8F, 0x24, 0x00, 0xB0,
      0x8F, 0x08, 0x00, 0xE0, 0x03, 0x30, 0x00, 0xBD, 0x27, 0xE0, 0xFF, 0xBD, 0x27, 0x2C, 0x00,
      0xA7, 0xAF, 0x2C, 0x00, 0xA7, 0x27, 0x1C, 0x00, 0xBF, 0xAF, 0xD0, 0x15, 0xC0, 0x0F, 0x10,
      0x00, 0xA7, 0xAF, 0x1C, 0x00, 0xBF, 0x8F, 0x08, 0x00, 0xE0, 0x03, 0x20, 0x00, 0xBD, 0x27,
      0x25, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x07, 0x24, 0xFF, 0xFF, 0xC6, 0x24, 0x03, 0x00, 0xC7,
      0x14, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xE0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
      0x83, 0x8C, 0x0C, 0x00, 0x88, 0x8C, 0x01, 0x00, 0x63, 0x24, 0x08, 0x00, 0x83, 0xAC, 0x2B,
      0x18, 0x03, 0x01, 0xF5, 0xFF, 0x60, 0x14, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x83, 0x8C,
      0x01, 0x00, 0x42, 0x24, 0x01, 0x00, 0xA5, 0x24, 0x01, 0x00, 0x68, 0x24, 0x04, 0x00, 0x88,
      0xAC, 0xFF, 0xFF, 0xA8, 0x80, 0xED, 0xFF, 0x00, 0x10, 0x00, 0x00, 0x68, 0xA0, 0x00, 0x7F,
      0x03, 0x3C, 0x04, 0x00, 0x85, 0xAC, 0x25, 0x10, 0x80, 0x00, 0x08, 0x00, 0x80, 0xAC, 0x98,
      0x58, 0x63, 0x24, 0x0C, 0x00, 0x86, 0xAC, 0x08, 0x00, 0xE0, 0x03, 0x00, 0x00, 0x83, 0xAC,
      0xD8, 0xFF, 0xBD, 0x27, 0x88, 0xBF, 0x02, 0x3C, 0xFF, 0x00, 0x03, 0x24, 0x24, 0x00, 0xBF,
      0xAF, 0x10, 0x00, 0xA4, 0x27, 0x10, 0x61, 0x43, 0xAC, 0x00, 0x7F, 0x02, 0x3C, 0x04, 0x00,
      0x05, 0x24, 0x10, 0x20, 0x46, 0x8C, 0x00, 0x7F, 0x02, 0x3C, 0x08, 0x20, 0x43, 0x8C, 0x00,
      0x7F, 0x02, 0x3C, 0x10, 0x00, 0xA6, 0xAF, 0x0C, 0x20, 0x42, 0x8C, 0x14, 0x00, 0xA3, 0xAF,
      0x18, 0x00, 0xA2, 0xAF, 0x0D, 0x00, 0x02, 0x24, 0x4F, 0x14, 0xC0, 0x0F, 0x1C, 0x00, 0xA2,
      0xAF, 0x14, 0x00, 0xA3, 0x8F, 0x24, 0x00, 0xBF, 0x8F, 0x28, 0x00, 0xBD, 0x27, 0x08, 0x00,
      0xE0, 0x03, 0x23, 0x10, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBC, 0x57, 0x00, 0x7F, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    // data
    { 0x2A, 0x00, 0x00, 0x00 }
};
*/

void test_elfload() {
    serial_printf("Testing elf load from file with builtin payload\n");
    eeprom_write(0x100, &foo, sizeof(foo));
    serial_printf("Paylod written to rom\n");
    EntryPoint entry = elf_load_file(0x100);
    serial_printf("Paylod loaded, entry point: 0x%x\n", entry);
    if (entry == NULL) return;
    serial_printf("Jumping into usermode!\n");
    LED_DEBUG(0);
    usermode_jump(entry, USER_DATA_END - 0x10);
    serial_printf("Uhh... this is weird, we returned from usermode_jump\n");
}

void test_savefile() {
    u8 buffer[100];
    u16 addr = 0x100;
    serial_printf("Testing receive_and_store\n");
    serial_printf("Send a little-endian (LSB first) size and then a file of that size\n");
    isize sz = receive_and_store(addr);
    serial_printf("Stored file of size %d\n", sz);
    serial_printf("Press button to read back file\n");
    serial_printf("Here are the bytes read:\n");
    while (getbtns() == 0) {}
    while (sz > sizeof(buffer)) {
        eeprom_read(addr, buffer, sizeof(buffer));
        uart_write_n(buffer, sizeof(buffer));
        sz -= sizeof(buffer);
        addr += sizeof(buffer);
    }
    eeprom_read(addr, buffer, sz);
    uart_write_n(buffer, sz);
    while (getbtns() == 0) { PORTE = 0xff; }
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

void test_eeprom_elfwrite() {
    serial_printf("Testing elf load from file with builtin payload\n");
    int ret = eeprom_write(0x100, &foo, sizeof(foo));
    serial_printf("Paylod written to rom, returned %d\n", ret);
    struct elf local;
    ret = eeprom_read(0x100, &local, sizeof(foo));
    serial_printf("Paylod read from rom, returned %d\n", ret);
    ret = __builtin_memcmp(&foo, &local, sizeof(foo));
    serial_printf("Memcmp returned %d\n", ret);
    serial_printf("Dumping read contents:\n");
    uart_write_n(&local, sizeof(foo));
}

void test_sleep(usize ms) {
    serial_printf("Global time: %ums\n", (usize)get_time());
    serial_printf("Sleeping for %ums\n", ms);
    usize tm = (usize)get_time();
    sleep(ms);
    usize at = (usize)get_time();
    serial_printf("Woke up!\n");
    serial_printf("Global time: %ums\n", (usize)get_time());
    serial_printf("Difference in global time around sleep: %d\n", at - tm);
}

void test_cause_int() {
    serial_printf("Jumping to interrupt handler manually\n");
    _cause_interrupt();
    serial_printf("Back from the handler, hopefully\n");
}

void test_cause_t4int() {
    volatile register i = 0;
    serial_printf("Causing interrupt to set epc, current time: %d\n", get_time());
    _cause_interrupt(200);
    serial_printf("Back from fake interrupt, current time: %d\n", get_time());
    if (i == 0) {
        serial_printf(
            "Setting flag and jumping to t4 interrupt handler manually, current time: %d\n",
            get_time());
        i = 1;
        IFSSET(0) = 1u << PIC32_IRQ_T4;
        __vector_16();
    }
    serial_printf("Back from the handler, hopefully, current time: %d\n", get_time());
}

void test_magic_shit(isize ret, Context *ctx) {
    test_sleep(222);
    test_cause_t4int();

    if (ret == 0) {
        serial_printf("Longjmping back\n");
        longjmp(ctx, 42);
    }
    serial_printf("Enabling t4 interrupt\n");
    sleep(1000);
    serial_printf("T4 interrupt enabled\n");
    test_sleep(288);
}

void test_save_elf_run() {
    u16 addr = 0x100;
    serial_printf("Send an elf program, i will run it!\n");
    serial_printf("Send a little-endian (LSB first) size and then an elf file of that size\n");
    isize sz = receive_and_store(addr);
    serial_printf("Stored file of size %d\n", sz);
    serial_printf("Loading elf file in memory\n");
    EntryPoint entry = elf_load_file(addr);
    serial_printf("Paylod loaded, entry point: 0x%x\n", entry);
    if (entry == NULL) return;
    serial_printf("Jumping into usermode!\n");
    LED_DEBUG(0);
    usermode_jump(entry, USER_DATA_END - 0x10);
    serial_printf("Uhh... this is weird, we returned from usermode_jump\n");
}

void test_ustar_find() {
    u16 start_addr = 0x100;
    u16 addr;
    char filename[] = "textfile.txt";
    usize size;
    isize ret = ustar_find_file(start_addr, "random", &addr, &size);
    serial_printf(
        "Find file for `random` returned %d, giving addr: 0x%x and size:%d\n",
        ret,
        (usize)addr,
        size);
    ret = ustar_find_file(start_addr, filename, &addr, &size);
    serial_printf("Find file returned %d, giving addr: 0x%x and size:%d\n", ret, (usize)addr, size);
    if (ret != 0) return;
    serial_printf("reading file:\n");
    char buffer[100];
    while (getbtns() == 0) {}
    while (size > sizeof(buffer)) {
        eeprom_read(addr, buffer, sizeof(buffer));
        uart_write_n(buffer, sizeof(buffer));
        size -= sizeof(buffer);
        addr += sizeof(buffer);
    }
    eeprom_read(addr, buffer, size);
    uart_write_n(buffer, size);
    serial_printf("\nThat was it!\n");
}

void test_save_ustar_find() {
    u16 addr = 0x100;
    serial_printf("Send a little-endian (LSB first) size containing `textfile.txt` and then an "
                  "ustar file of that size\n");
    isize sz = receive_and_store(addr);
    serial_printf("Stored file of size %d\n", sz);
    test_ustar_find();
    while (getbtns() == 0) { PORTE = 0xff; }
}

void test_menu(void) {
    int MENU_LEN = 3;
    const char *menuItems[] = { "#0: HELP", "#1: SCRNSVR", "#2: INPUT" };
    help_menu(); // Start the display on the help menu
    while (getsws() & 0x1) {
        int resp = display_menu(menuItems, MENU_LEN);

        if (resp == 0) { help_menu(); }

        if (resp == 1) { screensaver(); }

        if (resp == 2) { test_get_input(); }
    }
    serial_printf("Exited the menu loop.");
}

void test_get_input(void) {
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
            while (getbtns() & 0x8) {}
            break;
        }
    }
}

void test_syscall_display(void) {
    sys_screen_print(0, "Regular", 0x0);
    sys_screen_print(1, "Regular", 0x0);
    sys_screen_print(1, "Inverted", 0x1);
    sys_screen_print(3, " ", 0x1);
}
