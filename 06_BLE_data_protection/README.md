# BLE UART Terminal — nRF52840 (Zephyr / NCS v3.2.4)

Onboard LED control over Bluetooth LE using the **Nordic UART Service (NUS)**.

## Features

- Control onboard LED via BLE: send `LED_ON` / `LED_OFF` from a phone app
- nRF52840 responds with status: `LED ON\r\n` / `LED OFF\r\n`
- Works with any BLE UART app (nRF Connect, Serial Bluetooth Terminal, LightBlue...)

## BLE Protocol

| Item | UUID | Details |
|---|---|---|
| **Service** | `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` | Nordic UART Service |
| **TX Char** (nRF → Phone) | `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` | **Notify** — send responses |
| **RX Char** (Phone → nRF) | `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` | **Write Without Response** — receive commands |

## Security (Current Version)

**NO security enabled.**
- `CONFIG_BT_SMP=n` — Security Manager is disabled
- Data is transmitted in **plaintext**, no encryption
- Pairing / Bonding is not used
- Any BLE sniffer can read commands and responses

Details in [prj.conf](prj.conf) lines 16-17.

## Hardware

- **Board:** Pro Micro nRF52840
- **MCU:** nRF52840 (ARM Cortex-M4F @ 64MHz)
- **Onboard LED:** GPIO P0.15 (active HIGH)
- **Crypto accelerator:** CC310 (AES-128 + ECDH P-256) — not used yet

## Software Requirements

| Tool | Version |
|---|---|
| nRF Connect SDK | 3.2.4 |
| Zephyr RTOS | 4.2.99 |
| ARM GCC (GNU Arm Embedded) | `C:\ncs\toolchains\fd21892d0f\opt\bin` |
| CMake | ≥ 3.20.0 |
| Ninja | — |

## Build & Flash

### Build

```batch
build.bat
```

Or build + copy directly to UF2 drive:

```batch
build_and_copy.bat
```

(Drive `E:` is mounted by the Pro Micro nRF52840 UF2 bootloader)

### Output

- `build/06_BLE_data_protection/zephyr/zephyr.uf2` — drag-and-drop to flash
- `build/06_BLE_data_protection/zephyr/zephyr.hex` — use with nRF Programmer

## Directory Structure

```
06_BLE_data_protection/
├── CMakeLists.txt              # Build configuration
├── prj.conf                    # Kconfig project config
├── build_and_copy.bat          # Build + copy UF2 to E: drive
├── build.bat                   # Build only
├── promicro_nrf52840_nrf52840.overlay  # Devicetree overlay (currently empty)
├── src/
│   └── main.c                  # Application code (282 lines)
└── README.md                   # This file
```

## Log

Project uses the **Zephyr log system**, output to UART console:

```
*** Booting Zephyr OS build ncs-v3.2.4 ***
[00:00:00.001,068] <inf> main: === BLE UART Terminal ===
[00:00:00.001,098] <inf> main: NUS initialized
[00:00:00.001,129] <inf> main: BLE initialized
[00:00:00.001,129] <inf> main: Scanning
[00:00:00.799,255] <inf> main: Connected!
[00:00:05.123,456] <inf> main: Received: LED_ON
[00:00:05.123,567] <inf> main: LED ON
```

## References

- [nRF Connect SDK Documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/)
- [Zephyr Project Documentation](https://docs.zephyrproject.org/latest/)
- [Nordic UART Service](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/libraries/bluetooth_services/nus.html)
- [Pro Micro nRF52840](https://www.sparkfun.com/products/18288)
