#ifndef DISPLAY_H__
#define DISPLAY_H__

#include <stdbool.h>

int display_init(void);
void display_set_ble_connected(bool connected);
void display_set_led(bool on);
void display_set_lux(float lux);

#endif