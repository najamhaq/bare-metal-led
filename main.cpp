#include <stdint.h>


#if defined(DEBUG) || defined(_DEBUG)
  #define DBG_ANCHOR() __asm volatile("nop")
#else
  #define DBG_ANCHOR() do {} while(0)
#endif

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

#define SYST_CSR (*(volatile uint32_t*)0xE000E010)
#define SYST_RVR (*(volatile uint32_t*)0xE000E014)
#define SYST_CVR (*(volatile uint32_t*)0xE000E018)



static inline void delay_cycles(volatile uint32_t n) {
    while (n--) __asm volatile ("nop");
}
static inline uint32_t BIT(uint32_t pin) {return (1u << (pin)); }

typedef struct {
    uint8_t port;  // 0 or 1
    uint8_t pin;   // 0..31 (port0) or 0..15 (port1)
} gpio_t;




static const gpio_t col_pins[5] = {
    {0, 28}, {0, 11}, {0, 31}, {1, 5}, {0, 30}
};
static const gpio_t row_pins[5] = {
    {0, 21}, {0, 22}, {0, 15}, {0, 24}, {0, 19}
};

static void set_high(gpio_t gpio) {
    if (gpio.port == 0) {
        P0_OUTSET = BIT(gpio.pin);
        return;
    }
    P1_OUTSET = BIT(gpio.pin);
}

static void set_low(gpio_t gpio) {
    if (gpio.port == 0) {
        P0_OUTCLR = BIT(gpio.pin);
        return;
    }
    P1_OUTCLR = BIT(gpio.pin);
}

static volatile uint32_t ms = 0;
static volatile uint32_t led = 0;
static volatile uint32_t ticker = 0;

void init_systick(uint32_t ticks) {
    SYST_RVR = ticks - 1; // Set reload register
    SYST_CVR = 0;        // Load the SysTick Counter Value
    SYST_CSR = 0x07;        // ENABLE(1) | TICKINT(2) | CLKSOURCE(4=CPU)
    ticker = 0;
    ms = 0;
}

void animation_step() {
    // Turn all LED off
    for (int i = 0; i < 5; i++) {
        set_high(col_pins[i]); // Turn it OFF
        set_low(row_pins[i]); // Turn it OFF
    }

    for (int i = 0; i < 5; i++) {
        set_low(col_pins[i]); // Turn it ON
        set_high(row_pins[i]); // Turn it ON
        delay_cycles(50000 * 100);
    }
    for (int i = 0; i < 5; i++) {
        set_high(col_pins[i]); // Turn it OFF
        set_low(row_pins[i]); // Turn it OFF
        delay_cycles(50000 * 100);
    }
}

void led_on(uint32_t row, uint32_t col) {
    set_low(col_pins[col]); // Turn it ON
    set_high(row_pins[row]); // Turn it ON
}   

void led_off(uint32_t row, uint32_t col) {
    set_high(col_pins[col]); // Turn it OFF
    set_low(row_pins[row]); // Turn it OFF
}

void blink_led(uint32_t row, uint32_t col, uint32_t delay__ms = 300) {
    // Turn all LED off
    led_on(row, col);
    delay_cycles(delay__ms * 100);
    led_off(row, col);
    delay_cycles(delay__ms * 100);  // this may not be necessary , unless we are clalling multipl blin at the same time
}

void tick() {
    blink_led(2,1);
}


void led_test(){
    set_low(col_pins[3]); // Turn it ON
    set_high(row_pins[3]); // Turn it ON

}

extern "C" void SysTick_Handler(void){
    ms++;
    if (ms >= 5000) { // every 30 seconds
        ms = 0;
        ticker = 1;
    }

}



int main(void) {
    // Configure row + columns as outputs
    P0_DIRSET = 0xFFFFFFFF; // All GPIO as outputs
    P1_DIRSET = (1u << 5);   // Port 1: make P1.05 an output

    for (int i = 0; i < 5; i++) {
        set_high(col_pins[i]); // Default HIGH (OFF)
        set_low(row_pins[i]); // Default LOW (OFF)
    }
    init_systick(63999); // 1ms at 64 MHz

    //led_test();
     while (1) {
        __asm volatile("nop");   // anchor for stepping
        if (ticker) {
            tick();
            ticker = 0;
        }
        /* nothing */
    }
}


