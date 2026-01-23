//
// Created by najamhaq on 2026-01-18.
//


#include "irq.h"

#include "irq.h"
#include <pthread.h>

static pthread_mutex_t g_irq_lock = PTHREAD_MUTEX_INITIALIZER;

irq_state_t irq_save_disable(void)
{
  pthread_mutex_lock(&g_irq_lock);
  return 0;
}

void irq_restore(irq_state_t state)
{
  (void)state;
  pthread_mutex_unlock(&g_irq_lock);
}
