# I2C with LCD Display on Pro Micro NRF52840 (BH1750 + SSD1309 OLED)

This project reads ambient light using a BH1750 sensor over I2C, renders lux values on an SSD1309 OLED display, and controls an LED based on hysteresis thresholds. Built with Zephyr RTOS.

## Hardware

| Component | Details |
|-----------|---------|
| Board | Pro Micro NRF52840 |
| Display | SSD1309 OLED 128x64, I2C 0x3C |
| Sensor | BH1750, I2C 0x23 |
| LED | Internal LED on P0.15 |
| I2C | SDA=P1.00, SCL=P0.11, 400kHz |

## Features

- **BH1750 Sensor Reading**: Reads ambient light via Zephyr sensor API
- **Hysteresis Control**: LED ON when lux < 18, LED OFF when lux > 20
- **Sensor Offline Detection**: Displays error when BH1750 unavailable
- **Display Warning**: LED blinks continuously when OLED connection fails
- **Auto Reconnect**: Attempts to reconnect to display on each cycle
- **Mutex-protected State**: Thread-safe shared state between sensor and UI tasks

## Project Structure

```
src/
├── main.c                        # Entry point, creates 2 threads
├── display.c                     # Low-level SSD1309 OLED driver
├── font.c                       # 8x8 font data
├── bh1750/
│   ├── bh1750_reader.c           # BH1750 sensor wrapper
│   └── bh1750_reader.h
├── state/
│   ├── shared_state.c           # Shared state with mutex
│   └── shared_state.h
├── sensor/
│   └── sensor_task.c            # Reads BH1750, hysteresis logic
├── ui/
│   └── ui_task.c               # Renders OLED, LED blink warning
└── display/
    ├── display_manager.c       # High-level display API
    └── display_manager.h
```

## UI Display Layout

```
Line 0: AMBIENT LIGHT   (fixed title)
Line 1: (empty)
Line 2: 123.45 lux    (lux value)
Line 3: [ON ] LED     (LED status indicator)
```

## LED Behavior

| State | LED Behavior |
|-------|------------|
| lux < 18 | LED ON |
| 18 ≤ lux ≤ 20 | Keep previous state (hysteresis) |
| lux > 20 | LED OFF |
| Display error | LED blinks 100ms ON / 100ms OFF |
| Sensor offline | LED reflects hysteresis state |

## Hysteresis Thresholds

| Threshold | Value |
|-----------|-------|
| Lower bound (LED ON) | 18 lux |
| Upper bound (LED OFF) | 20 lux |

## Build & Flash

```bash
# Build
west build -b promicro_nrf52840_nrf52840

# Flash
west flash

# Or use cmake directly
cmake --build build/04_I2CwithLCDscreen
```

## Configuration

### Thread Priorities

| Thread | Priority | Stack Size |
|--------|----------|------------|
| sensor_task | 7 | 1024 |
| ui_task | 6 | 1024 |

### Timing

- Sensor read interval: 1000ms
- UI refresh interval: 500ms
- LED blink rate (warning): 100ms ON / 100ms OFF

## Error Handling

- **Sensor error**: UI displays "SENSOR ERROR" + "BH1750 Offline"
- **Display error**: LED blinks continuously, attempts reconnect each cycle
- **I2C error**: Logged via Zephyr log module

## Logging

Log levels: ERROR, WARNING, INFO (via `LOG_LEVEL_INF`)

```bash
# View serial console
west attach
```

## Future Extentions

- Runtime threshold configuration via UART/CLI
- Multiple sensor support
- Data logging to flash
- Web dashboard via BLE

## License
MIT License