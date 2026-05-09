cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(05_BLE)

target_include_directories(app PRIVATE
    src
)

target_sources(app PRIVATE
    src/main.c
    src/app/app.c
    src/hal/led.c
    src/drivers/display.c
    src/drivers/font.c
    src/drivers/sensor.c
    src/bluetooth/ble.c
    src/bluetooth/led_service.c
)