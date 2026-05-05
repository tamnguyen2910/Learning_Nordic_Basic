#ifndef FONT_H_
#define FONT_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Font dimensions */
#define FONT_WIDTH  8
#define FONT_HEIGHT 8

/* Display dimensions (for bounds checking) */
#define FONT_DISPLAY_WIDTH 128

/* ASCII range */
#define FONT_FIRST_CHAR 32
#define FONT_LAST_CHAR  126
#define FONT_CHAR_COUNT (FONT_LAST_CHAR - FONT_FIRST_CHAR + 1)

/**
 * @brief Get pixel data for a character
 * @param c ASCII character (32-126)
 * @return Pointer to 8-byte glyph data, NULL if character not supported
 */
const uint8_t *font_get_glyph(char c);

/**
 * @brief Render a character to a buffer
 * @param buf Destination buffer (must be at least 8 bytes)
 * @param buf_size Size of destination buffer
 * @param c Character to render
 * @param x X position in pixels
 * @param y Y position in pixels (page address, 0-7)
 * @return 0 on success, -1 if character not supported
 */
int font_render_char(uint8_t *buf, size_t buf_size, char c, uint16_t x, uint8_t y);

/**
 * @brief Check if character is renderable
 * @param c Character to check
 * @return true if character can be rendered
 */
bool font_is_printable(char c);

#endif /* FONT_H_ */
