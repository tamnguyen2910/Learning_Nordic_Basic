#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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

static void fb_invert_pages(uint8_t start_page, uint8_t num_pages)
{
    for (uint8_t p = start_page; p < start_page + num_pages; p++) {
        for (uint16_t i = p * 128; i < (p + 1) * 128; i++) {
            fb[i] ^= 0xFF;
        }
    }
}

static void fb_fill_rect(uint8_t page, uint16_t x, uint8_t w, uint8_t h)
{
    for (uint8_t col = x; col < x + w && col < 128; col++) {
        fb[page * 128 + col] |= (0xFF >> (8 - h));
    }
}

static void display_render(void)
{
    memset(fb, 0, FB_SIZE);

    /* Header bar: pages 0-1 */
    font_render_text(fb, FB_SIZE, "BLE CTRL", 36, 1);
    fb_invert_pages(0, 2);

    /* BLE status: page 3 */
    if (ble_connected) {
        fb_fill_rect(3, 4, 6, 6);
    }
    font_render_text(fb, FB_SIZE, "BLE", 12, 3);
    font_render_text(fb, FB_SIZE,
                     ble_connected ? "Connected" : "Scanning", 44, 3);

    /* LED status: page 4 */
    if (led_on) {
        fb_fill_rect(4, 4, 6, 6);
    }
    font_render_text(fb, FB_SIZE, "LED", 12, 4);
    font_render_text(fb, FB_SIZE, led_on ? "ON" : "OFF", 44, 4);

    /* Footer bar: pages 6-7 */
    font_render_text(fb, FB_SIZE, "promicro", 36, 6);
    fb_invert_pages(6, 2);

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
