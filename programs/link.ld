/* Very custom linker script */
OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(_start)
/*
 * Provide for a minimum stack and heap size
 * - _min_stack_size - represents the minimum space that must be made
 *                     available for the stack.  Can be overridden from
 *                     the command line using the linker's --defsym option.
 * - _min_heap_size  - represents the minimum space that must be made
 *                     available for the heap.  Can be overridden from
 *                     the command line using the linker's --defsym option.
 */
EXTERN (_min_stack_size _min_heap_size)
PROVIDE(_min_stack_size = 0x800) ;
PROVIDE(_min_heap_size = 0x800) ;

/*************************************************************************
 * Processor-specific object file.  Contains SFR definitions.
 *************************************************************************/

/*************************************************************************
 * Memory Regions
 *
 * Memory regions without attributes cannot be used for orphaned sections.
 * Only sections specifically assigned to these regions can be allocated
 * into these regions.
 *************************************************************************/
MEMORY
{
  /* 0x7F000000 + BMXDUDBA */
  kuseg_data_mem       (w!x) : ORIGIN = 0x7F002000, LENGTH = 0x3000
  /* 0x7F000000 + BMXDUPBA */
  kuseg_program_mem    (rx)  : ORIGIN = 0x7F005000, LENGTH = 0x3000
}

/*************************************************************************
 * Memory Address Equates
 *************************************************************************/
_START_ADDR             = ORIGIN(kuseg_program_mem);

/*************************************************************************
 *  Bootloader program directives.
 *
 * _IMAGE_TYPE
 *
 *  image type:
 */

_imageReserved                      = 0x00000000 ;
_imageMPIDE                         = 0x00000001 ;  /* This is a normal MPIDE sketch                                                                                                    */
_imageBootFlashBootloader           = 0x00000002 ;  /* This is a boot flash bootloader                                                                                                  */
_imageProgramFlashBootloader        = 0x00000004 ;  /* This is a program flash bootloader                                                                                               */
_imageSplitFlashBootloader          = 0x00000008 ;  /* This has bootloader code in both boot and program flash                                                                          */

 /*
 *  Instructions for the bootloader
 */

_imageFullFlashEraseLess4KEEProm    = 0x00010000 ;  /* The original bootloader method of erasing all of program flash except the last 4K reserved for eeprom                            */
_imageJustInTimeFlashErase          = 0x00020000 ;  /* Only flash pages written too needed by the sketch is erased                                                                      */
_imageLinkerSpecifiedFlashErase     = 0x00040000 ;  /* The linker defines the flash range to erase                                                                                      */
_imageFullFlashErase                = 0x00080000 ;  /* All of flash is erased                                                                                                           */
_imageExecutionJumpAddress          = 0x01000000 ;  /* the bootloader will jump to the execution jump address immediately after programming                                             */
_imageExecutionJumpToFirstInFlash   = 0x02000000 ;  /* the bootloader will jump to the first sketch loaded in flash ignoring the execution jump address immediately after programming   */

/*
 * _IMAGE_FLASH_SIZE
 *
 *      Typically _imageJustInTimeFlashErase is selected to just erase the pages
 *      of flash that code is written too; thus leaving all other flash pages untouched.
 *
 *      If _imageLinkerSpecifiedFlashErase set, then the range
 *      starting from _ebase_address for _IMAGE_FLASH_SIZE bytes are erased.
 *
 *      If _imageFullFlashErase is specified, than the whole flash
 *      as known by the bootloader will be erased. This will erase eeprom as well
 *
 *      if _imageFullFlashEraseLess4KEEProm is set, all of flash less the last 4K is
 *      erased, this is the old default. This bit could be set to make a program flash bootloader
 *      erasing everything but the old flash. If NOTHING is set, this will be the default as this is the old behavior.
 *
 *  _JUMP_ADDR
 *
 *      This is the address that the bootloader will jump to start execution
 *      of the sketch. This is almost always _RESET_ADDR.
 *
 *      However, you can specify an alternate entry execution point for example
 *      if you have alternate starup code that, say, shared
 *      the runtime with other sketches or needed some kind of specific handling
 *
 *      Immediately after programming (avrdude upload) the bootloader will typically
 *      jump to the just loaded sketch, no matter where it was loaded in flash.
 *      _imageExecutionJumpToFirstInFlash will tell the bootloader to jump to the first
 *      sketch in flash even if the just loaded one is not at the beginning of flash.
 *      This is useful when programming sketches in slots of flash and then always
 *      jumping to the program-flash loader (vector sketch) as if the board was just reset.
 *      This bit does not effect jumping to a sketch already in flash after reset.
 *      As of today, after reset, the first program in flash will always be jumped to.
 *
 *************************************************************************/
 _IMAGE_TYPE            = _imageMPIDE | _imageJustInTimeFlashErase | _imageExecutionJumpAddress;
 _IMAGE_FLASH_SIZE      = LENGTH(exception_mem) + LENGTH(kuseg_program_mem);
_JUMP_ADDR              = _START_ADDR;

/*INPUT("crt0.o") */ /* our pathetic excuse for a libc takes care of that */
INPUT("crti.o")
INPUT("crtbegin.o")
INPUT("crtend.o")
INPUT("crtn.o")

/*************************************************************************
 * Start the layout of memory, the sections...
 *************************************************************************/
SECTIONS
{
  /* Boot Sections */
  .start _START_ADDR :
  {
    KEEP(*(.start))
  } > kuseg_program_mem

  /* we have to explicitely discard these */
  /DISCARD/ : { *(.bev_handler) }
  /DISCARD/ : { *(.gen_handler) }

  .text  :
  {
    _text_begin = . ;

    *(.stub .gnu.linkonce.t.*)
    KEEP (*(.text.*personality*))
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.mips16.fn.*)
    *(.mips16.call.*)
    *(.gnu.warning)
    . = ALIGN(4) ;
  } >kuseg_program_mem =0

   .init      :
  {
  KEEP (*crti.o(.init))
  KEEP (*crtbegin.o(.init))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o *crtn.o ).init))
  KEEP (*crtend.o(.init))
    KEEP (*crtn.o(.init))
    . = ALIGN(4) ;
  }  >kuseg_program_mem

  .fini       :
  {
    KEEP (*(.fini))
    . = ALIGN(4) ;
  } >kuseg_program_mem

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    . = ALIGN(4) ;
    PROVIDE_HIDDEN (__preinit_array_end = .);
  }>kuseg_program_mem

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    . = ALIGN(4) ;
    PROVIDE_HIDDEN (__init_array_end = .);
  }>kuseg_program_mem

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array))
    . = ALIGN(4) ;
    PROVIDE_HIDDEN (__fini_array_end = .);
  }>kuseg_program_mem

  .ctors        :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    . = ALIGN(4) ;
  }>kuseg_program_mem

  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    . = ALIGN(4) ;
  }>kuseg_program_mem

  .preinit_array    :
  {
    KEEP (*(.preinit_array))
    . = ALIGN(4) ;
  }>kuseg_program_mem

  /* i really dont think we need this */

  /* Read-only sections */
  /* .header_info ALIGN(4) : */
  /* { */
  /*   _image_header_info = . ; */
  /*   LONG(SIZEOF(.header_info))                 /1* size of this structure       *1/ */
  /*   LONG(0xFFFFFFFF)                           /1* bootloader version           *1/ */
/* /1*  KEEP(*(.mpide_version *.mpide_version.*))  /1* MPIDE build version          *1/ */
  /*   LONG(0x0100030E)                           /1* Spoofed MPIDE build version  *1/ */
  /*   LONG(0xFFFFFFFF)                           /1* booloader capabilities       *1/ */
  /*   LONG(0xFFFFFFFF)                           /1* VID and PID                  *1/ */
  /*   LONG(_IMAGE_TYPE)                          /1* image type and options       *1/ */
  /*   LONG(_JUMP_ADDR)                           /1* image execution address      *1/ */
  /*   LONG(_ebase_address)                       /1* image base address           *1/ */
  /*   LONG(_IMAGE_FLASH_SIZE)                    /1* size of image flash used     *1/ */
  /*   LONG(ORIGIN(kuseg_eeprom_mem))             /1* eeprom location              *1/ */
  /*   LONG(LENGTH(kuseg_eeprom_mem))             /1* eeprom size                  *1/ */
  /*   LONG(ORIGIN(configsfrs))                   /1* config bits location         *1/ */
  /*   LONG(LENGTH(configsfrs))                   /1* config bits size             *1/ */
  /*   LONG(_ram_header_addr)                     /1* ram header pointer           *1/ */
  /*   LONG(SIZEOF(.ram_exchange_data))           /1* ram Header size              *1/ */
  /*   LONG(0xFFFFFFFF)                           /1* what the bootloader skips    *1/ */
  /*   LONG(_original_vector_0)                   /1* the original vector 0        *1/ */
  /*   LONG(_new_vector_0)                        /1* new/indirect vector 0        *1/ */
  /*   LONG(_vector_spacing << 5)                 /1* spacing of both org/new vec  *1/ */
  /*   _image_header_info_end = . ; */
  /* } > kuseg_program_mem */
  /* ASSERT (SIZEOF(.header_info) == 76, "MPIDE Version not specfied correctly") */

  .rodata  ALIGN(4) :
  {
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    . = ALIGN(4) ;
  } >kuseg_program_mem

  /*
   * Small initialized constant global and static data can be placed in the
   * .sdata2 section.  This is different from .sdata, which contains small
   * initialized non-constant global and static data.
   */
  .sdata2 ALIGN(4)   :
  {
    *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
    . = ALIGN(4) ;
  } >kuseg_program_mem

  /*
   * Uninitialized constant global and static data (i.e., variables which will
   * always be zero).  Again, this is different from .sbss, which contains
   * small non-initialized, non-constant global and static data.
   */
  .sbss2 ALIGN(4)   :
  {
    *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
    . = ALIGN(4) ;
  } >kuseg_program_mem

  _program_mem_end = .;

  .dbg_data ORIGIN(kuseg_data_mem) (NOLOAD) :
  {
    . += (DEFINED (_DEBUGGER) ? 0x200 : 0x0);
  } >kuseg_data_mem

  .ram_exchange_data (NOLOAD) :
  {
    _ram_header_addr = . ;
    . += 8 ;
  } >kuseg_data_mem

  /* Persistent data - Use the new C 'persistent' attribute instead. */
  .persist ALIGN(4) (NOLOAD) :
  {
    _persist_begin = .;
    *(.persist .persist.*)
    . = ALIGN(4) ;
    _persist_end = .;
  } >kuseg_data_mem

  ASSERT( _persist_end <= ORIGIN(kuseg_data_mem) + 0x600, "Bootloader does not preserve debug, RAM Header, or persistent data beyond the first 1.5K")

  /*
   *  Note that input sections named .data* are not mapped here.
   *  The best-fit allocator locates them, so that they may flow
   *  around absolute sections as needed.
   */
  .data : AT(_program_mem_end)
  {
    _data_begin = . ;
    *( .gnu.linkonce.d.*)
    KEEP (*(.gnu.linkonce.d.*personality*))

    *(.data1)
    . = ALIGN(4) ;

    _sdata_begin = . ;
    *(.gnu.linkonce.s.*)
    . = ALIGN(4) ;
    _sdata_end = . ;
  } >kuseg_data_mem

  _data_image_begin = LOADADDR(.data) ;
  _gp = . ;
/*  . = . + 0x20 ;*/

  . = ALIGN (4) ;
  _data_end = . ;
  _bss_begin = . ;

  .sbss ALIGN(4)   :
  {
    _sbss_begin = . ;
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    . = ALIGN(4) ;
    _sbss_end = . ;
  } >kuseg_data_mem

  .bss   :
  {
    *(.dynbss)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    /*
     * Align here to ensure that the .bss section occupies space up to
     * _end.  Align after .bss to ensure correct alignment even if the
     * .bss section disappears because there are no input sections.
     */
    . = ALIGN(4) ;
  } >kuseg_data_mem

  . = ALIGN(4) ;
  _end = . ;
  _bss_end = . ;
  _heap = . ;
  /*
   *  The heap and stack are best-fit allocated by the linker after other
   *  data and bss sections have been allocated.
   */
  /*
   * RAM functions go at the end of our stack and heap allocation.
   * Alignment of 2K required by the boundary register (BMXDKPBA).
   *
   * RAM functions are now allocated by the linker. The linker generates
   * _ramfunc_begin and _bmxdkpba_address symbols depending on the
   * location of RAM functions.
   */
  _bmxdudba_address = LENGTH(kuseg_data_mem) ;
  _bmxdupba_address = LENGTH(kuseg_data_mem) ;

    /* The .pdr section belongs in the absolute section */

    /DISCARD/ : { *(.pdr) }
    /* We don't load .reginfo onto the target, so don't locate it
     * in real memory
     */
    /DISCARD/ : { *(.reginfo) }

  .gcc_compiled_long32 : { KEEP(*(.gcc_compiled_long32)) }
  .gcc_compiled_long64 : { KEEP(*(.gcc_compiled_long64)) }

  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0  : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }
  .mdebug.abi32 : { KEEP(*(.mdebug.abi32)) }
  .mdebug.abiN32 : { KEEP(*(.mdebug.abiN32)) }
  .mdebug.abi64 : { KEEP(*(.mdebug.abi64)) }
  .mdebug.abiO64 : { KEEP(*(.mdebug.abiO64)) }
  .mdebug.eabi32 : { KEEP(*(.mdebug.eabi32)) }
  .mdebug.eabi64 : { KEEP(*(.mdebug.eabi64)) }
  /DISCARD/ : { *(.rel.dyn) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) }
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.discard) }

  _stack = ORIGIN(kuseg_data_mem) + LENGTH(kuseg_data_mem);
}

