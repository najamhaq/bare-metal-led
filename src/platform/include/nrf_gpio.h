//
// Created by najamhaq on 2026-01-23.
//

#ifndef MICROBIT_BAREMETAL_SRC_PLATFORM_NRF_GPIO_H
#define MICROBIT_BAREMETAL_SRC_PLATFORM_NRF_GPIO_H

#include <stdint.h>

#define GPIO_0_BASE   0x50000000UL
#define GPIO_1_BASE   0x50000300UL

#define OUTSET      (*(volatile uint32_t*)(GPIO_0_BASE + 0x508))
#define OUTCLR      (*(volatile uint32_t*)(GPIO_0_BASE + 0x50C))

#define P0_DIRSET (*(volatile uint32_t *)(GPIO_0_BASE + 0x518))
#define P1_DIRSET (*(volatile uint32_t *)(GPIO_1_BASE + 0x518))  // 0x50000300 + 0x518 = 0x50000818


#define P0_OUTSET (*(volatile uint32_t *)(GPIO_0_BASE + 0x508))
#define P0_OUTCLR (*(volatile uint32_t *)(GPIO_0_BASE + 0x50C))
#define P1_OUTSET (*(volatile uint32_t *)(GPIO_1_BASE + 0x508))
#define P1_OUTCLR (*(volatile uint32_t *)(GPIO_1_BASE + 0x50C))

#define P0_IN     (*(volatile uint32_t *)(GPIO_0_BASE + 0x510))
#define P0_DIRCLR (*(volatile uint32_t *)(GPIO_0_BASE + 0x51C))
#define P0_PIN_CNF(n) (*(volatile uint32_t *)(GPIO_0_BASE + 0x700 + 4u*(n)))

#define BTN_A_PIN 14u
#define BTN_B_PIN 23u

// PIN_CNF bits (nRF style):
// DIR=0 (input), PULL=3 (pull-up) in many nRF headers,
// but since you don't have enums, we'll set by bit positions:

#define PIN_CNF_DIR_Pos   0u
#define PIN_CNF_PULL_Pos  2u
#define PIN_CNF_INPUT_Pos 1u

#define PIN_CNF_DIR_Input     (0u)
#define PIN_CNF_INPUT_Connect (0u)
#define PIN_CNF_PULL_Pullup   (3u)


#define SYST_CSR (*(volatile uint32_t*)0xE000E010)
#define SYST_RVR (*(volatile uint32_t*)0xE000E014)
#define SYST_CVR (*(volatile uint32_t*)0xE000E018)




typedef struct {
  uint8_t port; // 0 or 1
  uint8_t pin;  // pin number
} gpio_t;

static inline uint32_t BIT(uint32_t pin) { return (1u << pin); }

void gpio_make_output(gpio_t p);
void gpio_set(gpio_t p);
void gpio_clear(gpio_t p);

void gpio_p0_dirset(uint32_t mask);
void gpio_p1_dirset(uint32_t mask);

uint32_t gpio_p0_in();
void gpio_p0_dirclr(uint32_t mask);
void gpio_p0_pin_cnf(uint32_t pin, uint32_t value);



void set_high(gpio_t gpio);
void set_low(gpio_t gpio);

#endif // MICROBIT_BAREMETAL_SRC_PLATFORM_NRF_GPIO_H
