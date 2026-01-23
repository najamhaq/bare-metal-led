//
// Created by najamhaq on 2026-01-23.
//

#ifndef MICROBIT_BAREMETAL_SRC_DRIVERS_LED_MATRIX_H
#define MICROBIT_BAREMETAL_SRC_DRIVERS_LED_MATRIX_H


void led_on(uint32_t row, uint32_t col);
void led_off(uint32_t row, uint32_t col);
void animation_step();
void blink_led(uint32_t row, uint32_t col, uint32_t delay__ms = 300);
void led_test();
void led_matrix_init();

#endif // MICROBIT_BAREMETAL_SRC_DRIVERS_LED_MATRIX_H
