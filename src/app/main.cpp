#include <stdint.h>

#include "buttons.h"
#include "led_matrix.h"
#include "message.h"
#include "nrf_gpio.h"
#include "queue.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DBG_ANCHOR() __asm volatile("nop")
#else
#define DBG_ANCHOR()                                                           \
  do {                                                                         \
  } while (0)
#endif

/***************************************
 * All static data is defined here
 ****************************************/
static volatile uint32_t ms = 0;
static volatile uint32_t led = 0;
static volatile uint32_t ticker = 0;
Message messages[40];
Queue queue;
uint32_t lastButton = 0;

void init_systick(uint32_t ticks) {
  SYST_RVR = ticks - 1; // Set reload register
  SYST_CVR = 0;         // Load the SysTick Counter Value
  SYST_CSR = 0x07;      // ENABLE(1) | TICKINT(2) | CLKSOURCE(4=CPU)
  ticker = 0;
  ms = 0;
}

void tick() { blink_led(4, 4, 300); }

extern "C" [[maybe_unused]] void SysTick_Handler(void) {
  Message tickMessage;
  ms++;
  bool buttonA = button_a_pressed();
  if (buttonA && ((ms - lastButton) > 1000)) {
    // dont activate for next 100 ms
    Message msg = {
        .type = MessageType::MESSAGE_TYPE_BUTTON_A, .param = 0, .data = 0};
    lastButton = ms;
    queue.push(msg);
  }

  if (ms >= 5000) {
    // every 30 seconds
    ms = 0;
    tickMessage.type = MessageType::MESSAGE_TYPE_TICK;
    tickMessage.param = 0;
    tickMessage.data = 0;
    queue.push(tickMessage);
    ticker = 1;
  }
}

void setup() {
  // Configure row + columns as outputs
  led_matrix_init();
  buttons_init();
  init_systick(63999); // 1ms at 64 MHz
}

void super_loop() {
  while (true) {
    Message msg;
    __asm volatile("nop"); // anchor for stepping
    if (!queue.isEmpty()) {
      bool popSuccess = queue.pop(msg);
      if (!popSuccess) {
        continue;
      }
      switch (msg.type) {
      case MessageType::MESSAGE_TYPE_BUTTON_A:
        animation_step();
        break;
      // nothing
      case MessageType::MESSAGE_TYPE_TICK:
        tick();
      default:
        break;
      }
    }
  }
  /* nothing */
}

int main(void) {
  // if we had memory management , we could use constructor
  queue.init(messages, sizeof(messages) / sizeof(messages[0]));
  setup();
  super_loop();
  return 0;
}