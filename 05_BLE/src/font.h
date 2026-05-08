#ifndef FONT_H__
#define FONT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define FONT_WIDTH          8
#define FONT_DISPLAY_WIDTH  128
#define FONT_FIRST_CHAR     32
#define FONT_LAST_CHAR      126
#define FONT_CHAR_COUNT     95

const uint8_t *font_get_glyph(char c);
int font_render_char(uint8_t *buf, size_t buf_size, char c, uint16_t x, uint8_t page);
int font_render_text(uint8_t *buf, size_t buf_size, const char *text, uint16_t x, uint8_t page);
bool font_is_printable(char c);

#endif
