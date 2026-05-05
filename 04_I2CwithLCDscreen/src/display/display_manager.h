#ifndef DISPLAY_MANAGER_H_
#define DISPLAY_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

int display_manager_init(void);
int display_manager_render(int32_t lux_fixed, bool led_on, bool sensor_ok);

#endif /* DISPLAY_MANAGER_H_ */
