# BLE Light Sensor Controller

A Zephyr RTOS-based BLE peripheral application for nRF52840 that reads ambient light sensor (BH1750), displays data on OLED, and allows remote LED control via BLE.

## Features

- **BLE Peripheral**: Advertises as "promicro nrf52840", accepts BLE connections
- **BH1750 Light Sensor**: Reads ambient light (lux) via I2C every 1 second
- **SSD1309 OLED Display**: 128x64 monochrome display showing:
  - BLE connection status (Connected/Scanning)
  - LED status (ON/OFF)
  - Current lux reading
- **BLE Remote Control**: Control onboard LED via BLE GATT write commands
- **BLE Notifications**: Real-time lux data push to connected app
- **LED Visual Feedback**: Onboard LED blinks during scanning, fast blinks on connection

## Hardware

| Component | Details |
|-----------|---------|
| **Board** | Adafruit Pro Micro nRF52840 |
| **MCU** | Nordic nRF52840 (ARM Cortex-M4, 64MHz, 256KB Flash, 64KB RAM) |
| **Display** | SSD1309 OLED 128x64 (I2C address 0x3C) |
| **Sensor** | BH1750 Ambient Light Sensor (I2C address 0x23) |
| **LED** | Onboard LED (P0.15) |

### Pin Connections

| Function | Pin |
|----------|-----|
| I2C SDA | P1.00 |
| I2C SCL | P0.11 |
| LED | P0.15 |

## Architecture

```
src/
├── main.c                    # Entry point
├── app/
│   ├── app.h                 # Application interface
│   └── app.c                 # Module initialization & orchestration
├── hal/
│   ├── led.h                 # LED hardware interface
│   └── led.c                 # LED driver (GPIO control)
├── drivers/
│   ├── display.h / display.c # SSD1309 OLED driver
│   ├── font.h / font.c       # 8x8 bitmap font renderer
│   ├── sensor.h / sensor.c   # BH1750 sensor driver
│   └── sensor.c
└── bluetooth/
    ├── ble.h / ble.c         # BLE peripheral stack & advertising
    └── led_service.h / led_service.c  # GATT service (LED + Lux)
```

## BLE GATT Service

### Service UUID: `0xABCD`

| Characteristic | UUID | Properties | Description |
|----------------|------|------------|-------------|
| LED Control | `0xABCE` | Write | Write `0x00` = OFF, `0x01` = ON |
| Lux Sensor | `0xABCF` | Read + Notify | Returns `uint32` (lux × 100) |

### Data Format

**Lux Value** (`0xABCF`):
- Format: `uint32_t` (little-endian)
- Value: `lux × 100` (e.g., 22.25 lux = 2225)
- Example: `0xA9 0x08 0x00 0x00` = 2225 = 22.25 lux

## Building

### Prerequisites

- [Nordic SDK v3.2.4](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v3.x.x/nRF5_SDK_3.2.4.zip)
- [Zephyr RTOS](https://zephyrproject.org/) (included in Nordic SDK)
- CMake, Ninja, GCC ARM toolchain

### Build Commands

```bash
# Set environment
export ZEPHYR_BASE=/path/to/ncs/v3.2.4/zephyr
export BOARD_ROOT=/path/to/ncs/v3.2.4/nrf

# Clean build
rm -rf build

# Configure
cmake -S . -B build -GNinja \
  -DBOARD=promicro_nrf52840/nrf52840 \
  -DCONF_FILE=prj.conf \
  -DDTC_OVERLAY_FILE=promicro_nrf52840_nrf52840.overlay

# Build
cmake --build build
```

### Output Files

| File | Description |
|------|-------------|
| `build/05_BLE/zephyr/zephyr.uf2` | UF2 (drag-and-drop flashing) |
| `build/05_BLE/zephyr/zephyr.hex` | HEX |
| `build/05_BLE/zephyr/zephyr.bin` | Raw binary |

## Flashing

### Method 1: UF2 (Drag & Drop)
1. Double-tap reset button on Pro Micro to enter UF2 mode
2. Drag `zephyr.uf2` to the BOOT drive

### Method 2: J-Link / nRF Connect
1. Open nRF Connect Programmer
2. Select J-Link / Nordic Segger
3. Erase and write `zephyr.hex`

## Usage

1. **Power up** - Board starts BLE advertising
2. **Connect** - Use nRF Connect or your app to scan and connect to "promicro nrf52840"
3. **View Lux** - Subscribe to characteristic `0xABCF` to receive lux notifications
4. **Control LED** - Write to characteristic `0xABCE`:
   - `0x00` → LED OFF
   - `0x01` → LED ON
5. **Monitor** - Watch the OLED display for real-time status

## Concurrency Model

Uses **Zephyr Work Queue** (event-driven, not RTOS threads):

| Component | Mechanism | Interval |
|-----------|-----------|----------|
| LED blink | `k_work_delayable` | 100ms ON / 900ms OFF |
| Sensor read | `k_work_delayable` | 1 second |
| BLE reconnect | `k_work_delayable` | 500ms retry |

Benefits:
- Low RAM usage (no thread stacks)
- Power efficient (runs in idle thread)
- Simple, event-driven code

## Error Handling

- **Sensor**: 3 retries with 50ms delay, logs warning/error
- **BLE**: Auto-reconnect on disconnect (500ms delay)
- **Display**: Graceful failure handling with error logs
