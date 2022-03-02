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

# Linkscript
# LINKSCRIPT	:= p$(shell echo "$(DEVICE)" | tr '[:upper:]' '[:lower:]').ld
LINKSCRIPT := link.ld

# Resources

INCLUDEDIR := include
BUILDDIR := build
PROGDIR := progs
LIBDIR := itklibc

# File transfer program
# PICOCOM_FTP ?= utils/sendelf_crc32
# PICOCOM_FTP ?= utils/sendtar_crc32

# Compiler and linker flags
C_INCLUDE_PATH = "$(TOOLCHAIN_INSTALL_DIR)/include"
SOME_LIBS = $(TOOLCHAIN_INSTALL_DIR)/lib/proc
MORE_LIBS = $(TOOLCHAIN_INSTALL_DIR)/lib
ASFLAGS =
CFLAGS = -march=mips32r2 -msoft-float -Wa,-msoft-float -G 0 -ffreestanding
CFLAGS += --sysroot=$(TOOLCHAIN_INSTALL_DIR) -I$(C_INCLUDE_PATH) -I$(INCLUDEDIR)
CFLAGS += -Wall -Wextra -Winline -std=gnu17
# setting max-page-size avoids aligning things to 4k page boundaries in the linked elf,
# since we aren't memmapping it or anything
# but just straight up copying it in memory mostly byte by byte, it should make no difference
# we shouldn't really need more than 4 bytes alignment but let's make it 16 for good measure
# (this is absolutely necessary to get size down to something reasonable)
LDFLAGS = -T $(LINKSCRIPT) -L $(SOME_LIBS) -L $(MORE_LIBS) -lc -z max-page-size=16

# set NOOPT = 1 to disable optimization
ifneq "$(NOOPT)" "1"
	CFLAGS += -O2
endif

PROGFILES = $(wildcard $(PROGDIR)/*.c)
LIBFILES = $(wildcard $(LIBDIR)/*.c)
ASLIBFILES = $(wildcard $(LIBDIR)/*.S)

# Filenames
LEANFILES = $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.elf.stripped,$(PROGFILES))
ELFFILES = $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.elf,$(PROGFILES))
HEXFILES = $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.hex,$(PROGFILES))

# Object file names
OBJFILES =  $(patsubst $(LIBDIR)/%.c,$(BUILDDIR)/$(LIBDIR)/%.c.o,$(LIBFILES))
OBJFILES +=  $(patsubst $(LIBDIR)/%.S,$(BUILDDIR)/$(LIBDIR)/%.S.o,$(ASLIBFILES))


.PHONY: all lib progs helpers progobjs clean install stripcheck picocom sendelf_crc32 sendtar_crc32 disasm
.SUFFIXES:


all: $(LEANFILES) progs

progs: $(ELFFILES)

lib: $(OBJFILES)

stripcheck:
	@sstrip --version > /dev/null

progobjs: $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.o,$(PROGFILES))

clean:
	$(RM) -rf $(LEANFILES)
	$(RM) -rf $(ELFFILES)
	$(RM) -rf $(HEXFILES)
	$(RM) -rf $(OBJFILES)

picocom: helpers
	picocom -b 19200 --parity n --send-cmd "$(PICOCOM_FTP)" $(TTYDEV)

helpers: sendelf_crc32 sendtar_crc32

sendelf_crc32:
	gcc -Wall -Wextra -o utils/sendelf_crc32 utils/sendelf_crc32.c;

sendtar_crc32:
	gcc -Wall -Wextra -o utils/sendtar_crc32 utils/sendtar_crc32.c;

$(BUILDDIR)/%.elf.stripped: $(BUILDDIR)/%.elf | stripcheck
	cp $< $@
	sstrip $@

$(BUILDDIR)/%.elf: $(BUILDDIR)/%.o $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/$(LIBDIR):
	mkdir -p $@

$(BUILDDIR)/%.o: $(PROGDIR)/%.c | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/$(LIBDIR)/%.c.o: $(LIBDIR)/%.c | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/$(LIBDIR)/%.S.o: $(LIBDIR)/%.S | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

