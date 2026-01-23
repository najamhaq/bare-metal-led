//
// Created by najamhaq on 2026-01-18.
//

#include "include/irq.h"

irq_state_t irq_save_disable(void)
{
  irq_state_t primask;
  __asm volatile ("mrs %0, primask" : "=r"(primask));
  __asm volatile ("cpsid i" : : : "memory");
  return primask;
}

void irq_restore(irq_state_t state)
{
  __asm volatile ("msr primask, %0" : : "r"(state) : "memory");
}
