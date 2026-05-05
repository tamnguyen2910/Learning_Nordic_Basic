/* Move sensor_task into new module path */
#include <zephyr/kernel.h>
/* Logging and BH1750 interface */
#include <zephyr/logging/log.h>
#include "bh1750/bh1750_reader.h"
#include "state/shared_state.h"

LOG_MODULE_REGISTER(sensor_task, LOG_LEVEL_INF);

#define LOWER_BOUND (18 * 1000000)
#define UPPER_BOUND (20 * 1000000)
#define READ_INTERVAL_MS 1000

void sensor_task(void *arg1, void *arg2, void *arg3) {
    (void)arg1; (void)arg2; (void)arg3;

    LOG_INF("Sensor task started");
    int ret = bh1750_reader_init();
    if (ret != 0) {
        LOG_ERR("BH1750 init failed: %d", ret);
    } else {
        LOG_INF("BH1750 initialized");
    }

    while (1) {
        int32_t lux_fixed = 0;
        int ret = bh1750_reader_read_lux(&lux_fixed);
        k_mutex_lock(&g_state_mutex, K_FOREVER);
        if (ret == 0) {
            g_shared_state.sensor_ok = true;
            g_shared_state.lux_fixed = lux_fixed;
            if (lux_fixed < LOWER_BOUND) {
                g_shared_state.led_on = true;
            } else if (lux_fixed > UPPER_BOUND) {
                g_shared_state.led_on = false;
            }
        } else {
            g_shared_state.sensor_ok = false;
            LOG_ERR("BH1750 read failed: %d", ret);
            k_msleep(500);
        }
        k_mutex_unlock(&g_state_mutex);
        k_msleep(READ_INTERVAL_MS);
    }
}
