//
// Created by najamhaq on 2026-01-23.
//
#include "nrf_gpio.h"
#include "buttons.h"


void buttons_init()
{
  // Make pins inputs
  P0_DIRCLR = (1u << BTN_A_PIN) | (1u << BTN_B_PIN);

  // Enable pull-ups (so released reads as 1)
  P0_PIN_CNF(BTN_A_PIN) =
      (PIN_CNF_DIR_Input     << PIN_CNF_DIR_Pos) |
      (PIN_CNF_INPUT_Connect << PIN_CNF_INPUT_Pos) |
      (PIN_CNF_PULL_Pullup   << PIN_CNF_PULL_Pos);

  P0_PIN_CNF(BTN_B_PIN) =
      (PIN_CNF_DIR_Input     << PIN_CNF_DIR_Pos) |
      (PIN_CNF_INPUT_Connect << PIN_CNF_INPUT_Pos) |
      (PIN_CNF_PULL_Pullup   << PIN_CNF_PULL_Pos);
}



bool button_a_pressed()
{
  return ((P0_IN & (1u << BTN_A_PIN)) == 0u);
}
bool button_b_pressed()
{
  return ((P0_IN & (1u << BTN_B_PIN)) == 0u);
}
