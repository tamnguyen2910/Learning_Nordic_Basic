#include "display.h"
#include "font.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
#include <string.h>

#define DISPLAY_NODE DT_NODELABEL(ssd1306)

LOG_MODULE_REGISTER(display, CONFIG_DISPLAY_LOG_LEVEL);

/* Frame buffer - SSD1306 uses 8 pages of 128 bytes each */
static uint8_t frame_buffer[DISPLAY_BUFFER_SIZE];

/* Display device */
static const struct device *display_dev = DEVICE_DT_GET(DISPLAY_NODE);

/* Display initialized flag */
static bool initialized = false;

int display_init(void)
{
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display device not ready");
        return DISPLAY_ERR_DEVICE;
    }

    /* Clear frame buffer */
    memset(frame_buffer, 0, DISPLAY_BUFFER_SIZE);
    initialized = true;
    LOG_INF("Display initialized successfully");
    LOG_INF("Display device: %s", display_dev->name);
    return DISPLAY_SUCCESS;
}

int display_write_text(uint16_t x, uint8_t y, const char *text)
{
    const char *ptr;
    uint16_t cur_x;
    int ret;

    if (!initialized || text == NULL) {
        return DISPLAY_ERR_PARAM;
    }

    if (y >= MAX_LINES) {
        return DISPLAY_ERR_PARAM;
    }

    cur_x = x;
    ptr = text;

    while (*ptr != '\0' && cur_x < DISPLAY_WIDTH) {
        ret = font_render_char(frame_buffer, DISPLAY_BUFFER_SIZE, *ptr, cur_x, y);
        if (ret < 0) {
            /* Skip unprintable character */
        }
        cur_x += FONT_WIDTH;
        ptr++;
    }

    return DISPLAY_SUCCESS;
}

int display_write_int(uint16_t x, uint8_t y, int32_t value)
{
    char buf[12];  /* -2147483648 + null */
    char *ptr = buf + sizeof(buf) - 1;
    bool negative = false;
    int32_t abs_value;
    int len = 0;

    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }

    *ptr = '\0';

    if (value < 0) {
        negative = true;
        abs_value = -value;
    } else {
        abs_value = value;
    }

    if (abs_value == 0) {
        *--ptr = '0';
        len = 1;
    } else {
        while (abs_value > 0) {
            *--ptr = '0' + (abs_value % 10);
            abs_value /= 10;
            len++;
        }
    }

    if (negative) {
        *--ptr = '-';
        len++;
    }

    return display_write_text(x, y, ptr);
}

int display_write_hex(uint16_t x, uint8_t y, uint32_t value)
{
    char buf[9];  /* 8 hex digits + null */
    static const char hex_chars[] = "0123456789ABCDEF";
    int i;

    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }

    buf[8] = '\0';
    for (i = 7; i >= 0; i--) {
        buf[i] = hex_chars[value & 0x0F];
        value >>= 4;
    }

    return display_write_text(x, y, buf);
}

int oled_clear(void)
{
    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }
    memset(frame_buffer, 0, DISPLAY_BUFFER_SIZE);
    return DISPLAY_SUCCESS;
}

int oled_set_contrast(uint8_t contrast)
{
    int ret;
    struct display_capabilities caps;
    
    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }

    display_get_capabilities(display_dev, &caps);
    
    /* Use display_blanking_off to ensure display is on */
    ret = display_blanking_off(display_dev);
    if (ret < 0) {
        LOG_ERR("Failed to turn on display: %d", ret);
        return DISPLAY_ERR_I2C;
    }

    return DISPLAY_SUCCESS;
}

int display_set_inversion(bool invert)
{
    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }

    /* SSD1306 driver doesn't have runtime inversion API */
    /* This would require sending direct commands */
    return DISPLAY_SUCCESS;
}

int display_off(void)
{
    int ret;
    
    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }

    ret = display_blanking_on(display_dev);
    if (ret < 0) {
        LOG_ERR("Failed to turn off display: %d", ret);
        return DISPLAY_ERR_I2C;
    }

    return DISPLAY_SUCCESS;
}

int display_on(void)
{
    int ret;
    
    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }

    ret = display_blanking_off(display_dev);
    if (ret < 0) {
        LOG_ERR("Failed to turn on display: %d", ret);
        return DISPLAY_ERR_I2C;
    }

    return DISPLAY_SUCCESS;
}

int display_update(void)
{
    int ret;
    struct display_buffer_descriptor buf_desc = {
        .buf_size = DISPLAY_BUFFER_SIZE,
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,
        .pitch = DISPLAY_WIDTH,
    };

    if (!initialized) {
        return DISPLAY_ERR_PARAM;
    }

    ret = display_write(display_dev, 0, 0, &buf_desc, frame_buffer);
    if (ret < 0) {
        LOG_ERR("Failed to write display: %d", ret);
        return DISPLAY_ERR_I2C;
    }

    return DISPLAY_SUCCESS;
}

const struct device *display_get_device(void)
{
    return display_dev;
}

uint8_t *display_get_buffer(void)
{
    return frame_buffer;
}
