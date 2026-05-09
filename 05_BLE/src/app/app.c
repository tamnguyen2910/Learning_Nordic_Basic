#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "hal/led.h"
#include "drivers/display.h"
#include "drivers/sensor.h"
#include "bluetooth/ble.h"
#include "bluetooth/led_service.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int app_init(void)
{
    led_init();

    if (display_init() < 0) {
        LOG_ERR("Display init failed");
        return -1;
    }

    if (led_service_init() < 0) {
        LOG_ERR("LED service init failed");
        return -1;
    }

    if (sensor_init() < 0) {
        LOG_ERR("Sensor init failed");
        return -1;
    }

    if (ble_init() < 0) {
        LOG_ERR("BLE init failed");
        return -1;
    }

    led_start_scan_blink();

    LOG_INF("Application initialized");
    return 0;
}