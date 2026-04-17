# Nordic Basic Learning with ProMicro nRF52840

Hands-on learning repository for Nordic nRF52840 development using nRF Connect SDK (NCS). Structured exercises covering GPIO, UART, TWI/I2C, SPI, PWM, ADC, Bluetooth Low Energy (BLE), and power management on the ProMicro nRF52840 board.

## Target Hardware
- **Board:** ProMicro nRF52840 (or compatible nRF52840 dev board)
- **SDK:** nRF Connect SDK v2.x
- **Toolchain:** Zephyr RTOS, CMake, west

## Structure
| Lesson | Topic |
|--------|-------|
| 01 | GPIO Basics & LED Blink |
| 02 | Button Input & Interrupts |
| 03 | UART Communication |
| 04 | I2C/TWI Sensor Reading |
| 05 | SPI Interface |
| 06 | PWM & LED Dimming |
| 07 | ADC & Battery Monitoring |
| 08 | BLE Peripheral Basics |
| 09 | BLE Services & Characteristics |
| 10 | Power Optimization |

## Prerequisites
- nRF Connect SDK installed
- J-Link or CMSIS-DAP debugger
- Basic C programming knowledge

---
Learning by doing. One peripheral at a time.

---

# 🚀 1. Build firmware

## 🔹 Build nhanh (incremental)

```bash
west build
```

* Chỉ build lại file thay đổi
* Dùng hằng ngày

---

## 🔹 Build lần đầu / chọn board

```bash
west build -b promicro_nrf52840
```

* Chọn board target
* Tạo build config

---

## 🔹 Clean build (build lại từ đầu)

```bash
west build -b promicro_nrf52840/nrf52840 -p always
```

* Xóa build cũ
* Build lại toàn bộ

👉 Dùng khi:

* Lỗi build
* Đổi board
* Đổi prj.conf

---

## 🔹 Build với multi-core (nhanh hơn)

```bash
west build -o=-j8
```

* Dùng 8 core CPU

---

# 📦 2. Output file

Sau khi build:

```
build/zephyr/
```

Các file quan trọng:

* `zephyr.elf` → file debug
* `zephyr.hex` → firmware dạng hex
* `zephyr.bin` → binary
* `zephyr.uf2` → dùng để copy vào USB (UF2 bootloader)

---

## 🔹 Enable UF2 output

Thêm vào `prj.conf`:

```ini
CONFIG_BUILD_OUTPUT_UF2=y
```

---

# 🔥 3. Flash firmware

## 🔹 Flash bằng debugger (JLink)

```bash
west flash
```

---

## 🔹 Flash với runner cụ thể

```bash
west flash --runner jlink
```

---

## 🔹 Flash bằng UF2 (Pro Micro)

Không dùng lệnh.

Thay vào đó:

1. Double reset board
2. Copy file:

```
build/zephyr/zephyr.uf2
```

vào ổ USB

---

# 🧪 4. Debug

## 🔹 Debug trực tiếp

```bash
west debug
```

---

## 🔹 Start debug server

```bash
west debugserver
```

---

# ⚙️ 5. Config & Build system

## 🔹 Mở menu config

```bash
west build -t menuconfig
```

---

## 🔹 Reconfigure project

```bash
west build -t reconfigure
```

---

## 🔹 Clean build folder

```bash
west build -t clean
```

---

# 🔍 6. Quản lý workspace

## 🔹 Init project (lần đầu)

```bash
west init
west update
```

---

## 🔹 Update SDK

```bash
west update
```

---

## 🔹 Xem list module

```bash
west list
```

---

# 📡 7. Xem log UART

```bash
python -m serial.tools.miniterm COM3 115200
```

---

# 🧠 8. Workflow chuẩn

```
Code → west build → zephyr.uf2 → copy vào board → test
```

---

# ⚠️ 9. Lỗi thường gặp

## ❌ Build chậm

→ Do dùng:

```bash
west build -p always
```

---

## ❌ Không có file UF2

→ Quên:

```ini
CONFIG_BUILD_OUTPUT_UF2=y
```

---

## ❌ App không chạy

→ Sai board hoặc sai flash address

---

# 🎯 10. Các lệnh quan trọng nhất

```bash
west build
west build -b promicro_nrf52840
west build -p always
west flash
```

---

# 💡 Ghi nhớ nhanh

* `west build` → build nhanh
* `-p always` → clean build
* `-b` → chọn board
* `uf2` → copy qua USB

---

# 🚀 Kết luận

Chỉ cần nhớ:

```bash
west build
```

→ là đủ cho 90% công việc

---

Chúc bạn build không lỗi 😄
