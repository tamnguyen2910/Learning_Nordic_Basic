#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "sensor.h"
#include "display.h"
#include "bluetooth/led_service.h"

LOG_MODULE_REGISTER(sensor, LOG_LEVEL_INF);

static const struct device *bh1750_dev;
static struct k_work_delayable sensor_work;

static void sensor_work_handler(struct k_work *work)
{
    struct sensor_value lux;
    int ret;
    uint8_t retry = 3;

    while (retry > 0) {
        ret = sensor_sample_fetch(bh1750_dev);
        if (ret == 0) {
            ret = sensor_channel_get(bh1750_dev, SENSOR_CHAN_LIGHT, &lux);
            if (ret == 0) {
                float lux_f = (float)lux.val1 + (float)lux.val2 / 1000000.0f;
                display_set_lux(lux_f);
                uint32_t lux_x100 = (uint32_t)(lux_f * 100.0f + 0.5f);
                led_service_send_lux(lux_x100);
                break;
            }
        }
        retry--;
        if (retry > 0) {
            LOG_WRN("Sensor read failed, retry %d/3", 3 - retry);
            k_msleep(50);
        }
    }

    if (retry == 0) {
        LOG_ERR("Sensor read failed after 3 retries");
    }

    k_work_schedule(&sensor_work, K_SECONDS(1));
}

int sensor_init(void)
{
    bh1750_dev = DEVICE_DT_GET(DT_NODELABEL(bh1750));
    if (!device_is_ready(bh1750_dev)) {
        LOG_ERR("BH1750 not ready");
        return -1;
    }

    k_work_init_delayable(&sensor_work, sensor_work_handler);
    k_work_schedule(&sensor_work, K_SECONDS(1));

    LOG_INF("BH1750 ready");
    return 0;
}