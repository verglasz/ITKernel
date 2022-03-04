#pragma once

#include "types.h"

#define USTAR_FILENAME_SIZE 100
#define USTAR_FILE_NOT_FOUND -1

typedef struct {
	u16 rom_addr;
	u16 size;
	u8 type;
	char filename[USTAR_FILENAME_SIZE];
} FileInfo;

int ustar_find_file(u16 start_addr, const char *filename, u16 *addr, usize *size);
isize ustar_list_files(FileInfo *buffer, usize max_count);

