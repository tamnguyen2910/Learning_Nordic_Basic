#include <zephyr/kernel.h>
#include <stdbool.h>
#include <zephyr/logging/log.h>
#include "state/shared_state.h"
#include "display/display_manager.h"
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(ui_task, LOG_LEVEL_INF);

#define LED_PIN 15
static const struct device *led_gpio = NULL;

static int ui_init(void) {
#if defined(DT_NODELABEL) && defined(DT_NODE_HAS_STATUS) && DT_NODE_HAS_STATUS(DT_NODELABEL(gpio0), okay)
    led_gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    if (led_gpio && device_is_ready(led_gpio)) {
        int ret = gpio_pin_configure(led_gpio, LED_PIN, GPIO_OUTPUT);
        if (ret != 0) {
            LOG_ERR("Failed to configure LED GPIO: %d", ret);
            return ret;
        }
        gpio_pin_set(led_gpio, LED_PIN, 0);
        LOG_INF("LED GPIO configured");
        return 0;
    } else {
        LOG_WRN("LED GPIO device not ready");
        led_gpio = NULL;
        return 0;
    }
#else
    LOG_WRN("gpio0 not available in DT; LED control disabled");
    led_gpio = NULL;
    return 0;
#endif
}

void ui_task(void *arg1, void *arg2, void *arg3) {
    (void)arg1; (void)arg2; (void)arg3;
    bool display_ok = true;

    LOG_INF("UI task started");
    ui_init();
    int ret = display_manager_init();
    if (ret != 0) {
        LOG_ERR("Display init failed: %d", ret);
        display_ok = false;
    }

    while (1) {
        int32_t lux_fixed;
        bool led_on;
        bool sensor_ok;

        k_mutex_lock(&g_state_mutex, K_FOREVER);
        lux_fixed = g_shared_state.lux_fixed;
        led_on = g_shared_state.led_on;
        sensor_ok = g_shared_state.sensor_ok;
        k_mutex_unlock(&g_state_mutex);

        if (display_ok) {
            ret = display_manager_render(lux_fixed, led_on, sensor_ok);
            if (ret != 0) {
                LOG_ERR("Display render failed: %d", ret);
                display_ok = false;
            }
        } else {
            ret = display_manager_init();
            if (ret != 0) {
                LOG_WRN("Display reconnect failed: %d", ret);
            } else {
                LOG_INF("Display reconnected");
                display_ok = true;
            }
        }

        if (!display_ok) {
            if (led_gpio) {
                gpio_pin_set(led_gpio, LED_PIN, 1);
            }
            k_msleep(100);
            if (led_gpio) {
                gpio_pin_set(led_gpio, LED_PIN, 0);
            }
            k_msleep(100);
        } else {
            if (led_gpio) {
                gpio_pin_set(led_gpio, LED_PIN, led_on ? 1 : 0);
            }
            k_msleep(500);
        }
    }
}
