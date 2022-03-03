
#pragma once
#include "types.h"

typedef void (*EntryPoint)(void);

EntryPoint elf_load_file(u16 addr);

/* ELF definitions taken from Linux kernel v5.16.11 source:
 * - include/uapi/linux/elf.h
 * - include/uapi/linux/elf-em.h
 * - arch/mips/include/asm/elf.h
 */

/* 32-bit ELF base types. */
typedef u32	Elf32_Addr;
typedef u16	Elf32_Half;
typedef u32	Elf32_Off;
typedef i32	Elf32_Sword;
typedef u32	Elf32_Word;

#define EI_NIDENT	16

typedef struct {
  u8			e_ident[EI_NIDENT];
  Elf32_Half	e_type;
  Elf32_Half	e_machine;
  Elf32_Word	e_version;
  Elf32_Addr	e_entry;  /* Entry point */
  Elf32_Off		e_phoff;
  Elf32_Off		e_shoff;
  Elf32_Word	e_flags;
  Elf32_Half	e_ehsize;
  Elf32_Half	e_phentsize;
  Elf32_Half	e_phnum;
  Elf32_Half	e_shentsize;
  Elf32_Half	e_shnum;
  Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct {
  Elf32_Word	sh_name;
  Elf32_Word	sh_type;
  Elf32_Word	sh_flags;
  Elf32_Addr	sh_addr;
  Elf32_Off		sh_offset;
  Elf32_Word	sh_size;
  Elf32_Word	sh_link;
  Elf32_Word	sh_info;
  Elf32_Word	sh_addralign;
  Elf32_Word	sh_entsize;
} Elf32_Shdr;

typedef struct {
  Elf32_Word	p_type;
  Elf32_Off		p_offset;
  Elf32_Addr	p_vaddr;
  Elf32_Addr	p_paddr;
  Elf32_Word	p_filesz;
  Elf32_Word	p_memsz;
  Elf32_Word	p_flags;
  Elf32_Word	p_align;
} Elf32_Phdr;

typedef struct {
  Elf32_Addr	r_offset;
  Elf32_Word	r_info;
} Elf32_Rel;

typedef struct {
  Elf32_Addr	r_offset;
  Elf32_Word	r_info;
  Elf32_Sword	r_addend;
} Elf32_Rela;

typedef struct {
  Elf32_Word	st_name;
  Elf32_Addr	st_value;
  Elf32_Word	st_size;
  u8			st_info;
  u8			st_other;
  Elf32_Half	st_shndx;
} Elf32_Sym;

typedef struct {
  Elf32_Sword d_tag;
  union{
    Elf32_Sword	d_val;
    Elf32_Addr	d_ptr;
  } d_un;
} Elf32_Dyn;


/* These constants are for the segment types stored in the image headers */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7               /* Thread local storage segment */
#define PT_LOOS    0x60000000      /* OS-specific */
#define PT_HIOS    0x6fffffff      /* OS-specific */
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff
#define PT_GNU_EH_FRAME		0x6474e550
#define PT_GNU_PROPERTY		0x6474e553

#define PT_GNU_STACK	(PT_LOOS + 0x474e551)

/*
 * Extended Numbering
 *
 * If the real number of program header table entries is larger than
 * or equal to PN_XNUM(0xffff), it is set to sh_info field of the
 * section header at index 0, and PN_XNUM is set to e_phnum
 * field. Otherwise, the section header at index 0 is zero
 * initialized, if it exists.
 *
 * Specifications are available in:
 *
 * - Oracle: Linker and Libraries.
 *   Part No: 817–1984–19, August 2011.
 *   https://docs.oracle.com/cd/E18752_01/pdf/817-1984.pdf
 *
 * - System V ABI AMD64 Architecture Processor Supplement
 *   Draft Version 0.99.4,
 *   January 13, 2010.
 *   http://www.cs.washington.edu/education/courses/cse351/12wi/supp-docs/abi.pdf
 */
#define PN_XNUM 0xffff

/* These constants define the different elf file types */
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

/* This info is needed when parsing the symbol table */
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4
#define STT_COMMON  5
#define STT_TLS     6

#define ELF_ST_BIND(x)		((x) >> 4)
#define ELF_ST_TYPE(x)		(((unsigned int) x) & 0xf)
#define ELF32_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)	ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)	ELF_ST_TYPE(x)

/* The following are used with relocations */
#define ELF32_R_SYM(x) ((x) >> 8)
#define ELF32_R_TYPE(x) ((x) & 0xff)

/* These constants define the permissions on sections in the program
   header, p_flags. */
#define PF_R		0x4
#define PF_W		0x2
#define PF_X		0x1

/* sh_type */
#define SHT_NULL	0
#define SHT_PROGBITS	1
#define SHT_SYMTAB	2
#define SHT_STRTAB	3
#define SHT_RELA	4
#define SHT_HASH	5
#define SHT_DYNAMIC	6
#define SHT_NOTE	7
#define SHT_NOBITS	8
#define SHT_REL		9
#define SHT_SHLIB	10
#define SHT_DYNSYM	11
#define SHT_NUM		12
#define SHT_LOPROC	0x70000000
#define SHT_HIPROC	0x7fffffff
#define SHT_LOUSER	0x80000000
#define SHT_HIUSER	0xffffffff

/* sh_flags */
#define SHF_WRITE		0x1
#define SHF_ALLOC		0x2
#define SHF_EXECINSTR		0x4
#define SHF_RELA_LIVEPATCH	0x00100000
#define SHF_RO_AFTER_INIT	0x00200000
#define SHF_MASKPROC		0xf0000000

/* special section indexes */
#define SHN_UNDEF	0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_LIVEPATCH	0xff20
#define SHN_ABS		0xfff1
#define SHN_COMMON	0xfff2
#define SHN_HIRESERVE	0xffff

#define	EI_MAG0		0		/* e_ident[] indexes */
#define	EI_MAG1		1
#define	EI_MAG2		2
#define	EI_MAG3		3
#define	EI_CLASS	4
#define	EI_DATA		5
#define	EI_VERSION	6
#define	EI_OSABI	7
#define	EI_PAD		8

#define	ELFMAG0		0x7f		/* EI_MAG */
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'
#define	ELFMAG		"\177ELF"
#define	SELFMAG		4

#define	ELFCLASSNONE	0		/* EI_CLASS */
#define	ELFCLASS32	1
#define	ELFCLASS64	2
#define	ELFCLASSNUM	3

#define ELFDATANONE	0		/* e_ident[EI_DATA] */
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#define EV_NONE		0		/* e_version, EI_VERSION */
#define EV_CURRENT	1
#define EV_NUM		2

#define ELFOSABI_NONE	0
#define ELFOSABI_LINUX	3

#ifndef ELF_OSABI
#define ELF_OSABI ELFOSABI_NONE
#endif

#ifndef _LINUX_ELF_EM_H
#define _LINUX_ELF_EM_H

/* These constants define the various ELF target machines */
#define EM_NONE		0
#define EM_M32		1
#define EM_SPARC	2
#define EM_386		3
#define EM_68K		4
#define EM_88K		5
#define EM_486		6	/* Perhaps disused */
#define EM_860		7
#define EM_MIPS		8	/* MIPS R3000 (officially, big-endian only) */
				/* Next two are historical and binaries and
				   modules of these types will be rejected by
				   Linux.  */
#define EM_MIPS_RS3_LE	10	/* MIPS R3000 little-endian */
#define EM_MIPS_RS4_BE	10	/* MIPS R4000 big-endian */

#define EM_PARISC	15	/* HPPA */
#define EM_SPARC32PLUS	18	/* Sun's "v8plus" */
#define EM_PPC		20	/* PowerPC */
#define EM_PPC64	21	 /* PowerPC64 */
#define EM_SPU		23	/* Cell BE SPU */
#define EM_ARM		40	/* ARM 32 bit */
#define EM_SH		42	/* SuperH */
#define EM_SPARCV9	43	/* SPARC v9 64-bit */
#define EM_H8_300	46	/* Renesas H8/300 */
#define EM_IA_64	50	/* HP/Intel IA-64 */
#define EM_X86_64	62	/* AMD x86-64 */
#define EM_S390		22	/* IBM S/390 */
#define EM_CRIS		76	/* Axis Communications 32-bit embedded processor */
#define EM_M32R		88	/* Renesas M32R */
#define EM_MN10300	89	/* Panasonic/MEI MN10300, AM33 */
#define EM_OPENRISC     92     /* OpenRISC 32-bit embedded processor */
#define EM_ARCOMPACT	93	/* ARCompact processor */
#define EM_XTENSA	94	/* Tensilica Xtensa Architecture */
#define EM_BLACKFIN     106     /* ADI Blackfin Processor */
#define EM_UNICORE	110	/* UniCore-32 */
#define EM_ALTERA_NIOS2	113	/* Altera Nios II soft-core processor */
#define EM_TI_C6000	140	/* TI C6X DSPs */
#define EM_HEXAGON	164	/* QUALCOMM Hexagon */
#define EM_NDS32	167	/* Andes Technology compact code size
				   embedded RISC processor family */
#define EM_AARCH64	183	/* ARM 64 bit */
#define EM_TILEPRO	188	/* Tilera TILEPro */
#define EM_MICROBLAZE	189	/* Xilinx MicroBlaze */
#define EM_TILEGX	191	/* Tilera TILE-Gx */
#define EM_ARCV2	195	/* ARCv2 Cores */
#define EM_RISCV	243	/* RISC-V */
#define EM_BPF		247	/* Linux BPF - in-kernel virtual machine */
#define EM_CSKY		252	/* C-SKY */
#define EM_FRV		0x5441	/* Fujitsu FR-V */

/*
 * This is an interim value that we will use until the committee comes
 * up with a final number.
 */
#define EM_ALPHA	0x9026

/* Bogus old m32r magic number, used by old tools. */
#define EM_CYGNUS_M32R	0x9041
/* This is the old interim value for S/390 architecture */
#define EM_S390_OLD	0xA390
/* Also Panasonic/MEI MN10300, AM33 */
#define EM_CYGNUS_MN10300 0xbeef


#endif /* _LINUX_ELF_EM_H */

#ifndef _ASM_ELF_H
#define _ASM_ELF_H


/* ELF header e_flags defines. */
/* MIPS architecture level. */
#define EF_MIPS_ARCH_1		0x00000000	/* -mips1 code.	 */
#define EF_MIPS_ARCH_2		0x10000000	/* -mips2 code.	 */
#define EF_MIPS_ARCH_3		0x20000000	/* -mips3 code.	 */
#define EF_MIPS_ARCH_4		0x30000000	/* -mips4 code.	 */
#define EF_MIPS_ARCH_5		0x40000000	/* -mips5 code.	 */
#define EF_MIPS_ARCH_32		0x50000000	/* MIPS32 code.	 */
#define EF_MIPS_ARCH_64		0x60000000	/* MIPS64 code.	 */
#define EF_MIPS_ARCH_32R2	0x70000000	/* MIPS32 R2 code.  */
#define EF_MIPS_ARCH_64R2	0x80000000	/* MIPS64 R2 code.  */

/* The ABI of a file. */
#define EF_MIPS_ABI_O32		0x00001000	/* O32 ABI.  */
#define EF_MIPS_ABI_O64		0x00002000	/* O32 extended for 64 bit.  */

#define PT_MIPS_REGINFO		0x70000000
#define PT_MIPS_RTPROC		0x70000001
#define PT_MIPS_OPTIONS		0x70000002
#define PT_MIPS_ABIFLAGS	0x70000003

/* Flags in the e_flags field of the header */
#define EF_MIPS_NOREORDER	0x00000001
#define EF_MIPS_PIC		0x00000002
#define EF_MIPS_CPIC		0x00000004
#define EF_MIPS_ABI2		0x00000020
#define EF_MIPS_OPTIONS_FIRST	0x00000080
#define EF_MIPS_32BITMODE	0x00000100
#define EF_MIPS_FP64		0x00000200
#define EF_MIPS_NAN2008		0x00000400
#define EF_MIPS_ABI		0x0000f000
#define EF_MIPS_ARCH		0xf0000000

#define DT_MIPS_RLD_VERSION	0x70000001
#define DT_MIPS_TIME_STAMP	0x70000002
#define DT_MIPS_ICHECKSUM	0x70000003
#define DT_MIPS_IVERSION	0x70000004
#define DT_MIPS_FLAGS		0x70000005
	#define RHF_NONE	0x00000000
	#define RHF_HARDWAY	0x00000001
	#define RHF_NOTPOT	0x00000002
	#define RHF_SGI_ONLY	0x00000010
#define DT_MIPS_BASE_ADDRESS	0x70000006
#define DT_MIPS_CONFLICT	0x70000008
#define DT_MIPS_LIBLIST		0x70000009
#define DT_MIPS_LOCAL_GOTNO	0x7000000a
#define DT_MIPS_CONFLICTNO	0x7000000b
#define DT_MIPS_LIBLISTNO	0x70000010
#define DT_MIPS_SYMTABNO	0x70000011
#define DT_MIPS_UNREFEXTNO	0x70000012
#define DT_MIPS_GOTSYM		0x70000013
#define DT_MIPS_HIPAGENO	0x70000014
#define DT_MIPS_RLD_MAP		0x70000016

#define R_MIPS_NONE		0
#define R_MIPS_16		1
#define R_MIPS_32		2
#define R_MIPS_REL32		3
#define R_MIPS_26		4
#define R_MIPS_HI16		5
#define R_MIPS_LO16		6
#define R_MIPS_GPREL16		7
#define R_MIPS_LITERAL		8
#define R_MIPS_GOT16		9
#define R_MIPS_PC16		10
#define R_MIPS_CALL16		11
#define R_MIPS_GPREL32		12
/* The remaining relocs are defined on Irix, although they are not
   in the MIPS ELF ABI.	 */
#define R_MIPS_UNUSED1		13
#define R_MIPS_UNUSED2		14
#define R_MIPS_UNUSED3		15
#define R_MIPS_SHIFT5		16
#define R_MIPS_SHIFT6		17
#define R_MIPS_64		18
#define R_MIPS_GOT_DISP		19
#define R_MIPS_GOT_PAGE		20
#define R_MIPS_GOT_OFST		21
/*
 * The following two relocation types are specified in the MIPS ABI
 * conformance guide version 1.2 but not yet in the psABI.
 */
#define R_MIPS_GOTHI16		22
#define R_MIPS_GOTLO16		23
#define R_MIPS_SUB		24
#define R_MIPS_INSERT_A		25
#define R_MIPS_INSERT_B		26
#define R_MIPS_DELETE		27
#define R_MIPS_HIGHER		28
#define R_MIPS_HIGHEST		29
/*
 * The following two relocation types are specified in the MIPS ABI
 * conformance guide version 1.2 but not yet in the psABI.
 */
#define R_MIPS_CALLHI16		30
#define R_MIPS_CALLLO16		31
/*
 * Introduced for MIPSr6.
 */
#define R_MIPS_PC21_S2		60
#define R_MIPS_PC26_S2		61
/*
 * This range is reserved for vendor specific relocations.
 */
#define R_MIPS_LOVENDOR		100
#define R_MIPS_HIVENDOR		127

#define SHN_MIPS_ACCOMON	0xff00		/* Allocated common symbols */
#define SHN_MIPS_TEXT		0xff01		/* Allocated test symbols.  */
#define SHN_MIPS_DATA		0xff02		/* Allocated data symbols.  */
#define SHN_MIPS_SCOMMON	0xff03		/* Small common symbols */
#define SHN_MIPS_SUNDEFINED	0xff04		/* Small undefined symbols */

#define SHT_MIPS_LIST		0x70000000
#define SHT_MIPS_CONFLICT	0x70000002
#define SHT_MIPS_GPTAB		0x70000003
#define SHT_MIPS_UCODE		0x70000004
#define SHT_MIPS_DEBUG		0x70000005
#define SHT_MIPS_REGINFO	0x70000006
#define SHT_MIPS_PACKAGE	0x70000007
#define SHT_MIPS_PACKSYM	0x70000008
#define SHT_MIPS_RELD		0x70000009
#define SHT_MIPS_IFACE		0x7000000b
#define SHT_MIPS_CONTENT	0x7000000c
#define SHT_MIPS_OPTIONS	0x7000000d
#define SHT_MIPS_SHDR		0x70000010
#define SHT_MIPS_FDESC		0x70000011
#define SHT_MIPS_EXTSYM		0x70000012
#define SHT_MIPS_DENSE		0x70000013
#define SHT_MIPS_PDESC		0x70000014
#define SHT_MIPS_LOCSYM		0x70000015
#define SHT_MIPS_AUXSYM		0x70000016
#define SHT_MIPS_OPTSYM		0x70000017
#define SHT_MIPS_LOCSTR		0x70000018
#define SHT_MIPS_LINE		0x70000019
#define SHT_MIPS_RFDESC		0x7000001a
#define SHT_MIPS_DELTASYM	0x7000001b
#define SHT_MIPS_DELTAINST	0x7000001c
#define SHT_MIPS_DELTACLASS	0x7000001d
#define SHT_MIPS_DWARF		0x7000001e
#define SHT_MIPS_DELTADECL	0x7000001f
#define SHT_MIPS_SYMBOL_LIB	0x70000020
#define SHT_MIPS_EVENTS		0x70000021
#define SHT_MIPS_TRANSLATE	0x70000022
#define SHT_MIPS_PIXIE		0x70000023
#define SHT_MIPS_XLATE		0x70000024
#define SHT_MIPS_XLATE_DEBUG	0x70000025
#define SHT_MIPS_WHIRL		0x70000026
#define SHT_MIPS_EH_REGION	0x70000027
#define SHT_MIPS_XLATE_OLD	0x70000028
#define SHT_MIPS_PDR_EXCEPTION	0x70000029

#define SHF_MIPS_GPREL		0x10000000
#define SHF_MIPS_MERGE		0x20000000
#define SHF_MIPS_ADDR		0x40000000
#define SHF_MIPS_STRING		0x80000000
#define SHF_MIPS_NOSTRIP	0x08000000
#define SHF_MIPS_LOCAL		0x04000000
#define SHF_MIPS_NAMES		0x02000000
#define SHF_MIPS_NODUPES	0x01000000

#define MIPS_ABI_FP_ANY		0	/* FP ABI doesn't matter */
#define MIPS_ABI_FP_DOUBLE	1	/* -mdouble-float */
#define MIPS_ABI_FP_SINGLE	2	/* -msingle-float */
#define MIPS_ABI_FP_SOFT	3	/* -msoft-float */
#define MIPS_ABI_FP_OLD_64	4	/* -mips32r2 -mfp64 */
#define MIPS_ABI_FP_XX		5	/* -mfpxx */
#define MIPS_ABI_FP_64		6	/* -mips32r2 -mfp64 */
#define MIPS_ABI_FP_64A		7	/* -mips32r2 -mfp64 -mno-odd-spreg */

#ifdef CONFIG_32BIT
/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch elf32_check_arch

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS32
#endif /* CONFIG_32BIT */

#ifdef CONFIG_64BIT
/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch elf64_check_arch
#define compat_elf_check_arch elf32_check_arch

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS64
#endif /* CONFIG_64BIT */

/*
 * These are used to set parameters in the core dumps.
 */
#ifdef __MIPSEB__
#define ELF_DATA	ELFDATA2MSB
#elif defined(__MIPSEL__)
#define ELF_DATA	ELFDATA2LSB
#endif
#define ELF_ARCH	EM_MIPS

/*
 * In order to be sure that we don't attempt to execute an O32 binary which
 * requires 64 bit FP (FR=1) on a system which does not support it we refuse
 * to execute any binary which has bits specified by the following macro set
 * in its ELF header flags.
 */
#ifdef CONFIG_MIPS_O32_FP64_SUPPORT
# define __MIPS_O32_FP64_MUST_BE_ZERO	0
#else
# define __MIPS_O32_FP64_MUST_BE_ZERO	EF_MIPS_FP64
#endif
#define vmcore_elf32_check_arch mips_elf_check_machine
#define vmcore_elf64_check_arch mips_elf_check_machine

#define CORE_DUMP_USE_REGSET
#define ELF_EXEC_PAGESIZE	PAGE_SIZE
#endif /* _ASM_ELF_H */

