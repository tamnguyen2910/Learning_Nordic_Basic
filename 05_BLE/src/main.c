#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "led.h"
#include "ble.h"
#include "led_service.h"
#include "display.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
    led_init();
    display_init();
    led_service_init();
    ble_init();
    led_start_scan_blink();

    return 0;
}