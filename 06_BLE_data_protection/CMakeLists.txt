cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(06_BLE_data_protection)

# NUS needs the bt (Bluetooth) library — Zephyr auto-links when CONFIG_BT_NUS=y

target_sources(app PRIVATE src/main.c)