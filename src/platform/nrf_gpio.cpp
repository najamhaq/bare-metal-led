//
// Created by najamhaq on 2026-01-23.
//

#include "nrf_gpio.h"

void set_high(gpio_t gpio)
{
  if (gpio.port == 0)
  {
    P0_OUTSET = BIT(gpio.pin);
    return;
  }
  P1_OUTSET = BIT(gpio.pin);
}

void set_low(gpio_t gpio)
{
  if (gpio.port == 0)
  {
    P0_OUTCLR = BIT(gpio.pin);
    return;
  }
  P1_OUTCLR = BIT(gpio.pin);
}
