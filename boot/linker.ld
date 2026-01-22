ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  RAM   (rwx): ORIGIN = 0x20000000, LENGTH = 64K
}

_stack_top = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .isr_vector : {
    KEEP(*(.isr_vector))

  } > FLASH

  .text : {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    } > FLASH

    .data : AT (ADDR(.text) + SIZEOF(.text)) {
      _data_start = .;
      *(.data*)
      _data_end = .;
    } > RAM

    _data_load = LOADADDR(.data);

    .bss : {
      _bss_start = .;
      *(.bss*)
      *(COMMON)
      _bss_end = .;
    } > RAM
}

