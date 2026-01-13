.syntax unified
.cpu cortex-m4
.thumb

.global g_pfnVectors
.global Reset_Handler
.global Timer_Handler
.extern main
.extern SysTick_Handler

/* 128 KB RAM @ 0x20000000 -> top = 0x20020000 */
.equ _estack, 0x20020000

.section .isr_vector, "a", %progbits
g_pfnVectors:
  .word _estack
  .word Reset_Handler + 1    /* <-- IMPORTANT: Thumb bit set */
  .word Default_Handler + 1
  .word Default_Handler + 1
  .word Default_Handler + 1
  .word Default_Handler + 1
  .word Default_Handler + 1
  .word 0
  .word 0
  .word 0
  .word 0
  .word Default_Handler + 1
  .word Default_Handler + 1
  .word 0
  .word Default_Handler + 1
  .word SysTick_Handler + 1 

  /* IRQs */
  .rept 48
    .word Default_Handler + 1
  .endr

.section .text.Reset_Handler, "ax", %progbits
.thumb_func
Reset_Handler:
  bl main
  b .

.section .text.Default_Handler, "ax", %progbits
.thumb_func
Default_Handler:
  b .
