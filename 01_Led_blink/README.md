# Pro Micro nRF52840 GPIO Demo

A multi-threaded Zephyr RTOS application demonstrating GPIO control on the Pro Micro nRF52840 board.

## Features

- **LED0 Blink**: Toggles on-board red LED (P0.15) every 1000ms
- **GPIO Toggle**: Toggles all available GPIO pins every 500ms
- **Multi-threading**: Uses Zephyr RTOS threads for concurrent operation
- **UART0 Disabled**: P0.09 and P0.10 freed for GPIO use

## Hardware Requirements

- **Board**: Pro Micro nRF52840 (nRF52840 SoC)
- **On-board LED**: P0.15 (red LED)
- **GPIO Pins**: All available GPIO pins (see pin mapping below)

## Pin Mapping

### Port 0 (P0.00 - P0.31)
| Pin | Function | Notes |
|-----|----------|-------|
| P0.02 | GPIO | Toggles every 500ms |
| P0.06 | GPIO | Toggles every 500ms |
| P0.08 | GPIO | Toggles every 500ms |
| P0.09 | GPIO | Formerly UART0 TX, now GPIO |
| P0.10 | GPIO | Formerly UART0 RX, now GPIO |
| P0.11 | GPIO | Toggles every 500ms |
| P0.12 | GPIO | Toggles every 500ms |
| P0.15 | LED0 | On-board red LED, toggles every 1000ms |
| P0.17 | GPIO | Toggles every 500ms |
| P0.20 | GPIO | Toggles every 500ms |
| P0.22 | GPIO | Toggles every 500ms |
| P0.24 | GPIO | Toggles every 500ms |
| P0.29 | GPIO | Toggles every 500ms |
| P0.31 | GPIO | Toggles every 500ms |

### Port 1 (P1.00 - P1.15)
| Pin | Function | Notes |
|-----|----------|-------|
| P1.00 | GPIO | Toggles every 500ms |
| P1.04 | GPIO | Toggles every 500ms |
| P1.06 | GPIO | Toggles every 500ms |
| P1.11 | GPIO | Toggles every 500ms |
| P1.13 | GPIO | Toggles every 500ms |
| P1.15 | GPIO | Toggles every 500ms |

## Configuration

### Device Tree Overlay
The overlay file `promicro_nrf52840_nrf52840.overlay` disables UART0 to free P0.09 and P0.10 for GPIO use:

```dts
&uart0 {
    status = "disabled";
};
```

### Kconfig (prj.conf)
```ini
CONFIG_GPIO=y
CONFIG_SERIAL=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_BUILD_OUTPUT_UF2=y
CONFIG_I2C=y
```

## Source Code Structure

### `src/main.c` - Main Application

#### 1. Configuration Defines
```c
#define LED_PERIOD_MS          1000    // LED0 toggle period
#define GPIO_TOGGLE_PERIOD_MS  500     // GPIO pins toggle period
#define STACK_SIZE             512     // Thread stack size
```

#### 2. Pin Definitions
- Port 0: 14 pins (P0.02, P0.06, P0.08, P0.09, P0.10, P0.11, P0.12, P0.17, P0.20, P0.22, P0.24, P0.29, P0.31)
- Port 1: 6 pins (P1.00, P1.04, P1.06, P1.11, P1.13, P1.15)

#### 3. Thread Functions

**LED Blink Thread** (`led_blink_thread()`):
- Toggles LED0 (P0.15) every 1000ms
- Prints LED state to console
- Runs in separate thread with priority 5

**GPIO Toggle Thread** (`gpio_toggle_thread()`):
- Toggles all GPIO pins every 500ms
- Prints GPIO state to console
- Runs in separate thread with priority 5

#### 4. Initialization Flow
1. **Device Tree Initialization**: Loads LED0 alias and GPIO devices
2. **GPIO Configuration**: Configures all pins as output, initially inactive
3. **Thread Creation**: Starts LED blink and GPIO toggle threads
4. **Main Loop**: Main thread sleeps while child threads handle GPIO operations

## Build and Flash

### Prerequisites
- nRF Connect SDK v3.2.4 or later
- Zephyr RTOS toolchain
- J-Link or CMSIS-DAP debugger

### Build Commands
```bash
# Clean build
west build -b promicro_nrf52840 -p always

# Incremental build
west build -b promicro_nrf52840

# Build with multi-core (faster)
west build -b promicro_nrf52840 -o=-j8
```

### Flash Commands
```bash
# Flash via debugger
west flash

# Flash via UF2 (Pro Micro)
# 1. Double reset board
# 2. Copy build/zephyr/zephyr.uf2 to USB drive
```

## Output

### Console Output (via J-Link RTT)
```
========================================
Pro Micro nRF52840 GPIO Demo
LED Period: 1000 ms, GPIO Period: 500 ms
========================================
GPIO pins initialized successfully
Threads started successfully
[LED Thread] Started - Period: 1000 ms
[GPIO Thread] Started - Period: 500 ms
[GPIO Thread] Port 0 pins: 14, Port 1 pins: 6
[LED] ON
[GPIO] All pins HIGH
[GPIO] All pins LOW
[LED] OFF
[GPIO] All pins HIGH
...
```

## Troubleshooting

### P0.09 and P0.10 Not Toggling
1. Verify UART0 is disabled in overlay file
2. Check that `CONFIG_UART_CONSOLE` is not forcing UART pins
3. Ensure no other peripheral is using these pins

### No Console Output
- UART0 is disabled, so no serial console output
- Use J-Link RTT for debug output
- Or enable UART0 with different pins in overlay

### Build Errors
```bash
# Clean build directory
west build -t clean

# Reconfigure project
west build -t reconfigure
```

## Customization

### Change Toggle Periods
```c
// In src/main.c
#define LED_PERIOD_MS          2000    // Change to 2 seconds
#define GPIO_TOGGLE_PERIOD_MS  250     // Change to 250ms
```

### Add More GPIO Pins
1. Add pin definition in `Port 0 pins` section
2. Add to `port0_pins[]` array
3. Rebuild and flash

### Re-enable UART Console
1. Remove `&uart0 { status = "disabled"; }` from overlay
2. Remove P0.09 and P0.10 from `port0_pins[]` array
3. Rebuild and flash

## Project Structure
```
01_Led_blink/
├── src/
│   └── main.c              # Main application source
├── promicro_nrf52840_nrf52840.overlay  # Device tree overlay
├── prj.conf                # Kconfig settings
├── CMakeLists.txt          # Build configuration
└── README.md               # This file
```

## License
SPDX-License-Identifier: Apache-2.0
Copyright (c) 2016 Intel Corporation
