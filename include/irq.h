//
// Created by najamhaq on 2026-01-18.
//

#ifndef MICROBIT_BAREMETAL_IRQ_H
#define MICROBIT_BAREMETAL_IRQ_H



#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef uint32_t irq_state_t;

  // Disable interrupts (or equivalent). Returns previous state.
  irq_state_t irq_save_disable(void);

  // Restore previous state.
  void irq_restore(irq_state_t state);

#ifdef __cplusplus
}
#endif


#endif //MICROBIT_BAREMETAL_IRQ_H