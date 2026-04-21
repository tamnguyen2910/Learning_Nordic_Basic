# BH1750 Light Sensor with LED Indicator

## Overview
This project demonstrates how to interface the BH1750 ambient light sensor with a Pro Micro nRF52840 board using the Zephyr RTOS. The system reads ambient light via I2C and controls an onboard LED based on the measured lux value.

- **Board:** Pro Micro nRF52840 (nRF52840 SoC)
- **Sensor:** BH1750 (Digital Light Sensor)
- **RTOS:** Zephyr
- **Functionality:**
  - Reads ambient light (lux) from BH1750 via I2C0
  - Turns LED0 ON if lux < 20 (dark environment)
  - Turns LED0 OFF if lux >= 20 (bright environment)

---

## Hardware Connections

| BH1750 Pin | nRF52840 Pin         |
|------------|---------------------|
| VCC        | 3.3V                |
| GND        | GND                 |
| SDA        | P1.00 (I2C0 SDA)    |
| SCL        | P0.11 (I2C0 SCL)    |
| ADDR       | GND (I2C Addr 0x23) |

- **LED0:** On-board red LED at P0.15

---

## Project Structure

```
03_I2CwithBH1750/
├── CMakeLists.txt
├── prj.conf
├── promicro_nrf52840_nrf52840.overlay
├── src/
│   └── main.c
├── build/
└── ...
```

- `CMakeLists.txt`: Build configuration for Zephyr
- `prj.conf`: Zephyr project configuration
- `promicro_nrf52840_nrf52840.overlay`: Device tree overlay for board and peripherals
- `src/main.c`: Main application source code

---

## How It Works

1. **Initialization:**
   - Configures LED0 as output
   - Initializes I2C0 bus
   - Powers on and configures BH1750 sensor
2. **Main Loop:**
   - Reads light intensity from BH1750 every 1 second
   - Converts raw sensor value to lux
   - Turns LED0 ON if lux < 20, otherwise turns it OFF
   - Prints lux value and LED status to console

---

## Build & Flash Instructions

### Prerequisites
- [Zephyr SDK](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
- nRF Connect SDK (for nRF52840)
- West tool (Zephyr build system)

### Build
```sh
west build -b promicro_nrf52840 .
```

### Flash
```sh
west flash
```

---

## Configuration
- **Light Threshold:** Set in `main.c` (`#define LUX_THRESHOLD 20`)
- **Read Interval:** Set in `main.c` (`#define READ_INTERVAL 1000` ms)
- **I2C Address:** 0x23 (ADDR pin to GND)

---

## References
- [BH1750 Datasheet](https://www.mouser.com/datasheet/2/348/bh1750fvi-e-186247.pdf)
- [Zephyr I2C API](https://docs.zephyrproject.org/latest/hardware/peripherals/i2c.html)
- [Zephyr GPIO API](https://docs.zephyrproject.org/latest/hardware/peripherals/gpio.html)

---

