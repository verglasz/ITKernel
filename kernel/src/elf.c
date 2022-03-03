
#include "elf.h"

#include "eeprom.h"
#include "serial_io.h"
#include "usermode.h"

static bool is_userspace(usize addr) {
    return (
        USER_DATA_START <= addr && addr < USER_DATA_END && USER_PROG_START <= addr
        && addr < USER_PROG_END);
}

/* Load an elf file from the eeprom into its memory
 */
EntryPoint elf_load_file(u16 addr) {
    Elf32_Ehdr elfheader;
    isize ret = eeprom_read(addr, &elfheader, sizeof(elfheader));
    if (ret != 0 || __builtin_memcmp(elfheader.e_ident, ELFMAG, SELFMAG)) {
        return NULL; // XXX: should probably distinguish the errors
    }
    // ideally we'd validate more than just the magic bytes, but time is what it is
    u16 ph_start = addr + elfheader.e_phoff;
    Elf32_Phdr ph; // program header
    for (int i = 0; i < elfheader.e_phnum; i++) {
        u16 phaddr = ph_start + i * elfheader.e_phentsize;
        ret = eeprom_read(phaddr, &ph, sizeof(ph));
        if (ret != 0) return NULL;
        if (ph.p_type != PT_LOAD) continue;
        if (!is_userspace(ph.p_vaddr) || !is_userspace(ph.p_vaddr + ph.p_memsz - 1)) {
            return NULL;
        }
        eeprom_read(addr + ph.p_offset, (u8 *)ph.p_vaddr, ph.p_filesz);
        if (ph.p_memsz > ph.p_filesz) { // zero out the remaining memory
            __builtin_memset(ph.p_vaddr + ph.p_filesz, 0, ph.p_memsz - ph.p_filesz);
        }
    }
    return (EntryPoint)elfheader.e_entry;
}
