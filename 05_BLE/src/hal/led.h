#ifndef LED_H__
#define LED_H__

#include <stdint.h>

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
void led_blink_fast(uint8_t count);
void led_start_scan_blink(void);
void led_stop_scan_blink(void);

#endif