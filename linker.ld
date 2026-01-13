ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 512K
  RAM   (rwx): ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
  } > FLASH

  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    *(.data*)
  } > RAM

  .bss :
  {
    *(.bss*)
    *(COMMON)
  } > RAM
}
