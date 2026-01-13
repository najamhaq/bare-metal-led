# Bare-Metal ARM Cortex-M Firmware (micro:bit V2 / nRF52833)

## Overview

This repository contains a **bare-metal firmware project** targeting the **BBC micro:bit V2**, built directly for the **Nordic nRF52833 (ARM Cortex-M4F)** microcontroller.

The objective of this project is **not** to rely on vendor frameworks or runtime libraries, but to rebuild a precise and modern embedded-systems mental model by working directly with:

- Memory-mapped GPIO registers  
- Custom startup code and linker scripts  
- Interrupt-driven timing  
- Hardware-level LED matrix control  

This reflects how production firmware operates *underneath* higher-level abstractions. Though  no high-level abstraction is used here, the code is structured and commented for clarity and maintainability.

---

## Hardware

- **Board:** BBC micro:bit V2 (development board only)  
- **Microcontroller:** Nordic nRF52833  
- **CPU Core:** ARM Cortex-M4F (ARMv7E-M)  
- **Clock:** Default HFCLK  
- **Programming Method:** DAPLink USB drag-and-drop (`.hex`)  

> The micro:bit is used strictly as a **carrier board**.  
> All firmware targets the **nRF52833 MCU directly**.

---

## Design Philosophy

### Bare-Metal by Intent

This project intentionally avoids:

- CODAL / micro:bit runtime  
- mbed  
- CMSIS HAL drivers  
- Arduino-style APIs  
- RTOS (for now)

Instead, it includes:

- Custom `startup.s` (vector table + reset handler)  
- Custom `linker.ld`  
- Freestanding C/C++ (`-ffreestanding -nostdlib`)  
- Direct peripheral access via memory-mapped registers  

This ensures full visibility into:

- Reset and boot flow  
- Stack initialization  
- Exception and interrupt handling  
- Peripheral side effects  

---

## LED Matrix Control

The micro:bit’s 5×5 display is a **multiplexed LED matrix**, not 25 independent LEDs.

### Electrical Model

- **Rows (anodes):** Driven HIGH  
- **Columns (cathodes):** Driven LOW  
- LEDs illuminate at **ROW × COLUMN intersections**  
- Only one row is active at a time during scanning  

### GPIO Reality

Row and column pins are **not sequential GPIO numbers**.  
They are wired based on PCB routing constraints, not logical LED layout.

Example mappings:

- **Rows:** `P0.15, P0.19, P0.21, P0.22, P0.24`  
- **Columns:** `P0.11, P0.28, P0.30, P0.31, P1.05`  

A logical `(row, column)` abstraction is mapped explicitly to physical GPIO pins.

---

## GPIO Access Model

The nRF52833 uses **memory-mapped GPIO**.

Each port exposes:

- `OUT` — absolute output latch  
- `OUTSET` — write-1-to-set  
- `OUTCLR` — write-1-to-clear  
- `DIRSET` — configure pins as outputs  
- `DIRCLR` — configure pins as inputs  

This project uses:

- `OUT` for deterministic initialization  
- `OUTSET / OUTCLR` for atomic runtime control  

No software shadow registers are required.

---

## Timing and Animation

Initial bring-up uses short busy-wait delays for validation only.

Planned / implemented concepts include:

- SysTick or hardware TIMER interrupts  
- Blocking LED animations  
- Interrupt-driven button handling  
- Time-based debounce logic  

This involves polling and demonstrates real firmware timing discipline.

---

## Project Structure

```
mb2-baremetal/
│
├── CMakeLists.txt
├── arm-gcc-toolchain.cmake
├── linker.ld
├── startup.s
├── main.cpp
│
├── flash.ps1
├── clean.cmd
├── build/        (generated, not versioned)
└── README.md
```

---

## Build and Flash

- Toolchain: **GNU Arm Embedded Toolchain**
- Build system: **CMake** (generator-agnostic)
- Output: `.hex`
- Flashing: USB drag-and-drop via DAPLink Supported
- Flashing via pyocd

Exact build backend (Ninja / CMake ) is intentionally decoupled from firmware logic.

---

## What This Project Demonstrates

- Clear separation of ARM CPU vs MCU peripherals  
- Correct use of memory-mapped I/O  
- Deterministic GPIO initialization  
- LED matrix multiplexing  
- Timer-driven state machines  
- Button input with debounce  
- Button-controlled animation
- Queue-based event handling
- Unit Testing of queue implementation

---
## Shortcomings
 We are well aware of the following limitations in the current implementation:
  
- No RTOS or advanced scheduling yet
- Basic error handling and edge cases not fully covered
- Polling of Button states cause missing button presses during LED updates.
- No power management or low-power modes implemented

## Next Steps

Planned extensions:

- Full 5×5 LED scan loop using timer interrupts  
- Button-controlled animation pause/resume  
- SysTick vs hardware TIMER comparison  
- Optional RTOS introduction after fundamentals  

---

## License

Educational and demonstrational use only.
