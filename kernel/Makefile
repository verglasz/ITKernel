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
PICOCOM_FTP ?= sendelf_crc32
# PICOCOM_FTP ?= sendtar_crc32

# Compiler and linker flags
C_INCLUDE_PATH = "$(TOOLCHAIN_INSTALL_DIR)/include"
SOME_LIBS = "$(TOOLCHAIN_INSTALL_DIR)/lib/proc"
MORE_LIBS = "$(TOOLCHAIN_INSTALL_DIR)/lib"
ASFLAGS =
CFLAGS = -march=mips32r2 -msoft-float -Wa,-msoft-float -G 0 -ffreestanding
CFLAGS += --sysroot=$(TOOLCHAIN_INSTALL_DIR) -I$(C_INCLUDE_PATH) -I$(INCLUDEDIR)
CFLAGS += -Wall -Wextra -Winline -std=gnu17
LDFLAGS = -T $(LINKSCRIPT) -lc -L $(SOME_LIBS) -L $(MORE_LIBS)

# set NOOPT = 1 to disable optimization
ifneq "$(NOOPT)" "1"
	CFLAGS += -O2
endif

# Filenames
ELFFILE = $(PROGNAME).elf
HEXFILE = $(PROGNAME).hex

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
	picocom -b 19200 -y n --send-cmd "./helpers/$(PICOCOM_FTP)" $(TTYDEV)

sendelf_crc32:
	@echo "$(TARGET)" | grep mcb32 > /dev/null && { \
		echo "This should be compiled in the native environment!"; \
		exit 1; \
	} || { \
		gcc -Wall -Wextra -o helpers/sendelf_crc32 helpers/sendelf_crc32.c; \
	}

sendtar_crc32:
	@echo "$(TARGET)" | grep mcb32 > /dev/null && { \
		echo "This should be compiled in the native environment!"; \
		exit 1; \
	} || { \
		gcc -Wall -Wextra -o helpers/sendtar_crc32 helpers/sendtar_crc32.c; \
	}

disasm: envcheck
	$(PREFIX)objdump -d $(PROGNAME).elf > untracked/$(PROGNAME).elf.S

envcheck:
	@echo "$(TARGET)" | grep mcb32 > /dev/null || (\
		echo ""; \
		echo " **************************************************************"; \
		echo " * Make sure you have sourced the cross compiling environment *"; \
		echo " * Do this by issuing:                                        *"; \
		echo " * . /path/to/crosscompiler/environment                       *"; \
		echo " **************************************************************"; \
		echo ""; \
		exit 1)

install: $(HEXFILE)
	$(PREFIX)avrdude -v -p $(shell echo "$(DEVICE)" | tr '[:lower:]' '[:upper:]') -c stk500v2 -P "$(TTYDEV)" -b $(TTYBAUD) -U "flash:w:$(HEXFILE)"

$(HEXFILE): $(ELFFILE) | envcheck
	$(PREFIX)bin2hex -a $(ELFFILE)

$(ELFFILE): $(OBJFILES) |  envcheck
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEPDIR):
	mkdir $@

$(BUILDDIR):
	mkdir $@

# Compile C files
$(BUILDDIR)/%.c.o: $(SOURCEDIR)/%.c envcheck | $(BUILDDIR)
	# echo $*.c.d $(df).c.P $$
	$(CC) $(CFLAGS) -c -o $@ $<
	# @cp $(BUILDDIR)/$*.c.d $(df).c.P; sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' < $(BUILDDIR)/$*.c.d >> $(df).c.P

# Compile ASM files with C pre-processor directives
$(BUILDDIR)/%.S.o: $(SOURCEDIR)/%.S envcheck | $(BUILDDIR)
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $@ $<
	# @cp $(BUILDDIR)/$*.S.d $(df).S.P; sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' < $(BUILDDIR)/$*.S.d >> $(df).S.P;

# Link symbol lists to object files
# %.syms.o: %.syms
# 	$(LD) -o $@ -r --just-symbols=$<

# Check dependencies
# -include $(CFILES:%.c=$(DEPDIR)/%.c.P)
# -include $(ASFILES:%.S=$(DEPDIR)/%.S.P)

