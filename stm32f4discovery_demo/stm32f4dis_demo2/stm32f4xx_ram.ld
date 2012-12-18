/* for stm32f4xx */
MEMORY {
/*	rom(xwr)	: ORIGIN = 0x08000000, LENGTH = 1024k	/* internal	flash rom */
/*	ram(xwr)	: ORIGIN = 0x20000000, LENGTH =  128k	/* internal	ram	(able to use w/DMA) */
/*	stack(xwr)	: ORIGIN = 0x10000000, LENGTH =   64k	/* internal	ram	(CCM: MPU access only) */
	rom(xwr)	: ORIGIN = 0x20000000, LENGTH =  128k	/* internal	ram	(able to use w/DMA) */
	ram(xwr)	: ORIGIN = 0x10000000, LENGTH =   60k	/* internal	ram	(CCM: MPU access only) */
	stack(xwr)	: ORIGIN = 0x1000f000, LENGTH =    4k	/* internal	ram	(CCM: MPU access only) */
	nvram(wr)	: ORIGIN = 0x40024000, LENGTH =    4k	/* battery backup memory */
}

_startof_flash = ORIGIN(rom);            /* Start of available flash memory address */
_endof_flash = ORIGIN(rom) + LENGTH(rom);   /* End of available above address + 1 */
_startof_sram = ORIGIN(ram);             /* Start of available SRAM address */
_endof_sram = ORIGIN(ram) + LENGTH(ram);    /* End of available above address + 1 */
_startof_nvram = ORIGIN(nvram);          /* Start of available battery backup SRAM address */
_endof_nvram = ORIGIN(nvram)+LENGTH(nvram); /* End of available above address + 1 */
_startof_stack = ORIGIN(stack);          /* Start of available inner-SRAM address */
_endof_stack = ORIGIN(stack)+LENGTH(stack); /* End of available above address + 1 */

SECTIONS {
	.text : {	FILL(0xff)
		KEEP(*(.VECTOR))       /* Exception vector table */
/*		KEEP(*(.isr_vector)) */
/*		KEEP(*(.init)) */
/*    . = 0x2FC; */
/*    KEEP(*(.CRP)) */          /* CRP value */
		_text =	.;
		PROVIDE(text =	.);	  
		*(.text .text.*)       /* Program code */
		*(.gnu.linkonce.t*)
		*(.gcc_except_table)
		*(.rodata .rodata.*)   /* Read-only data (constants) */
		*(.gnu.linkonce.r*)
	} >rom

	.ARM.excep : {	FILL(0xff) /* Data table for library exception handling */
		__exidx_start =	.;
		*(.ARM.exidx*)
		__exidx_end =	.;
		__extab_start =	.;
		*(.ARM.extab*)
		__exidx_end =	.;
	} >rom

	. = ALIGN(4);
	_etext = .;         /* End of .text section */
	PROVIDE(etext =	.);	  

	_sidata = .;        /* Start of the initialization values of the .data section */

	.data : AT (_sidata) {	FILL(0xff)
		. = ALIGN(4);
		_sdata = .;
		*(.data .data.*)
		*(.gnu.linkonce.d*)
		. = ALIGN(4);
		_edata = .;
		PROVIDE(edata =	.);	  
	} >ram

	.bss (NOLOAD) : ALIGN(4) {
		_sbss = .;
		__bss_start = .;
		__bss_start__ = .;
		*(.bss .bss.*)
		*(.gnu.linkonce.b*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		_heap  = .;
		PROVIDE(heap =	.);
		_bss_end__ = . ;
		__bss_end__ = . ;
		__end__ = . ;
		PROVIDE (end = .);
		_end  = .;
	} >ram

	.nonvolatile (NOLOAD) : ALIGN(4) {
		_snonvolatile = .;
		*(.nonvolatile .nonvolatile.*)
		_enonvolatile = .;
	} >nvram

	.stack (NOLOAD) : ALIGN(8) {
		*(.stack)
		*(.STACK)
		. = ALIGN(4);
	} >stack

	. = ALIGN(4);

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
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
}

