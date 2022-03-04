#include "ustar.h"

#include "eeprom.h"
#include "serial_io.h"
#include "stdio.h"
#include "types.h"

#define USTAR_BLOCK_SIZE 512
#define USTAR_ADDR_MASK (~511)

typedef struct {
    /* Pre-POSIX.1-1988 */
    char filename[USTAR_FILENAME_SIZE];
    char filemode[8];
    char uid[8];
    char gid[8];
    char filesize[12];
    char modtime[12];
    char checksum[8];
    /* Post-1988 UStar */
    char type;
    char linkname[100];
    char ustar[6];
    char version[2];
    char owner[32];
    char group[32];
    char major[8];
    char minor[8];
    char prefix[155];
} FileHeader;

/* Get the value of an ASCII octal number
 */
int oct_atoi(const char *str) {
    int val = 0;
    while (*str != '\0') {
        val *= 8;
        val += *str - '0';
        str++;
    }
    return val;
}

/* Read the eeprom from address start_addr assuming it contains an ustar filesystem,
 * scanning all files found and saving their info in buffer
 * The filesystem MUST be aligned to the ustar blocksize (512, 0x200)
 */
isize ustar_list_files(u16 start_addr, FileInfo *buffer, usize max_count) {
    FileHeader fh;
    usize filesize = 0;
    u16 rom_addr = start_addr;
    usize count = 0;
    while (count < max_count && rom_addr < EEPROM_SIZE - USTAR_BLOCK_SIZE) {
        isize ret = eeprom_read(rom_addr, &fh, sizeof(fh));
        serial_printf(
            "list_files: read %d bytes at addr 0x%x, "
            "ret: %d\n",
            sizeof(fh),
            (usize)rom_addr,
            ret);
        if (ret != 0) return USTAR_READ_ERROR;
        if (__builtin_memcmp(fh.ustar, "ustar", 5)) {
            char buf[6];
            __builtin_memcpy(buf, fh.ustar, 5);
            buf[5] = '\0';
            serial_printf(
                "list_files: at addr 0x%x wasn't `ustar`, found: %s, stopping search\n",
                rom_addr,
                buf);
            break;
        }
        filesize = oct_atoi(fh.filesize);
        rom_addr += USTAR_BLOCK_SIZE;
        serial_printf(
            "list_files: file `%s` found at addr 0x%x, size %d\n",
            fh.filename,
            (usize)rom_addr,
            filesize);
        buffer[count].rom_addr = rom_addr;
        buffer[count].size = filesize;
        __builtin_memcpy(buffer[count].filename, fh.filename, USTAR_FILENAME_SIZE);
        count++;
        rom_addr += filesize;
        if (rom_addr % USTAR_BLOCK_SIZE != 0) {
            // skip rest of block (zeroes) and to go start of next
            rom_addr = (rom_addr + USTAR_BLOCK_SIZE) & USTAR_ADDR_MASK;
        }
    }
    return count;
}

int ustar_find_file(u16 start_addr, const char *filename, u16 *addr, usize *size) {
    return -1;
}
