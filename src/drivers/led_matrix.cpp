//
// Created by najamhaq on 2026-01-23.
//
#include <stdint.h>
#include "led_matrix.h"
#include "nrf_gpio.h"
#include <stdint.h>

static const gpio_t col_pins[5] = {{0, 28}, {0, 11}, {0, 31}, {1, 5}, {0, 30}};
static const gpio_t row_pins[5] = {{0, 21}, {0, 22}, {0, 15}, {0, 24}, {0, 19}};

static inline void delay_cycles(volatile uint32_t n) {
  while (n--)
    __asm volatile("nop");
}

void led_on(uint32_t row, uint32_t col) {
  set_low(col_pins[col]);  // Turn it ON
  set_high(row_pins[row]); // Turn it ON
}

void led_off(uint32_t row, uint32_t col) {
  set_high(col_pins[col]); // Turn it OFF
  set_low(row_pins[row]);  // Turn it OFF
}

void animation_step() {
  // Turn all LED off
  for (int i = 0; i < 5; i++) {
    set_high(col_pins[i]); // Turn it OFF
    set_low(row_pins[i]);  // Turn it OFF
  }

  for (int i = 0; i < 5; i++) {
    set_low(col_pins[i]);  // Turn it ON
    set_high(row_pins[i]); // Turn it ON
    delay_cycles(50000 * 100);
  }
  for (int i = 0; i < 5; i++) {
    set_high(col_pins[i]); // Turn it OFF
    set_low(row_pins[i]);  // Turn it OFF
    delay_cycles(50000 * 100);
  }
}

void blink_led(uint32_t row, uint32_t col, uint32_t delay__ms) {
  // Turn all LED off
  led_on(row, col);
  delay_cycles(delay__ms * 100);
  led_off(row, col);
  delay_cycles(delay__ms * 100); // this may not be necessary , unless we are
                                 // clalling multipl blin at the same time
}

void led_test() {
  set_low(col_pins[3]);  // Turn it ON
  set_high(row_pins[3]); // Turn it ON
}

void led_matrix_init() {
  // at present we dont care about inputs, so make all output
  P0_DIRSET = 0xFFFFFFFF; // All GPIO as outputs
  P1_DIRSET = (1u << 5);  // Port 1: make P1.05 an output
  for (int i = 0; i < 5; i++) {
    set_high(col_pins[i]); // Default HIGH (OFF)
    set_low(row_pins[i]);  // Default LOW (OFF)
  }
}
