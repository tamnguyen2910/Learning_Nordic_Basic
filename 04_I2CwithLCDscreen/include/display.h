#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>

/* Display dimensions */
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      64
#define DISPLAY_BUFFER_SIZE ((DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8)

/* Font dimensions */
#define FONT_WIDTH  8
#define FONT_HEIGHT 8

/* Maximum characters per line */
#define MAX_CHARS_PER_LINE (DISPLAY_WIDTH / FONT_WIDTH)
#define MAX_LINES          (DISPLAY_HEIGHT / FONT_HEIGHT)

/* Error codes */
#define DISPLAY_SUCCESS         0
#define DISPLAY_ERR_DEVICE     -1
#define DISPLAY_ERR_I2C        -2
#define DISPLAY_ERR_TIMEOUT    -3
#define DISPLAY_ERR_PARAM      -4

/**
 * @brief Initialize the display subsystem
 * @return 0 on success, negative error code on failure
 */
int display_init(void);

/**
 * @brief Write text at specified position
 * @param x X coordinate in pixels (0-127)
 * @param y Y coordinate in pages (0-7, each page is 8 pixels)
 * @param text Null-terminated string
 * @return 0 on success, negative error code on failure
 */
int display_write_text(uint16_t x, uint8_t y, const char *text);

/**
 * @brief Write integer at specified position
 * @param x X coordinate in pixels
 * @param y Y coordinate in pages
 * @param value Integer value to display
 * @return 0 on success, negative error code on failure
 */
int display_write_int(uint16_t x, uint8_t y, int32_t value);

/**
 * @brief Write hex value at specified position
 * @param x X coordinate in pixels
 * @param y Y coordinate in pages
 * @param value Hex value to display
 * @return 0 on success, negative error code on failure
 */
int display_write_hex(uint16_t x, uint8_t y, uint32_t value);

/**
 * @brief Clear the display
 * @return 0 on success, negative error code on failure
 */

/**
 * @brief Set display contrast
 * @param contrast Contrast level (0-255)
 * @return 0 on success, negative error code on failure
 */
int oled_clear(void);

/**
 * @brief Set display contrast (user function, tránh xung đột với Zephyr API)
 * @param contrast Contrast level (0-255)
 * @return 0 on success, negative error code on failure
 */
int oled_set_contrast(uint8_t contrast);

/**
 * @brief Set display inversion mode
 * @param invert true to invert display, false for normal
 * @return 0 on success, negative error code on failure
 */
int display_set_inversion(bool invert);

/**
 * @brief Turn display off (low power)
 * @return 0 on success, negative error code on failure
 */
int display_off(void);

/**
 * @brief Turn display on
 * @return 0 on success, negative error code on failure
 */
int display_on(void);

/**
 * @brief Get display device pointer
 * @return Pointer to display device, NULL if not initialized
 */
const struct device *display_get_device(void);

/**
 * @brief Update display from buffer
 * @return 0 on success, negative error code on failure
 */
int display_update(void);

/**
 * @brief Get frame buffer pointer
 * @return Pointer to frame buffer
 */
uint8_t *display_get_buffer(void);

#endif /* DISPLAY_H_ */
