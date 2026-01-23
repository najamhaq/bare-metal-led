//
// Created by najamhaq on 2026-01-18.
//

#ifndef MICROBIT_BAREMETAL_IRQ_ARM_H
#define MICROBIT_BAREMETAL_IRQ_ARM_H
#include "irq.h"

irq_state_t irq_save_disable(void) ;
void irq_restore(irq_state_t state);

#endif //MICROBIT_BAREMETAL_IRQ_ARM_H