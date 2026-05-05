/* Use Zephyr kernel and sensor API with compatibility shims */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "bh1750_reader.h"

LOG_MODULE_REGISTER(bh1750_reader, LOG_LEVEL_INF);

static const struct device *bh1750_dev = NULL;

int bh1750_reader_init(void)
{
    bh1750_dev = DEVICE_DT_GET(DT_NODELABEL(bh1750));
    if (!device_is_ready(bh1750_dev)) {
        LOG_ERR("BH1750 device not ready");
        return -ENODEV;
    }
    LOG_INF("BH1750 initialized");
    return 0;
}

int bh1750_reader_read_lux(int32_t *lux_fixed)
{
    if (!bh1750_dev || !device_is_ready(bh1750_dev)) {
        return -ENODEV;
    }
    int ret = 0;
    struct sensor_value lux;
    ret = sensor_sample_fetch(bh1750_dev);
    if (ret != 0) {
        return ret;
    }
    ret = sensor_channel_get(bh1750_dev, SENSOR_CHAN_LIGHT, &lux);
    if (ret != 0) {
        return ret;
    }
    *lux_fixed = lux.val1 * 1000000 + lux.val2; /* fixed-point micro-lux */
    return 0;
}
