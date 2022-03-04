
/* Some code adapted from Wiktor Dobrosierdow's implementation
 */

#include "ustar.h"

#include "eeprom.h"
#include "serial_io.h"
#include "stdio.h"
#include "types.h"

#define USTAR_BLOCK_SIZE 512

typedef union {
    struct {
        /* Pre-POSIX.1-1988 */
        char filename[USTAR_FILENAME_SIZE];
        char filemode[8];
        char uid[8];
        char gid[8];
        char filesize[12];
        char modtime[12];
        char checksum[8];
        /* UStar */
        char type;
        char linkname[100];
        char ustar[6];
        char version[2];
        char owner[32];
        char group[32];
        char major[8];
        char minor[8];
        char prefix[155];
    };

    /* Make sure this type is 512 bytes long (size of a block) */
    char block[USTAR_BLOCK_SIZE];
} UStar_Fhdr;

/**
 * Convert an ASCII octal number into an integer.
 */
int oct2bin(const char *data, usize size) {
    int n = 0;
    while (size > 0) {
        n *= 8;
        n += *data - '0';
        data++;
        size--;
    }
    return n;
}

/**
 * Find a file in the EEPROM containing a UStar filesystem (tar archive).
 * addr and size will not be modified unless a file is found.
 *
 * @param filename Filename.
 * @param addr File address on EEPROM (if found).
 * @param size File size on EEPROM (if found).
 */
int ustar_find_file(u16 start_addr, const char *filename, u16 *addr, usize *size) {
    UStar_Fhdr header;
    usize tmpsize;
    u16 curaddr;
    for (curaddr = start_addr; curaddr < EEPROM_SIZE - USTAR_BLOCK_SIZE;) {
        serial_printf("0x%x\n", curaddr);
        isize rd = eeprom_read(curaddr, &header, USTAR_BLOCK_SIZE);
        serial_printf("Read %d bytes\n", USTAR_BLOCK_SIZE);
        // Make sure we are still in the filesystem
        if (__builtin_memcmp(header.ustar, "ustar", 5)) {
            char buf[6];
            __builtin_memcpy(buf, header.ustar, 5);
            buf[5] = '\0';
            serial_printf("Header wasn't `ustar`, found: %s", buf);
            return USTAR_FILE_NOT_FOUND;
        }

        curaddr += USTAR_BLOCK_SIZE;
        // FIXME: limit filename size to 99 (+ \0)
        tmpsize = oct2bin(header.filesize, sizeof(header.filesize) - 1);
        if (!__builtin_memcmp(header.filename, filename, __builtin_strlen(filename) + 1)) {
            serial_printf(
                "file found! has filename %s, at addr 0x%x, has size %d\n",
                header.filename,
                (usize)curaddr,
                tmpsize);
            // File found
            *addr = curaddr;
            *size = tmpsize;
            return 0;
        }

        curaddr += tmpsize;
        if (tmpsize % USTAR_BLOCK_SIZE) curaddr += USTAR_BLOCK_SIZE - (tmpsize % USTAR_BLOCK_SIZE);
    }
    serial_printf("File not found, reached address 0x%x\n", curaddr);

    return USTAR_FILE_NOT_FOUND;
}

/* Read the eeprom from address 0 assuming it contains an ustar filesystem,
 *
 */
isize ustar_list_files(FileInfo *buffer, usize max_count) {
    return -1;
}
