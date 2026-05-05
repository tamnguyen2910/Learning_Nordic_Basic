#ifndef SHARED_STATE_H_
#define SHARED_STATE_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int32_t lux_fixed; /* micro-lux */
    bool led_on;       /* LED state derived from lux */
    bool sensor_ok;    /* BH1750 sensor status; true if last read succeeded */
} shared_state_t;

extern struct k_mutex g_state_mutex;
extern shared_state_t g_shared_state;

#endif /* SHARED_STATE_H_ */
