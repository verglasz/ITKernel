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
ifeq "$(shell hostname)" "helium"
	TTYDEV		=/dev/ttyUSB0
	PICOCOMMAND =picocom
	SSTRIP 		=sstrip
else ifeq "$(shell hostname)" "boron"
	TTYDEV		=/dev/ttyUSB0
	PICOCOMMAND =picocom
	SSTRIP 		=sstrip
else
	PICOCOMMAND = NO_CUSTOM_BAUD=1 /mnt/c/Users/User/Desktop/picocom/picocom
	TTYDEV		=/dev/ttyS3
	SSTRIP 		=touch
endif
TTYBAUD		?=115200 # would be nice to use this for picocom as well, but not sure we can really take advantage of it

# Linkscript
LINKSCRIPT := link.ld # lovingly handcrafted link script

# Resources

INCLUDEDIR := include
BUILDDIR := build
PROGDIR := progs
LIBDIR := itklibc
FINALDIR := $(BUILDDIR)/run

# File transfer program
PICOCOM_SEND ?= ./utils/pysend.py
PICOCOM_RECEIVE ?= ./utils/readserial.py

COM_FLAGS = -b 19200 --parity n --send-cmd $(PICOCOM_SEND) --receive-cmd $(PICOCOM_RECEIVE) --omap crlf --imap igncr,lfcrlf,spchex,8bithex # --emap nrmhex,spchex,8bithex --echo

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
PAYLOAD := payload.ustar
RUNFILES := $(patsubst $(PROGDIR)/%.noio.c,$(FINALDIR)/%.run,$(PROGFILES))
RUNFILES := $(patsubst $(PROGDIR)/%.c,$(FINALDIR)/%.run,$(RUNFILES))
LEANFILES = $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.elf.stripped,$(PROGFILES))
ELFFILES = $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.elf,$(PROGFILES))
HEXFILES = $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.hex,$(PROGFILES))


PROGOBJS = $(patsubst $(PROGDIR)/%.c,$(BUILDDIR)/%.o,$(PROGFILES))
# Object file names
OBJFILES =  $(patsubst $(LIBDIR)/%.c,$(BUILDDIR)/$(LIBDIR)/%.c.o,$(LIBFILES))
OBJFILES +=  $(patsubst $(LIBDIR)/%.S,$(BUILDDIR)/$(LIBDIR)/%.S.o,$(ASLIBFILES))
NOIOOBJS = $(patsubst $(BUILDDIR)/$(LIBDIR)/%.o,$(BUILDDIR)/$(LIBDIR)/%.noio.o,$(OBJFILES))

.PHONY: all iolib noiolib lean lib progs helpers progobjs clean install stripcheck picocom sendelf_crc32 sendtar_crc32 disasm
.SUFFIXES:


all: $(PAYLOAD)

$(PAYLOAD): $(RUNFILES)
	cd $(FINALDIR) && tar --create --file ../../$@ --format=ustar --record-size=1k $(patsubst $(FINALDIR)/%, %, $^)

lean: $(LEANFILES)

progs: $(ELFFILES)

lib: iolib noiolib

iolib:$(OBJFILES)

noiolib: $(NOIOOBJS)
	echo $^

progobjs: $(PROGOBJS)

stripcheck:
	@(sstrip --version > /dev/null || (echo "ERROR: elfkickers's `sstrip` not installed!" && exit 1))

clean:
	$(RM) -rf $(PAYLOAD)
	$(RM) -rf $(LEANFILES)
	$(RM) -rf $(ELFFILES)
	$(RM) -rf $(HEXFILES)
	$(RM) -rf $(OBJFILES)
	$(RM) -rf $(RUNFILES)
	$(RM) -rf $(PROGOBJS)
	$(RM) -rf $(NOIOOBJS)

picocom: helpers
	picocom $(COM_FLAGS) $(TTYDEV)

helpers: sendelf_crc32 sendtar_crc32

sendelf_crc32:
	gcc -Wall -Wextra -o utils/sendelf_crc32 utils/sendelf_crc32.c;

sendtar_crc32:
	gcc -Wall -Wextra -o utils/sendtar_crc32 utils/sendtar_crc32.c;

$(FINALDIR)/fib.run: $(BUILDDIR)/fib.elf.stripped
	cp $< $@

$(FINALDIR)/%.run: $(BUILDDIR)/%.noio.elf.stripped | $(FINALDIR)
	cp $< $@

$(BUILDDIR)/%.elf.stripped: $(BUILDDIR)/%.elf | stripcheck
	cp $< $@
	$(SSTRIP) $@

$(BUILDDIR)/%.noio.elf: $(BUILDDIR)/%.noio.o $(NOIOOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.elf: $(BUILDDIR)/%.o $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(FINALDIR):
	mkdir -p $@

$(BUILDDIR)/$(LIBDIR):
	mkdir -p $@

$(BUILDDIR)/%.o: $(PROGDIR)/%.c | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/$(LIBDIR)/%.c.noio.o: $(LIBDIR)/%.c | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) -c -o $@ $< -DNOIO

$(BUILDDIR)/$(LIBDIR)/%.c.o: $(LIBDIR)/%.c | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/$(LIBDIR)/%.S.noio.o: $(LIBDIR)/%.S | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

$(BUILDDIR)/$(LIBDIR)/%.S.o: $(LIBDIR)/%.S | $(BUILDDIR)/$(LIBDIR)
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

