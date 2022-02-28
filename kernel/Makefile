# env setup
TOOLCHAIN_INSTALL_DIR=/opt/mcb32tools
TARGET=mipsel-mcb32-elf-

PREFIX=$(TOOLCHAIN_INSTALL_DIR)/bin/$(TARGET)

AR=$(PREFIX)ar
AS=$(PREFIX)as
LD=$(PREFIX)ld
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
CPP=$(PREFIX)cpp

# PIC32 device number
DEVICE		= 32MX340F512H

# UART settings for programmer
TTYDEV		?=/dev/ttyUSB0
TTYBAUD		?=115200

# Name of the project
PROGNAME = kernel

# Linkscript
# LINKSCRIPT	:= p$(shell echo "$(DEVICE)" | tr '[:upper:]' '[:lower:]').ld

# Resources
LINKSCRIPT := p32mx340f512h.ld
INCLUDEDIR := include
SOURCEDIR := src
BUILDDIR := build

# File transfer program
# PICOCOM_FTP ?= sendelf_crc32
# PICOCOM_FTP ?= sendtar_crc32

# Compiler and linker flags
C_INCLUDE_PATH = "$(TOOLCHAIN_INSTALL_DIR)/include"
SOME_LIBS = $(TOOLCHAIN_INSTALL_DIR)/lib/proc
MORE_LIBS = $(TOOLCHAIN_INSTALL_DIR)/lib
ASFLAGS =
CFLAGS = -march=mips32r2 -msoft-float -Wa,-msoft-float -G 0 -ffreestanding
CFLAGS += --sysroot=$(TOOLCHAIN_INSTALL_DIR) -I$(C_INCLUDE_PATH) -I$(INCLUDEDIR)
CFLAGS += -Wall -Wextra -Winline -std=gnu17
LDFLAGS = -T $(LINKSCRIPT) -L $(SOME_LIBS) -L $(MORE_LIBS) -lc

# set NOOPT = 1 to disable optimization
ifneq "$(NOOPT)" "1"
	CFLAGS += -O2
endif

# Filenames
ELFFILE = $(BUILDDIR)/$(PROGNAME).elf
HEXFILE = $(BUILDDIR)/$(PROGNAME).hex

CFILES = $(wildcard $(SOURCEDIR)/*.c)
ASFILES = $(wildcard $(SOURCEDIR)/*.S)
SYMSFILES = $(wildcard *.syms)

# Object file names
OBJFILES =  $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.c.o,$(CFILES))
OBJFILES += $(patsubst $(SOURCEDIR)/%.S,$(BUILDDIR)/%.S.o,$(ASFILES))
OBJFILES += $(SYMSFILES:.syms=.syms.o)


# Hidden directory for dependency file
DEPDIR = .deps
df = $(DEPDIR)/$(*F)

.PHONY: all clean install envcheck picocom sendelf_crc32 sendtar_crc32 disasm
.SUFFIXES:

all: $(HEXFILE)

clean:
	$(RM) helpers/sendelf_crc32
	$(RM) -rf $(HEXFILE) $(ELFFILE)
	$(RM) -rf $(BUILDDIR)/*
	$(RM) -rf $(DEPDIR)

picocom:
	picocom -b 19200 --parity n --send-cmd "$(PICOCOM_FTP)" $(TTYDEV)

sendelf_crc32:
	gcc -Wall -Wextra -o helpers/sendelf_crc32 helpers/sendelf_crc32.c;

sendtar_crc32:
	gcc -Wall -Wextra -o helpers/sendtar_crc32 helpers/sendtar_crc32.c;

disasm: $(ELFFILE)
	$(PREFIX)objdump -d $< > untracked/$(PROGNAME).dis.S

install: $(HEXFILE)
	$(PREFIX)avrdude -v -p $(shell echo "$(DEVICE)" | tr '[:lower:]' '[:upper:]') -c stk500v2 -P "$(TTYDEV)" -b $(TTYBAUD) -U "flash:w:$(HEXFILE)"

$(HEXFILE): $(ELFFILE)
	$(PREFIX)bin2hex -a $(ELFFILE)

$(ELFFILE): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEPDIR):
	mkdir -p $@

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/%.c.o: $(SOURCEDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.S.o: $(SOURCEDIR)/%.S | $(BUILDDIR)
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

