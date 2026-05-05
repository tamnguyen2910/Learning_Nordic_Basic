cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf52840_ssd1309_display)

# Display subsystem sources
target_sources(app PRIVATE
    src/main.c
    src/font.c
    src/display.c
    src/bh1750/bh1750_reader.c
    src/state/shared_state.c
    src/sensor/sensor_task.c
    src/ui/ui_task.c
    src/display/display_manager.c
)

# Include directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bh1750
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display
)
