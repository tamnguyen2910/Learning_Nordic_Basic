#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include "display.h"
#include "font.h"

LOG_MODULE_REGISTER(disp, LOG_LEVEL_INF);

#define FB_SIZE 1024

static const struct device *display_dev;
static uint8_t fb[FB_SIZE];
static bool ble_connected;
static bool led_on;
static float current_lux;

static void fb_invert_pages(uint8_t start_page, uint8_t num_pages)
{
    for (uint8_t p = start_page; p < start_page + num_pages; p++) {
        for (uint16_t i = p * 128; i < (p + 1) * 128; i++) {
            fb[i] ^= 0xFF;
        }
    }
}

static void display_render(void)
{
    memset(fb, 0, FB_SIZE);

    /* Header bar: pages 0-1 */
    font_render_text(fb, FB_SIZE, "BLE CTRL", 36, 1);
    fb_invert_pages(0, 2);

    /* BLE status: page 3 */
    font_render_text(fb, FB_SIZE, "BLE", 12, 3);
    font_render_text(fb, FB_SIZE,
                     ble_connected ? "Connected" : "Scanning", 44, 3);

    /* LED status: page 4 */
    font_render_text(fb, FB_SIZE, "LED", 12, 4);
    font_render_text(fb, FB_SIZE, led_on ? "ON" : "OFF", 44, 4);

    /* Footer: page 6 - Lux value (black background, no inversion) */
    {
        char buf[24];
        int int_part = (int)current_lux;
        int frac_part = (int)((current_lux - int_part) * 100.0f + 0.5f);
        if (frac_part >= 100) {
            int_part++;
            frac_part = 0;
        }
        snprintf(buf, sizeof(buf), "Lux: %d.%02d", int_part, frac_part);
        font_render_text(fb, FB_SIZE, buf, 12, 6);
    }

    /* Flush to display */
    struct display_buffer_descriptor desc = {
        .buf_size = FB_SIZE,
        .width = 128,
        .height = 64,
        .pitch = 128,
    };
    display_write(display_dev, 0, 0, &desc, fb);
}

int display_init(void)
{
    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display not ready");
        return -1;
    }

    display_blanking_off(display_dev);

    ble_connected = false;
    led_on = false;
    current_lux = 0.0f;
    display_render();

    LOG_INF("Display ready");
    return 0;
}

void display_set_ble_connected(bool connected)
{
    ble_connected = connected;
    display_render();
}

void display_set_led(bool on)
{
    led_on = on;
    display_render();
}

void display_set_lux(float lux)
{
    current_lux = lux;
    display_render();
}